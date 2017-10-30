#include "readtask.h"
#include"hdebug.h"
#include"taskevent.h"
#include"user_cfg.h"
#include"user_type.h"

#define CYCLE_READ_INTERVAL     (3)                         //循环读取hid设备的时间间隔,ms
#define USB_FRAME_SIZE          (sizeof(USB_RcvBufDat))     //读到的一帧数据中有效的usb数据，去除第一个字节
#define READ_SIZE               (1+sizeof(USB_RcvBufDat))   //每次读取的字节长度
#define WRITE_WAIT_TIME         (1000)                      //写超时等待时间,ms

static HANDLE read_handle  = INVALID_HANDLE_VALUE;
static OVERLAPPED read_overlapped;

static HANDLE write_handle = INVALID_HANDLE_VALUE;
static OVERLAPPED write_overlapped;

ReadTask::ReadTask(QObject *Receiver, const QString &DevPath)
  :TaskBase(Receiver,DevPath)
{
    memset(&read_overlapped,0,sizeof(read_overlapped));
    read_overlapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
}

ReadTask::~ReadTask()
{
    ResetEvent(read_overlapped.hEvent);
    CloseHandle(read_overlapped.hEvent);
    DbgLog()<<"~ReadTask";
}

void ReadTask::exitThread()
{
    QMutexLocker locker(&mutex);
    m_is_exit = true;
    SetEvent(read_overlapped.hEvent);
}

void ReadTask::run()
{
    HidReader reader(this);
    
    /* 1.打开设备 */
    read_handle = CreateFileA(m_dev_path.toLocal8Bit().data(),
                                GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_OVERLAPPED,
                                NULL);
    
    if(INVALID_HANDLE_VALUE == read_handle){
        
        postMsg(new DisconEvent("连接已断开!"));
#ifdef DEBUG_TEST
        postMsg(new DbgEvent(QString("ReadTask -> CreateFile fail,error :%1").arg(GetLastError())));
#endif
        return;
    }
    
    /* 2.循环读hid设备 */
    while(!isExit()){
        reader.doWork();
        QThread::msleep(CYCLE_READ_INTERVAL);
    }
    
    
    /* 3.关闭设备 */
//CleanUp:
    CloseHandle(read_handle);
    read_handle = INVALID_HANDLE_VALUE;
    DbgLog()<<"ReadTask finished";
}


HidReader::HidReader(ReadTask *Task)
    :m_task(Task)
    ,m_is_read_file(false)
    ,m_file_size(0)
    ,m_has_recv_count(0)
{
    m_file_content.clear();
}


void HidReader::doWork()
{
    QByteArray buffer;
    bool bRet;
    DWORD dwNumberOfRead;
    
    buffer.resize(READ_SIZE);
        
    bRet = ReadFile(read_handle
             ,buffer.data()
             ,READ_SIZE
             ,NULL
             ,&read_overlapped
             );
    
    
    if(!bRet){
        DWORD dwErr = GetLastError();
        //DbgLog()<<"ReadFile -> GetLastError: "<<dwErr;
        /* 检查是否异步读取 */
        if(ERROR_IO_PENDING == dwErr){
           
            bRet = GetOverlappedResult(
                        read_handle
                        ,&read_overlapped
                        ,&dwNumberOfRead
                        ,true
                        );
            
            if(bRet && dwNumberOfRead == READ_SIZE){
                
                if(read_handle != INVALID_HANDLE_VALUE){
                    /* 解析读到的数据帧 */
                    parseDataFrame(buffer);
                }
                ResetEvent(read_overlapped.hEvent);
                return;
            }
        }
        else{
            /* 出现读取错误 */
            m_task->postMsg(new DisconEvent("读取数据错误，请重新连接设备!"));
            return;
        }
    }
    else{
        /* 解析读到的数据帧 */
        parseDataFrame(buffer);
        return;
    }
}

