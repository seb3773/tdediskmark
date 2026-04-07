#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <tqobject.h>
#include "global.h"

class TDEConfig;

class AppSettings : public TQObject
{
    TQ_OBJECT

public:
    AppSettings(TQObject *parent = nullptr);

    void setupLocalization();

    Global::PerformanceProfile getPerformanceProfile() const;
    void setPerformanceProfile(Global::PerformanceProfile performanceProfile);
    static Global::PerformanceProfile defaultPerformanceProfile();

    bool getMixedState() const;
    void setMixedState(bool mixedState);
    static bool defaultMixedState();

    Global::BenchmarkParams getBenchmarkParams(Global::BenchmarkTest test, Global::PerformanceProfile profile = Global::PerformanceProfile::Default) const;
    void setBenchmarkParams(Global::BenchmarkTest test, Global::PerformanceProfile profile, Global::BenchmarkParams params);
    static Global::BenchmarkParams defaultBenchmarkParams(Global::BenchmarkTest test, Global::PerformanceProfile profile, Global::BenchmarkPreset preset);

    Global::BenchmarkMode getBenchmarkMode() const;
    void setBenchmarkMode(Global::BenchmarkMode benchmarkMode);
    static Global::BenchmarkMode defaultBenchmarkMode();

    Global::BenchmarkTestData getBenchmarkTestData() const;
    void setBenchmarkTestData(Global::BenchmarkTestData benchmarkTestData);
    static Global::BenchmarkTestData defaultBenchmarkTestData();

    int getLoopsCount() const;
    void setLoopsCount(int loopsCount);
    static int defaultLoopsCount();

    int getFileSize() const;
    void setFileSize(int fileSize);
    static int defaultFileSize();

    int getMeasuringTime() const;
    void setMeasuringTime(int measuringTime);
    static int defaultMeasuringTime();

    int getIntervalTime() const;
    void setIntervalTime(int intervalTime);
    static int defaultIntervalTime();

    int getRandomReadPercentage() const;
    void setRandomReadPercentage(int randomReadPercentage);
    static int defaultRandomReadPercentage();

    bool getCacheBypassState() const;
    void setCacheBypassState(bool cacheBypassState);
    static bool defaultCacheBypassState();

    bool getFlushingCacheState() const;
    void setFlushingCacheState(bool flushingCacheState);
    static bool defaultFlushingCacheState();

    Global::ComparisonUnit getComparisonUnit() const;
    void setComparisonUnit(Global::ComparisonUnit comparisonUnit);
    static Global::ComparisonUnit defaultComparisonUnit();

    Global::Theme getTheme() const;
    void setTheme(Global::Theme theme);
    static Global::Theme defaultTheme();

    bool getUseGauges() const;
    void setUseGauges(bool use);
    static bool defaultUseGauges();

    bool getNotifyOnCompletion() const;
    void setNotifyOnCompletion(bool notify);
    static bool defaultNotifyOnCompletion();

private:
    TDEConfig *m_settings;
};

#endif // APPSETTINGS_H
