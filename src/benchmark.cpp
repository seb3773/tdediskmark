#include "benchmark.h"
#include "global.h"
#include "mainwindow.h"
#include <tdeapplication.h>
#include <tqregexp.h>
#include <ntqdatetime.h>
Benchmark::Benchmark()
{
    m_running = false;

    TQProcess process;
    process.addArgument("fio");
    process.addArgument("--version");
    process.start();
    
    while(process.isRunning()) {
        tqApp->processEvents();
    }

    m_FIOVersion = TQString(process.readStdout()).simplifyWhiteSpace();
}

Benchmark::~Benchmark() {}

TQString Benchmark::getFIOVersion()
{
    return m_FIOVersion;
}

bool Benchmark::isFIODetected()
{
    return m_FIOVersion.find("fio-") == 0;
}

void Benchmark::setDir(const TQString &dir)
{
    m_dir = dir;
}

TQString Benchmark::getBenchmarkFile()
{
    return m_dir;
}

void Benchmark::setRunning(bool state)
{
    if (m_running != state)
    {
        m_running = state;
        emit runningStateChanged(m_running);
    }
}

bool Benchmark::isRunning()
{
    return m_running;
}

void Benchmark::startTest(int blockSize, int queueDepth, int threads, const TQString &rw, const TQString &statusMessage)
{
    const AppSettings settings;

    PerformanceResult totalRead { 0, 0, 0 }, totalWrite { 0, 0, 0 };
    unsigned int index = 0;

    for (int i = 0; i < settings.getLoopsCount(); i++) {
        if (!m_running) break;

        emit benchmarkStatusUpdate(statusMessage.arg(index + 1).arg(settings.getLoopsCount()));
        
        if (settings.getFlushingCacheState()) {
            TQProcess flushProc;
            flushProc.addArgument("tdesu");
            flushProc.addArgument("-d");
            flushProc.addArgument("--comment");
            flushProc.addArgument(TQObject::tr("tdeDiskMark: Clearing RAM caches for accurate measurements"));
            flushProc.addArgument("-c");
            flushProc.addArgument("sync; echo 3 > /proc/sys/vm/drop_caches");
            flushProc.start();
            while(flushProc.isRunning() && m_running) tqApp->processEvents();

            // Allow tdesud daemon to stabilize its credential cache socket
            TQTime t;
            t.start();
            while (t.elapsed() < 500 && m_running) {
                tqApp->processEvents(100);
            }
        }

        if (!isRunning()) return;

        TQStringList args;
        args << "--name=" + rw << "--filename=" + m_dir + "/tdediskmark.tmp" << "--output-format=json";
        args << "--ioengine=libaio" << "--randrepeat=0" << "--end_fsync=1";
        args << TQString("--size=%1M").arg(settings.getFileSize());
        args << TQString("--bs=%1K").arg(blockSize);
        args << TQString("--iodepth=%1").arg(queueDepth);
        args << TQString("--numjobs=%1").arg(threads);
        args << TQString("--rw=%1").arg(rw);
        
        if (rw.contains("rw")) args << TQString("--rwmixread=%1").arg(settings.getRandomReadPercentage());
        args << TQString("--zero_buffers=%1").arg(settings.getBenchmarkTestData() == Global::BenchmarkTestData::Zeros ? 1 : 0);
        args << TQString("--direct=%1").arg(settings.getCacheBypassState() ? 1 : 0);
        
        args << "--runtime=" + TQString::number(settings.getMeasuringTime());



        TQProcess proc;
        proc.addArgument("tdesu");
        proc.addArgument("-d");
        proc.addArgument("--comment");
        proc.addArgument(TQObject::tr("tdeDiskMark: Running FIO Benchmark"));
        proc.addArgument("-c");
        proc.addArgument("fio " + args.join(" "));
        proc.start();

        while (proc.isRunning() && m_running) {
            tqApp->processEvents();
        }

        if (!m_running) {
            proc.kill();
            break;
        }

        index++;
        TQString output = TQString(proc.readStdout());
        TQString errorOutput = TQString(proc.readStderr());

        auto result = parseResult(output, errorOutput);

        switch (settings.getPerformanceProfile())
        {
            case Global::PerformanceProfile::Default:
                totalRead  += result.read;
                totalWrite += result.write;
            break;
            case Global::PerformanceProfile::Peak:
            case Global::PerformanceProfile::RealWorld:
            case Global::PerformanceProfile::Demo:
                totalRead.updateWithBetterValues(result.read);
                totalWrite.updateWithBetterValues(result.write);
            break;
        }

        if (rw.contains("read")) {
            sendResult(totalRead, index);
        }
        else if (rw.contains("write")) {
            sendResult(totalWrite, index);
        }
        else if (rw.contains("rw")) {
            float p = settings.getRandomReadPercentage();
            sendResult((totalRead * p + totalWrite * (100.f - p)) / 100.f, index);
        }
    }

    if (!m_running) return;

    if (rw.contains("read")) {
        sendResult(totalRead, index);
    }
    else if (rw.contains("write")) {
        sendResult(totalWrite, index);
    }
    else if (rw.contains("rw")) {
        float p = settings.getRandomReadPercentage();
        sendResult((totalRead * p + totalWrite * (100.f - p)) / 100.f, index);
    }
}

