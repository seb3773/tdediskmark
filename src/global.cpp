#include "global.h"

int Global::getOutputColumnsCount()
{
    return 80;
}

TQString Global::getBenchmarkButtonText(BenchmarkParams params, TQString paramsLine)
{
    TQString text = TQString("%1%2%3\n%4")
            .arg(params.Pattern == Global::BenchmarkIOPattern::SEQ ? "SEQ" : "RND")
            .arg(params.BlockSize >= 1024 ? params.BlockSize / 1024 : params.BlockSize)
            .arg(params.BlockSize >= 1024 ? "M" : "K");
    if (paramsLine.isEmpty())
        return text.arg(TQString("Q%1T%2").arg(params.Queues).arg(params.Threads));
    else
        return text.arg(TQString("(%1)").arg(paramsLine));
}

TQString Global::getBenchmarkButtonToolTip(BenchmarkParams params, bool extraLine)
{
    return TQObject::tr("<h2>%1 %2 %3<br/>Queues=%4<br/>Threads=%5%6</h2>")
            .arg(params.Pattern == Global::BenchmarkIOPattern::SEQ ? TQObject::tr("Sequential") : TQObject::tr("Random"))
            .arg(params.BlockSize >= 1024 ? params.BlockSize / 1024 : params.BlockSize)
            .arg(params.BlockSize >= 1024 ? TQObject::tr("MiB") : TQObject::tr("KiB"))
            .arg(params.Queues).arg(params.Threads).arg(extraLine ? TQString("<br/>(%1)") : TQString());
}

TQString Global::getToolTipTemplate()
{
    return TQObject::tr("<h1>%1 MB/s<br/>%2 GB/s<br/>%3 IOPS<br/>%4 μs</h1>");
}

TQString Global::getComparisonLabelTemplate()
{
    return TQString("<p align=\"center\">%1 [%2]</p>");
}

TQString Global::getRWSequentialRead()
{
    return "read";
}

TQString Global::getRWSequentialWrite()
{
    return "write";
}

TQString Global::getRWSequentialMix()
{
    return "rw";
}

TQString Global::getRWRandomRead()
{
    return "randread";
}

TQString Global::getRWRandomWrite()
{
    return "randwrite";
}

TQString Global::getRWRandomMix()
{
    return "randrw";
}
