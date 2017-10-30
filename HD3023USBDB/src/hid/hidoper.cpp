#include "hidoper.h"
#include "hdebug.h"

extern "C" {
// Declare the C libraries used
#include "Inc/setupapi.h"   // Must link in setupapi.lib
#include "Inc/hidsdi.h"     // Must link in hid.lib
}

#define HID_WRITE_SIZE          (1 + sizeof(USB_SendBufDat))        //每次发送的字节数量，第一个字节没用到
static const char my_device_string[50] = "HD3023";

HidOperBase::HidOperBase(QObject *parent) 
    : QObject(parent)
    , m_hid_handle(INVALID_HANDLE_VALUE)
{
    
    /* OVERLAPPED 结构在使用前必须初始化为0，不然会返回 无效参数 */
    memset(&m_overlapped,0,sizeof(m_overlapped));
            
    m_overlapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
}

HidOperBase::~HidOperBase()
{
    CloseHandle(m_overlapped.hEvent);
    DbgLog()<<"~HidOperBase";
}

//---------------------------------------------
//获取所有hid设备的信息
//---------------------------------------------
QList<HID_INFO *> HidOperBase::getHidDevices()
{
    GUID guid;
    HDEVINFO hInfo;
    DWORD index;
    DWORD needed;
    SP_DEVICE_INTERFACE_DATA  devInterfaceData;
    PSP_INTERFACE_DEVICE_DETAIL_DATA detail;
    SP_DEVINFO_DATA did;
    QList<HID_INFO *> hids;
    
    
    devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    did.cbSize              = sizeof(SP_DEVINFO_DATA);
    
    
    /* 获取hid guid */
    HidD_GetHidGuid(&guid);
    
    hInfo = SetupDiGetClassDevs(&guid,NULL,NULL,DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    
    if(INVALID_HANDLE_VALUE == hInfo)
        goto _CleanUp;
    
    /* 枚举hid设备接口 */
    for(index = 0; SetupDiEnumDeviceInterfaces(hInfo,NULL,&guid,index,&devInterfaceData) ;index++){
        

        /* 获取需要的buffer大小 needed */
        SetupDiGetDeviceInterfaceDetail(hInfo, &devInterfaceData, NULL, 0, &needed, NULL);
        
        detail          = (PSP_INTERFACE_DEVICE_DETAIL_DATA)new BYTE[needed];
        
        detail->cbSize  = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
        
        did.cbSize      = sizeof(SP_DEVINFO_DATA);
        
        if (SetupDiGetDeviceInterfaceDetail(hInfo, &devInterfaceData, detail, needed, NULL, &did)){
            
            
            HID_INFO *hidItem = new HID_INFO;
            hidItem->DevicePath = QString::fromWCharArray(detail->DevicePath);
            
            qDebug()<<"DevicePath:"<<hidItem->DevicePath.toLocal8Bit().data();
            hids.append(hidItem);
        }
        
        delete[] (PBYTE)detail;
    }
    
    /* 获取hid caps 和 attributes */
    getHidDeviceInfoSet(hids);
_CleanUp:
    
    if(INVALID_HANDLE_VALUE != hInfo){
        SetupDiDestroyDeviceInfoList(hInfo);
    }
    return hids;
}

void HidOperBase::getHidGuid(LPGUID Guid)
{
    /* 获取hid guid */
    HidD_GetHidGuid(Guid);
}


inline bool HidOperBase::openWrite(const QString &DevPath)
{

    m_hid_handle = CreateFileA(DevPath.toLocal8Bit().data(),
                                GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_OVERLAPPED,
                                NULL);
    
    if(INVALID_HANDLE_VALUE == m_hid_handle){
        
        DbgLog()<<"CreateFile fail,error :"<<GetLastError();
        return false;
    }
    
    return true;
}

inline void HidOperBase::closeWrite()
{
    if(INVALID_HANDLE_VALUE != m_hid_handle){
        CloseHandle(m_hid_handle);
        m_hid_handle = INVALID_HANDLE_VALUE;
    }
}


inline bool HidOperBase::write(const char *Data, unsigned int Length)
{
    ResetEvent(m_overlapped.hEvent);
    
    bool bRet = WriteFile(m_hid_handle
                          ,Data
                          ,Length
                          ,NULL
                          ,&m_overlapped
                          );
    if(!bRet){
        DWORD dwErr = GetLastError();
        if(ERROR_IO_PENDING == dwErr){
           DWORD dwRet = WaitForSingleObject(m_overlapped.hEvent,WRITE_WAIT_TIME);
           if(WAIT_OBJECT_0 == dwRet){
               
               ResetEvent(m_overlapped.hEvent);
               return true;
           }
        }
        else
            return false;
    }
    
    return true;
}


void HidOperBase::getHidDeviceInfoSet(QList<HID_INFO *> &HidList)
{
    int i;
    int count = HidList.count();
    HID_INFO *item;
    HANDLE handle;
    PHIDP_PREPARSED_DATA PreparsedData;
    HIDP_CAPS Capabilities;
    wchar_t buffer[MAX_PATH + 1];
    
    for(i = 0; i < count; i++){
        
        item = HidList.at(i);
        handle = CreateFileA(item->DevicePath.toLatin1().data(),
                             0,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL
                             );
        
        if(INVALID_HANDLE_VALUE == handle){
            DbgLog()<<"CreateFile failed,Error : "<<GetLastError();
            continue;
        }
            

        /*
        API function: HidD_GetPreparsedData
        Returns: a pointer to a buffer containing the information about the device's capabilities.
        Requires: A handle returned by CreateFile.
        There's no need to access the buffer directly,
        but HidP_GetCaps and other API functions require a pointer to the buffer.
        */
    
        HidD_GetPreparsedData (handle, &PreparsedData);
        //DisplayLastError("HidD_GetPreparsedData: ");
    
        /*
        API function: HidP_GetCaps
        Learn the device's capabilities.
        For standard devices such as joysticks, you can find out the specific
        capabilities of the device.
        For a custom device, the software will probably know what the device is capable of,
        and the call only verifies the information.
        Requires: the pointer to the buffer returned by HidD_GetPreparsedData.
        Returns: a Capabilities structure containing the information.
        */
        
        HidP_GetCaps (PreparsedData, &Capabilities);
        
        /* 保存 caps */
        item->Usage     = Capabilities.Usage;
        item->UsagePage = Capabilities.UsagePage;
        item->InputReportByteLength     = Capabilities.InputReportByteLength;
        item->InputReportByteLength     = Capabilities.OutputReportByteLength;
        item->FeatureReportByteLength   = Capabilities.FeatureReportByteLength;
        item->NumberLinkCollectionNodes = Capabilities.NumberLinkCollectionNodes;
        item->NumberInputButtonCaps     = Capabilities.NumberInputButtonCaps;
        item->NumberInputValueCaps      = Capabilities.NumberInputValueCaps;
        item->NumberInputDataIndices    = Capabilities.NumberInputDataIndices;
        
        item->NumberOutputButtonCaps    = Capabilities.NumberOutputButtonCaps;
        item->NumberOutputValueCaps     = Capabilities.NumberOutputValueCaps;
        item->NumberOutputDataIndices   = Capabilities.NumberOutputDataIndices;
        
        item->NumberFeatureButtonCaps    = Capabilities.NumberFeatureButtonCaps;
        item->NumberFeatureValueCaps     = Capabilities.NumberFeatureValueCaps;
        item->NumberFeatureDataIndices   = Capabilities.NumberFeatureDataIndices;
        
        //No need for PreparsedData any more, so free the memory it's using.
    
        HidD_FreePreparsedData(PreparsedData);
        
        /* 获取attributes */
        HIDD_ATTRIBUTES Attributes;
        if(HidD_GetAttributes(handle,&Attributes)){
            item->VendorID      = Attributes.VendorID;
            item->ProductID     = Attributes.ProductID;
            item->VersionNumber = Attributes.VersionNumber;
            
            /* 解析pid vid 到字符串 */
            getPidString(item->ProductID,item->PidString);
            getVidString(item->VendorID,item->VidString);
            getPvnString(item->VersionNumber,item->PvnString);
        }
        
        memset(buffer,0,sizeof(buffer));
        if(HidD_GetManufacturerString(handle,buffer,sizeof(buffer))){
            item->ManufacturerString = QString::fromWCharArray(buffer);
            DbgLog()<<"ManufacturerString:"<<item->ManufacturerString;
        }
        memset(buffer,0,sizeof(buffer));
        if(HidD_GetProductString(handle,buffer,sizeof(buffer))){
            item->ProductString = QString::fromWCharArray(buffer);
            DbgLog()<<"ProductString:"<<item->ProductString;
        }
        CloseHandle(handle);
    }
}

void HidOperBase::getPidString(USHORT Pid, QString &PidString)
{
    PidString = QString::number(Pid,16);
    
    /* 前面补 0 */
    for(int i = PidString.count(); i < 4; i++){
        PidString = "0" + PidString;
    }
}

void HidOperBase::getVidString(USHORT Vid, QString &VidString)
{
    VidString = QString::number(Vid,16);
    
    for(int i = VidString.count(); i < 4; i++){
        VidString = "0" + VidString;
    }
}

void HidOperBase::getPvnString(USHORT Pvn, QString &PvnString)
{
    PvnString = QString::number(Pvn);
    
    for(int i = PvnString.count(); i < 4; i++){
        PvnString = "0" + PvnString;
    }
}


//----------------------------------------------------
//hid操作类
//----------------------------------------------------
HidOper::HidOper(QObject *parent):HidOperBase(parent)
{
    
}

HidOper::~HidOper()
{
    DbgLog()<<"~HidOper";
}

bool HidOper::isMyHidDevice(const QString &ProductStr)
{
    QString myStr = QString(my_device_string);
    
    return (myStr == ProductStr);
}

void HidOper::setDevPath(const QString &DevPath)
{
    m_dev_path = DevPath;
}

QString HidOper::getDevPath() const
{
    return m_dev_path;
}


bool HidOper::send(Uint16 Cmd, char *Dat, Uint32 Length)
{
    Q_ASSERT(Length <= (HID_WRITE_SIZE - sizeof(Uint16) - 1));   //62
    bool bRet;
    char *buffer;
    USB_CmdDat *usb;
    QByteArray array;
    
    if(!openWrite(m_dev_path))
        return false;
    
    array.resize(HID_WRITE_SIZE);
    /* buffer使用前要清零 */
    array.fill(0);
    
    buffer = array.data();
    //第一个字节是长度，cmd从第二个字节开始
    usb    = (USB_CmdDat *)(array.data() + 1);    
     
    usb->CmdCode    = Cmd;
    memcpy(usb->Buf, Dat, Length);
    *buffer         = (char)0;
    bRet            = write(array.data(), array.count());
    
    closeWrite();
    return bRet; 
    
    
}

bool HidOper::send(Uint16 Cmd)
{
    bool bRet;
    char *buffer;
    USB_CmdDat *usb;
    QByteArray array;
    
    if(!openWrite(m_dev_path))
        return false;
    
    array.resize(HID_WRITE_SIZE);
    array.fill(0);
    
    buffer = array.data();
    //第一个字节是长度，cmd从第二个字节开始
    usb    = (USB_CmdDat *)(array.data() + 1);    
    
    usb->CmdCode    = Cmd;
    *buffer         = (char)0;
    bRet            = write(array.data(), array.count());
    
    closeWrite();
    return bRet; 
}

