#ifndef HIDOPER_H
#define HIDOPER_H

#include <QObject>
#include<windows.h>
#include"user_type.h"
#include"user_cfg.h"


//---------------------------------------------
//封装HID操作类
//---------------------------------------------
#define WRITE_WAIT_TIME         (1000)  //ms
typedef struct _HID_INFO{
    QString DevicePath;
    QString ManufacturerString;
    QString ProductString;
    USHORT Usage;
    USHORT UsagePage;
    USHORT InputReportByteLength;
    USHORT OutputReportByteLength;
    USHORT FeatureReportByteLength;
    USHORT NumberLinkCollectionNodes;
    USHORT NumberInputButtonCaps;
    USHORT NumberInputValueCaps;
    USHORT NumberInputDataIndices;
    USHORT NumberOutputButtonCaps;
    USHORT NumberOutputValueCaps;
    USHORT NumberOutputDataIndices;
    USHORT NumberFeatureButtonCaps;
    USHORT NumberFeatureValueCaps;
    USHORT NumberFeatureDataIndices;
    
    QString  VidString;
    QString  PidString;
    QString  PvnString;
    USHORT  VendorID;
    USHORT  ProductID;
    USHORT  VersionNumber;
}HID_INFO;


//----------------------------------------------------
//hid操作基类
//----------------------------------------------------
class HidOperBase : public QObject
{
    Q_OBJECT
public:
    explicit HidOperBase(QObject *parent = 0);
    ~HidOperBase();
    
    QList<HID_INFO *> getHidDevices();
    void getHidGuid(LPGUID Guid);
    void getHidDeviceInfoSet(QList<HID_INFO *> &HidList);
    void getPidString(USHORT Pid, QString &PidString);
    void getVidString(USHORT Vid, QString &VidString);
    void getPvnString(USHORT Pvn, QString &PvnString);
    
    
    bool openWrite(const QString &DevPath);
    void closeWrite();
    bool write(const char *Data,unsigned int Length);
    
    
private:
    HANDLE m_hid_handle;
    OVERLAPPED m_overlapped;
};

class HidOper : public HidOperBase
{
    Q_OBJECT
public:
    explicit HidOper(QObject *parent = 0);
    ~HidOper();
    bool isMyHidDevice(const QString &ProductStr);
    void setDevPath(const QString &DevPath);
    QString getDevPath()const;
    
    /* 封装通信协议 */
    bool send(Uint16 Cmd,char * Dat,Uint32 Length);
    bool send(Uint16 Cmd);
private:
    QString m_dev_path;
};

#endif // HIDOPER_H