void Benchmark::sendResult(const Benchmark::PerformanceResult &result, const int index)
{
    const AppSettings settings;
    if (settings.getPerformanceProfile() == Global::PerformanceProfile::Default) {
        for (unsigned int j = 0; j < m_progressBars.count(); j++) {
            emit resultReady(m_progressBars[j], result / index);
        }
    }
    else {
        for (unsigned int j = 0; j < m_progressBars.count(); j++) {
            emit resultReady(m_progressBars[j], result);
        }
    }
}

Benchmark::ParsedJob Benchmark::parseResult(const TQString &output, const TQString &errorOutput)
{
    ParsedJob parsedJob {{0, 0, 0}, {0, 0, 0}};

    auto extractVal = [](const TQString& out, const TQString& key) -> float {
        TQRegExp rx("\"" + key + "\"\\s*:\\s*([0-9.]+)");
        if (rx.search(out) != -1) {
            return rx.cap(1).toFloat();
        }
        return 0;
    };

    auto extractLat = [](const TQString& out, const TQString& context) -> float {
        // Find context block e.g. "read" : { ... "lat_ns" : { "mean" : 123
        int pos = out.find("\"" + context + "\"");
        if (pos == -1) return 0;
        
        TQRegExp rx("\"lat_ns\"\\s*:\\s*\\{[^}]*\"mean\"\\s*:\\s*([0-9.]+)");
        if (rx.search(out, pos) != -1) {
            return rx.cap(1).toFloat() / 1000.0f; // raw to us
        }
        return 0;
    };

    parsedJob.read.Bandwidth = extractVal(output, "read.*\"bw\""); 
    // Wait regex syntax in TQt3 TQRegExp is limited. Better to just slice.
    
    int readPos = -1;
    TQRegExp readNodeRx("\"read\"\\s*:\\s*\\{");
    if (readNodeRx.search(output) != -1) {
        readPos = readNodeRx.pos();
        TQRegExp bwRx("\"bw\"\\s*:\\s*([0-9.]+)");
        if (bwRx.search(output, readPos) != -1) parsedJob.read.Bandwidth = bwRx.cap(1).toFloat() / 1024.0f; // KiB to MiB
        
        TQRegExp iopsRx("\"iops\"\\s*:\\s*([0-9.]+)");
        if (iopsRx.search(output, readPos) != -1) parsedJob.read.IOPS = iopsRx.cap(1).toFloat();
        
        parsedJob.read.Latency = extractLat(output, "read");
    }

    int writePos = -1;
    TQRegExp writeNodeRx("\"write\"\\s*:\\s*\\{");
    if (writeNodeRx.search(output) != -1) {
        writePos = writeNodeRx.pos();
        TQRegExp bwRx("\"bw\"\\s*:\\s*([0-9.]+)");
        if (bwRx.search(output, writePos) != -1) parsedJob.write.Bandwidth = bwRx.cap(1).toFloat() / 1024.0f; // KiB to MiB
        
        TQRegExp iopsRx("\"iops\"\\s*:\\s*([0-9.]+)");
        if (iopsRx.search(output, writePos) != -1) parsedJob.write.IOPS = iopsRx.cap(1).toFloat();
        
        parsedJob.write.Latency = extractLat(output, "write");
    }

    return parsedJob;
}

