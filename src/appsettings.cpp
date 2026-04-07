#include "appsettings.h"
#include <tdeapplication.h>
#include <tdeconfig.h>
#include <tdeglobal.h>

AppSettings::AppSettings(TQObject *parent)
    : TQObject(parent)
    , m_settings(TDEGlobal::config())
{
}

void AppSettings::setupLocalization()
{
}

Global::PerformanceProfile AppSettings::getPerformanceProfile() const
{
    m_settings->setGroup("Benchmark");
    return (Global::PerformanceProfile)m_settings->readNumEntry("PerformanceProfile", (int)defaultPerformanceProfile());
}

void AppSettings::setPerformanceProfile(Global::PerformanceProfile performanceProfile)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("PerformanceProfile", (int)performanceProfile);
}

Global::PerformanceProfile AppSettings::defaultPerformanceProfile()
{
    return Global::PerformanceProfile::Default;
}

bool AppSettings::getMixedState() const
{
    m_settings->setGroup("Benchmark");
    return m_settings->readBoolEntry("Mixed", defaultMixedState());
}

void AppSettings::setMixedState(bool state)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("Mixed", state);
}

bool AppSettings::defaultMixedState()
{
    return false;
}

Global::BenchmarkMode AppSettings::getBenchmarkMode() const
{
    m_settings->setGroup("Benchmark");
    return (Global::BenchmarkMode)m_settings->readNumEntry("Mode", (int)defaultBenchmarkMode());
}

void AppSettings::setBenchmarkMode(Global::BenchmarkMode benchmarkMode)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("Mode", (int)benchmarkMode);
}

Global::BenchmarkMode AppSettings::defaultBenchmarkMode()
{
    return Global::BenchmarkMode::ReadWriteMix;
}

Global::BenchmarkTestData AppSettings::getBenchmarkTestData() const
{
    m_settings->setGroup("Benchmark");
    return (Global::BenchmarkTestData)m_settings->readNumEntry("TestData", (int)defaultBenchmarkTestData());
}

void AppSettings::setBenchmarkTestData(Global::BenchmarkTestData benchmarkTestData)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("TestData", (int)benchmarkTestData);
}

Global::BenchmarkTestData AppSettings::defaultBenchmarkTestData()
{
    return Global::BenchmarkTestData::Random;
}

int AppSettings::getLoopsCount() const
{
    m_settings->setGroup("Benchmark");
    return m_settings->readNumEntry("LoopsCount", defaultLoopsCount());
}

void AppSettings::setLoopsCount(int loopsCount)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("LoopsCount", loopsCount);
}

int AppSettings::defaultLoopsCount()
{
    return 5;
}

int AppSettings::getFileSize() const
{
    m_settings->setGroup("Benchmark");
    return m_settings->readNumEntry("FileSize", defaultFileSize());
}

void AppSettings::setFileSize(int fileSize)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("FileSize", fileSize);
}

int AppSettings::defaultFileSize()
{
    return 1024;
}

int AppSettings::getMeasuringTime() const
{
    m_settings->setGroup("Benchmark");
    return m_settings->readNumEntry("MeasuringTime", defaultMeasuringTime());
}

void AppSettings::setMeasuringTime(int measuringTime)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("MeasuringTime", measuringTime);
}

int AppSettings::defaultMeasuringTime()
{
    return 5;
}

int AppSettings::getIntervalTime() const
{
    m_settings->setGroup("Benchmark");
    return m_settings->readNumEntry("IntervalTime", defaultIntervalTime());
}

void AppSettings::setIntervalTime(int intervalTime)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("IntervalTime", intervalTime);
}

int AppSettings::defaultIntervalTime()
{
    return 5;
}

int AppSettings::getRandomReadPercentage() const
{
    m_settings->setGroup("Benchmark");
    return m_settings->readNumEntry("RandomReadPercentage", defaultRandomReadPercentage());
}

void AppSettings::setRandomReadPercentage(int randomReadPercentage)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("RandomReadPercentage", randomReadPercentage);
}

int AppSettings::defaultRandomReadPercentage()
{
    return 70;
}

bool AppSettings::getCacheBypassState() const
{
    m_settings->setGroup("Benchmark");
    return m_settings->readBoolEntry("BypassCache", defaultCacheBypassState());
}

void AppSettings::setCacheBypassState(bool state)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("BypassCache", state);
}

bool AppSettings::defaultCacheBypassState()
{
    return true;
}

bool AppSettings::getFlushingCacheState() const
{
    m_settings->setGroup("Benchmark");
    return m_settings->readBoolEntry("FlushingCache", defaultFlushingCacheState());
}

void AppSettings::setFlushingCacheState(bool state)
{
    m_settings->setGroup("Benchmark");
    m_settings->writeEntry("FlushingCache", state);
}

bool AppSettings::defaultFlushingCacheState()
{
    return true;
}

Global::ComparisonUnit AppSettings::getComparisonUnit() const
{
    m_settings->setGroup("Interface");
    return (Global::ComparisonUnit)m_settings->readNumEntry("ComparisonUnit", (int)defaultComparisonUnit());
}

void AppSettings::setComparisonUnit(Global::ComparisonUnit comparisonUnit)
{
    m_settings->setGroup("Interface");
    m_settings->writeEntry("ComparisonUnit", (int)comparisonUnit);
}

