#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <tqprocess.h>
#include <tqstringlist.h>
#include <tqstring.h>
#include <tqobject.h>
#include <tqvaluevector.h>

#include <memory>

#include "appsettings.h"

class BenchmarkProgressBar;

class Benchmark : public TQObject
{
    TQ_OBJECT

public:
    Benchmark();
    ~Benchmark();

    TQString getFIOVersion();
    bool isFIODetected();

    void setDir(const TQString &dir);
    TQString getBenchmarkFile();

    void setRunning(bool state);
    bool isRunning();

    void runBenchmark(TQValueList< std::pair<std::pair<Global::BenchmarkTest, Global::BenchmarkIOReadWrite>, TQValueVector<BenchmarkProgressBar*>> > tests);

    struct PerformanceResult
    {
        float Bandwidth;
        float IOPS;
        float Latency;

        PerformanceResult operator+ (const PerformanceResult& rhs)
        {
            return *this += rhs;
        }

        PerformanceResult operator+= (const PerformanceResult& rhs)
        {
            Bandwidth += rhs.Bandwidth;
            IOPS += rhs.IOPS;
            Latency += rhs.Latency;
            return *this;
        }

        PerformanceResult operator/ (const unsigned int rhs) const
        {
            if (rhs == 0) return *this;

            return PerformanceResult { Bandwidth / rhs, IOPS / rhs, Latency / rhs };
        }

        PerformanceResult operator* (const unsigned int rhs) const
        {
            return PerformanceResult { Bandwidth * rhs, IOPS * rhs, Latency * rhs };
        }

        void updateWithBetterValues(const PerformanceResult& result) {
            Bandwidth = Bandwidth < result.Bandwidth ? result.Bandwidth : Bandwidth;
            IOPS = IOPS < result.IOPS ? result.IOPS : IOPS;
            if (Latency == 0) Latency = result.Latency;
            Latency = Latency > result.Latency ? result.Latency : Latency;
        }
    };

    struct ParsedJob
    {
        PerformanceResult read, write;
    };

private:
    bool m_running;
    TQString m_FIOVersion;
    TQValueVector<BenchmarkProgressBar*> m_progressBars;
    TQString m_dir;

private:
    void startTest(int blockSize, int queueDepth, int threads, const TQString &rw, const TQString &statusMessage);
    Benchmark::ParsedJob parseResult(const TQString &output, const TQString &errorOutput);
    void sendResult(const Benchmark::PerformanceResult &result, const int index);

    void initSession();
    void prepareDirectory(const TQString &benchmarkFile);
    void prepareFile(const TQString &benchmarkFile, int fileSize);

signals:
    void runningStateChanged(bool);
    void currentTestRunning(Global::BenchmarkTest);
    void testRowAdvanced();
    void passStarted();
    void resultReady(BenchmarkProgressBar*, const Benchmark::PerformanceResult&);
    void benchmarkStatusUpdate(const TQString &);
    void failed(const TQString &);
    void finished();

};

#endif // BENCHMARK_H
