#ifndef GLOBAL_H
#define GLOBAL_H

#include <tqobject.h>
#include <tqstring.h>
#include <tqstringlist.h>

namespace Global
{
    enum BenchmarkTest {
        Test_1,
        Test_2,
        Test_3,
        Test_4
    };

    enum BenchmarkIOReadWrite {
        Read,
        Write,
        Mix
    };

    enum BenchmarkIOPattern {
        SEQ,
        RND
    };

    struct BenchmarkParams {
        BenchmarkIOPattern Pattern;
        int BlockSize; // KiB
        int Queues;
        int Threads;

        bool operator==(const BenchmarkParams& rhs)
        {
            return this->Pattern == rhs.Pattern && this->BlockSize == rhs.BlockSize &&
                   this->Queues  == rhs.Queues  && this->Threads   == rhs.Threads;
        }
    };

    enum PerformanceProfile {
        Default,
        Peak,
        RealWorld,
        Demo
    };

    enum BenchmarkPreset {
        Standard,
        NVMe_SSD
    };

    enum BenchmarkMode {
        ReadWriteMix,
        ReadMix,
        WriteMix
    };

    enum BenchmarkTestData {
        Random,
        Zeros
    };

    enum ComparisonUnit {
        MBPerSec,
        GBPerSec,
        IOPS,
        Latency,
    };

    enum Theme {
        ThemeOrange,
        ThemeBlue,
        ThemeNone,
        UseGauges,
    };

    struct Storage
    {
        TQString path;
        long long bytesTotal;
        long long bytesOccupied;
        TQString formatedSize;
        bool permanentInList = false;
    };

    int getOutputColumnsCount();
    TQString getBenchmarkButtonText(BenchmarkParams params, TQString paramsLine = TQString());
    TQString getBenchmarkButtonToolTip(BenchmarkParams params, bool extraField = false);
    TQString getToolTipTemplate();
    TQString getComparisonLabelTemplate();
    TQString getRWSequentialRead();
    TQString getRWSequentialWrite();
    TQString getRWSequentialMix();
    TQString getRWRandomRead();
    TQString getRWRandomWrite();
    TQString getRWRandomMix();
}

#endif // GLOBAL_H