Global::ComparisonUnit AppSettings::defaultComparisonUnit()
{
    return Global::ComparisonUnit::MBPerSec;
}

Global::Theme AppSettings::getTheme() const
{
    m_settings->setGroup("Interface");
    return (Global::Theme)m_settings->readNumEntry("Theme", (int)defaultTheme());
}

void AppSettings::setTheme(Global::Theme theme)
{
    m_settings->setGroup("Interface");
    m_settings->writeEntry("Theme", (int)theme);
}

Global::Theme AppSettings::defaultTheme()
{
    return Global::Theme::ThemeOrange;
}

bool AppSettings::getNotifyOnCompletion() const
{
    m_settings->setGroup("Interface");
    return m_settings->readBoolEntry("NotifyOnCompletion", defaultNotifyOnCompletion());
}

void AppSettings::setNotifyOnCompletion(bool notify)
{
    m_settings->setGroup("Interface");
    m_settings->writeEntry("NotifyOnCompletion", notify);
}

bool AppSettings::defaultNotifyOnCompletion()
{
    return true;
}

bool AppSettings::getUseGauges() const
{
    m_settings->setGroup("Interface");
    return m_settings->readBoolEntry("UseGauges", defaultUseGauges());
}

void AppSettings::setUseGauges(bool use)
{
    m_settings->setGroup("Interface");
    m_settings->writeEntry("UseGauges", use);
}

bool AppSettings::defaultUseGauges()
{
    return false;
}

Global::BenchmarkParams AppSettings::getBenchmarkParams(Global::BenchmarkTest test, Global::PerformanceProfile profile) const
{
    Global::BenchmarkParams defaultSet = defaultBenchmarkParams(test, profile, Global::BenchmarkPreset::Standard);
    if (profile == Global::PerformanceProfile::RealWorld) return defaultSet;

    TQString settingKey = TQString("Benchmark/Params/%1/%2").arg((int)profile).arg((int)test);
    m_settings->setGroup(settingKey);

    return {
        (Global::BenchmarkIOPattern)m_settings->readNumEntry("Pattern", (int)defaultSet.Pattern),
        m_settings->readNumEntry("BlockSize", defaultSet.BlockSize),
        m_settings->readNumEntry("Queues", defaultSet.Queues),
        m_settings->readNumEntry("Threads", defaultSet.Threads)
    };
}

void AppSettings::setBenchmarkParams(Global::BenchmarkTest test, Global::PerformanceProfile profile, Global::BenchmarkParams params)
{
    TQString settingKey = TQString("Benchmark/Params/%1/%2").arg((int)profile).arg((int)test);
    m_settings->setGroup(settingKey);

    m_settings->writeEntry("Pattern", (int)params.Pattern);
    m_settings->writeEntry("BlockSize", params.BlockSize);
    m_settings->writeEntry("Queues", params.Queues);
    m_settings->writeEntry("Threads", params.Threads);
}

Global::BenchmarkParams AppSettings::defaultBenchmarkParams(Global::BenchmarkTest test, Global::PerformanceProfile profile, Global::BenchmarkPreset preset)
{
    switch (profile)
    {
        case Global::PerformanceProfile::Default:
            switch (test)
            {
            case Global::BenchmarkTest::Test_1: return { Global::BenchmarkIOPattern::SEQ, 1024,  8,  1 };
            case Global::BenchmarkTest::Test_2:
                if (preset == Global::BenchmarkPreset::Standard) return { Global::BenchmarkIOPattern::SEQ, 1024,  1,  1 };
                else return { Global::BenchmarkIOPattern::SEQ,  128, 32,  1 };
            case Global::BenchmarkTest::Test_3:
                if (preset == Global::BenchmarkPreset::Standard) return { Global::BenchmarkIOPattern::RND,    4, 32,  1 };
                else return { Global::BenchmarkIOPattern::RND,    4, 32, 16 };
            case Global::BenchmarkTest::Test_4: return { Global::BenchmarkIOPattern::RND,    4,  1,  1 };
            }
            break;
        case Global::PerformanceProfile::Peak:
            switch (test)
            {
            case Global::BenchmarkTest::Test_1: return { Global::BenchmarkIOPattern::SEQ, 1024,  8,  1 };
            case Global::BenchmarkTest::Test_2:
                if (preset == Global::BenchmarkPreset::Standard) return { Global::BenchmarkIOPattern::RND,    4, 32,  1 };
                else return { Global::BenchmarkIOPattern::RND,    4, 32, 16 };
            }
        case Global::PerformanceProfile::RealWorld:
            switch (test)
            {
            case Global::BenchmarkTest::Test_1: return { Global::BenchmarkIOPattern::SEQ, 1024,  1,  1 };
            case Global::BenchmarkTest::Test_2: return { Global::BenchmarkIOPattern::RND,    4,  1,  1 };
            }
        case Global::PerformanceProfile::Demo:
            switch (test)
            {
            case Global::BenchmarkTest::Test_1: return { Global::BenchmarkIOPattern::SEQ, 1024,  8,  1 };
            }
    }
    return { Global::BenchmarkIOPattern::SEQ, 1024,  8,  1 };
}
