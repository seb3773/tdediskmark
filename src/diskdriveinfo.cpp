#include "diskdriveinfo.h"

#include <tqstring.h>
#include <tqfile.h>
#include <tqfileinfo.h>
#include <tqtextstream.h>
#include <unistd.h>
#include <stdlib.h>

DiskDriveInfo::DiskDriveInfo()
{
}

TQString DiskDriveInfo::getDeviceByVolume(const TQString &volume)
{
    TQString device = TQFileInfo(volume).absFilePath(); // resolve path
    return device.mid(device.findRev("/") + 1);
}

TQString DiskDriveInfo::getModelName(const TQString &volume)
{
    // Resolve volume like /dev/sda1 into its parent device block path
    TQString blockPath = TQString("/sys/class/block/%1/..").arg(getDeviceByVolume(volume));
    char resolved_path[4096];
    if (realpath(blockPath.local8Bit().data(), resolved_path) == NULL)
        return TQString();
        
    TQString parentBaseName = TQFileInfo(TQString::fromLocal8Bit(resolved_path)).baseName();

    TQFile sysBlock(TQString("/sys/block/%1/device/model").arg(parentBaseName));

    if (!sysBlock.open(IO_ReadOnly))
        return TQString();

    TQTextStream stream(&sysBlock);
    TQString model = stream.readLine().simplifyWhiteSpace();
    sysBlock.close();

    return model;
}

bool DiskDriveInfo::isEncrypted(const TQString &volume)
{
    TQString device = getDeviceByVolume(volume);

    if (device.find("dm") != 0)
        return false;

    TQFile sysBlock(TQString("/sys/block/%1/dm/uuid").arg(device));

    if (!sysBlock.open(IO_ReadOnly))
        return false;

    TQTextStream stream(&sysBlock);
    TQString uuid = stream.readLine().simplifyWhiteSpace();
    sysBlock.close();

    return uuid.find("CRYPT") == 0;
}

void DiskDriveInfo::setModelName(const TQString &modelName)
{
    m_modelName = modelName;
}