void Benchmark::runBenchmark(TQValueList< std::pair<std::pair<Global::BenchmarkTest, Global::BenchmarkIOReadWrite>, TQValueVector<BenchmarkProgressBar*>> > tests)
{
    setRunning(true);

    const AppSettings settings;
    prepareFile(m_dir + "/tdediskmark.tmp", settings.getFileSize());

    bool firstTest = true;
    for (const auto& test : tests) {
        if (!m_running) break;

        if (!firstTest) {
            int interval = settings.getIntervalTime();
            for (int w = interval; w > 0; --w) {
                if (!m_running) break;
                emit benchmarkStatusUpdate(TQObject::tr("Waiting (%1s)").arg(w));
                
                TQTime t;
                t.start();
                while (t.elapsed() < 1000 && m_running) {
                    tqApp->processEvents(100);
                }
            }
        }
        firstTest = false;
        if (!m_running) break;

        emit testRowAdvanced();
        m_progressBars = test.second;
        
        auto mode = test.first.first;
        auto rw = test.first.second;

        emit currentTestRunning(mode);

        const AppSettings settings;
        Global::BenchmarkParams params = settings.getBenchmarkParams(mode, settings.getPerformanceProfile());

        TQString fio_rw;
        if (rw == Global::BenchmarkIOReadWrite::Read) {
            if (params.Pattern == Global::BenchmarkIOPattern::SEQ) fio_rw = "read";
            else fio_rw = "randread";
        } else if (rw == Global::BenchmarkIOReadWrite::Write) {
            if (params.Pattern == Global::BenchmarkIOPattern::SEQ) fio_rw = "write";
            else fio_rw = "randwrite";
        } else {
            if (params.Pattern == Global::BenchmarkIOPattern::SEQ) fio_rw = "rw";
            else fio_rw = "randrw";
        }

        TQString statusTemplate;
        if (params.Pattern == Global::BenchmarkIOPattern::SEQ) {
            statusTemplate = "SEQ%1M Q%2 T%3 (%4/%5)";
        } else {
            statusTemplate = "RND%1K Q%2 T%3 (%4/%5)";
        }
        int bs = params.BlockSize;
        statusTemplate = statusTemplate.arg(bs >= 1024 ? bs / 1024 : bs).arg(params.Queues).arg(params.Threads);

        startTest(params.BlockSize, params.Queues, params.Threads, fio_rw, statusTemplate);
    }

    setRunning(false);
    emit finished();
}

void Benchmark::prepareFile(const TQString &benchmarkFile, int fileSize)
{
    const AppSettings settings;
    bool fillZeros = settings.getBenchmarkTestData() == Global::BenchmarkTestData::Zeros;

    emit benchmarkStatusUpdate(TQObject::tr("Preparation..."));

    TQProcess proc;
    proc.addArgument("tdesu");
    proc.addArgument("-d");
    proc.addArgument("--comment");
    proc.addArgument(TQObject::tr("tdeDiskMark: Preparing Test Payload File"));
    proc.addArgument("-c");
    
    TQStringList args;
    args << "--output-format=json" << "--create_only=1";
    args << TQString("--filename=%1").arg(benchmarkFile);
    args << TQString("--size=%1m").arg(fileSize);
    args << TQString("--zero_buffers=%1").arg(fillZeros ? 1 : 0);
    args << "--name=prepare";
    
    proc.addArgument("fio " + args.join(" "));
    proc.start();
    
    while(proc.isRunning() && m_running) tqApp->processEvents();

    // Allow tdesud daemon to stabilize its credential cache socket
    TQTime t;
    t.start();
    while (t.elapsed() < 500 && m_running) {
        tqApp->processEvents(100);
    }
}