void HidReader::parseDataFrame(QByteArray &DataFrame)
{
    USB_CmdDat *usb;
    //DISKFILEINFO_Dat *d;
    /* 去除第一个无用的字节 */
    DataFrame.remove(0,1);
    
    /* 检查是否是读取文件内容，这个比较特殊，不满足USB_CmdDat结构 */
    if(!isReadFile()){
         usb = (USB_CmdDat *)DataFrame.data();
         
         switch (usb->CmdCode) {
         case USB_ACK_FILE_NAME:
             m_task->postMsg(new FileNameEvent(DataFrame));
             break;
         case USB_ACK_MEAS_RESULT:
             m_task->postMsg(new MeaEvent(DataFrame));
             break;
         case USB_ACK_OK:
         case USB_ACK_FAIL:
             m_task->postMsg(new CmdResEvent(DataFrame));
             break;
         case USB_ACK_FILE:
             /* 下一帧准备接收数据 */
//             readyToRecvFile(*(unsigned int *)usb->Buf);
             break;
         default:
             break;
         }
    }
    else{
        /* DataFrame里面是读取到的文件内容 */
        recvFile(DataFrame);
    }
}

inline bool HidReader::isReadFile() const
{
    return m_is_read_file;
}

inline void HidReader::resetFileDataMember()
{
    m_is_read_file = false;
}

void HidReader::readyToRecvFile(unsigned int FileSize)
{
    DbgLog()<<"FileSize: "<<FileSize;
    m_is_read_file   = true;
    m_file_size      = FileSize;
    m_has_recv_count = 0;
    m_file_content.clear();
    
    /* 发送文件读开始 */
    m_task->postMsg(new FileReadStartEvent());
    
    /* 处理文件size 为 0，下位机在发送ACK_FILE之后如果size为，就不进行下一帧的发送了 */
    if(0 >= FileSize){
        DbgLog()<<"FileSize = 0";
        m_task->postMsg(new FileSizeZeroEvent());
        resetFileDataMember();
    }
}

//----------------------------------------------------
//实现接收文件内容，根据size做出判断,size 应该是 SAVE_Dat(32)的整数倍
//----------------------------------------------------
void HidReader::recvFile(QByteArray &DataFrame)
{
    m_file_content.append(DataFrame);
    
    m_has_recv_count += sizeof(USB_RcvBufDat);
    
    if(m_has_recv_count >= m_file_size){
        /* 说明这是最后一次接收文件内容 */
       
        if(m_has_recv_count > m_file_size){
            /* 从接收buffer中移除多余的字节 */
            m_file_content.remove(m_file_size,m_has_recv_count - m_file_size);
            
            m_has_recv_count = m_file_size;   
        }
        /* 发送 读完成 */
        DbgLog()<<"读完成";
        m_task->postMsg(new FileReadEndEvent(m_file_content));
        resetFileDataMember();
    }
    /* 发送实时进度 */
    m_task->postMsg(new FileReadProgEvent(m_has_recv_count,m_file_size));
    
    
}




HidWriter::HidWriter(const QString &DevPath)
    :m_dev_path(DevPath)
{
    memset(&write_overlapped,0,sizeof(write_overlapped));
    write_overlapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    
    write_handle = CreateFileA(m_dev_path.toLocal8Bit().data(),
                                GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_OVERLAPPED,
                                NULL);
    if(INVALID_HANDLE_VALUE == write_handle)
        DbgLog()<<"CreateFile fail,error :"<<GetLastError();
}

HidWriter::~HidWriter()
{
    ResetEvent(write_overlapped.hEvent);
    CloseHandle(write_overlapped.hEvent);
    CloseHandle(write_handle);
    write_handle = INVALID_HANDLE_VALUE;
}

bool HidWriter::write(const char *Data, unsigned int Length)
{
    
    bool bRet = WriteFile(write_handle
                          ,Data
                          ,Length
                          ,NULL
                          ,&write_overlapped
                          );
    if(!bRet){
        DWORD dwErr = GetLastError();
        if(ERROR_IO_PENDING == dwErr){
           DWORD dwRet = WaitForSingleObject(write_overlapped.hEvent,WRITE_WAIT_TIME);
           if(WAIT_OBJECT_0 == dwRet){
               
               ResetEvent(write_overlapped.hEvent);
               return true;
           }
        }
        else
            return false;
    }
    
    return true;
}
