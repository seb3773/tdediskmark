#ifndef DISKDRIVEINFO_H
#define DISKDRIVEINFO_H

#include <tqstring.h>

class DiskDriveInfo
{
public:
    static DiskDriveInfo& Instance()
    {
        static DiskDriveInfo instance;
        return instance;
    }

    TQString getModelName(const TQString &blockDevice);
    void setModelName(const TQString &modelName);

    bool isEncrypted(const TQString &volume);

private:
    DiskDriveInfo();
    ~DiskDriveInfo() {}

    TQString getDeviceByVolume(const TQString &volume);
    TQString m_modelName;
};

#endif // DISKDRIVEINFO_H
