#include "workerbase.h"
#include"../hdebug.h"

#include<QCoreApplication>

static HANDLE read_handle           = INVALID_HANDLE_VALUE;
static OVERLAPPED read_overlapped;

WorkerBase::WorkerBase(QObject *parent)
    : QObject(parent)
{
    
}


ThreadBase::ThreadBase(QObject *Receiver, const QString &DevPath)
{
    m_receiver = Receiver;
    m_dev_path = DevPath;
    connect(this,SIGNAL(finished()),SLOT(deleteLater()));
    read_overlapped.hEvent  = CreateEvent(NULL,TRUE,FALSE,NULL);
}

ThreadBase::~ThreadBase()
{
    ResetEvent(read_overlapped.hEvent);
    
    CloseHandle(read_overlapped.hEvent);
}

void ThreadBase::exitThread()
{
    QMutexLocker locker(&mutex);
    m_is_exit = true;
    SetEvent(read_overlapped.hEvent);
}

void ThreadBase::myPostEvent(QEvent *event)
{
    if(m_receiver.isNull())
        return;
    
    QCoreApplication::postEvent(m_receiver,event);
}

bool ThreadBase::isExit()
{
    QMutexLocker locker(&mutex);
    return m_is_exit;
}

MeaThread::MeaThread(QObject *Receiver, const QString &DevPath)
    :ThreadBase(Receiver,DevPath)
{
    
}

void MeaThread::run()
{
    MeaWorker w(this);
    HidOper hidOper;
    hidOper.setDevPath(m_dev_path);
    
    /* 发送开始测量命令 */
    if(!hidOper.send(USB_CMD_MEAS_START)){
        
        QCoreApplication::postEvent(m_receiver,new DisconEvent("发送 [USB_CMD_MEAS_START] 失败!"));
#ifdef DEBUG_TEST
        QCoreApplication::postEvent(m_receiver,new DbgEvent("发送 [USB_CMD_MEAS_START] 失败!"));
#endif
        return;
    }

    if(!hidOper.recv()){
        
        QCoreApplication::postEvent(m_receiver,new DisconEvent("接收 [USB_CMD_MEAS_START] ACK FAIL!"));
#ifdef DEBUG_TEST
        QCoreApplication::postEvent(m_receiver,new DbgEvent("接收 [USB_CMD_MEAS_START] ACK FAIL!"));
#endif
        return;
    }
    
#ifdef DEBUG_TEST
        QCoreApplication::postEvent(m_receiver,new DbgEvent("发送 [USB_CMD_MEAS_START] OK!"));
#endif
    
    read_handle = CreateFileA(m_dev_path.toLocal8Bit().data(),
                                GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_OVERLAPPED,
                                NULL);
    
    if(INVALID_HANDLE_VALUE == read_handle){
        
        QCoreApplication::postEvent(m_receiver,new DisconEvent("连接已断开!"));
#ifdef DEBUG_TEST
        QCoreApplication::postEvent(m_receiver,new DbgEvent(QString("测量任务 -> CreateFile fail,error :%1").arg(GetLastError())));
#endif
        DbgLog()<<"测量任务 ->CreateFile fail,error :"<<GetLastError();
        return;
    }
   
    while(!isExit()){
        w.doWork();
        QThread::msleep(3);
    }
    
    CloseHandle(read_handle);
    read_handle = INVALID_HANDLE_VALUE;
    
    
}


MeaWorker::MeaWorker(MeaThread * Thread, QObject *parent)
    :WorkerBase(parent)
    ,m_thread(Thread)
{
    
}

void MeaWorker::doWork()
{
    QByteArray buffer;
    bool bRet;
    buffer.resize(64 + 1);
   
    bRet = ReadFile(read_handle
             ,buffer.data()
             ,(64 + 1)
             ,NULL
             ,&read_overlapped
             );
    
    if(!bRet){
        DWORD dwErr = GetLastError();
        if(dwErr == ERROR_IO_PENDING){
            //DbgLog()<<"dwErr == ERROR_IO_PENDING";
            DWORD dwNumberOfRead;
            bool bResult = GetOverlappedResult(read_handle
                                ,&read_overlapped
                                ,&dwNumberOfRead
                                ,true
                                );
            
            if(bResult && dwNumberOfRead == 64 + 1){
                
                if(read_handle != INVALID_HANDLE_VALUE){
                    //DbgLog()<<"dwNumberOfRead :"<<dwNumberOfRead;
                    
                    /* 把第一个字节去除，留下后面64个字节 */
                    buffer.remove(0,1);
                    if(((USB_CmdDat *)buffer.data())->CmdCode == USB_ACK_MEAS_RESULT)
                        QCoreApplication::postEvent(m_thread->m_receiver,new MeaEvent(buffer));
                    //
                }
                ResetEvent(read_overlapped.hEvent);   
            }
        }
        else{
            /* 读取失败，连接已断开 */
            DisconEvent *dis = new DisconEvent(QString("测量线程读取数据失败,Error = %1").arg(dwErr));
            QCoreApplication::postEvent(m_thread->m_receiver,dis);
            return;
        }
    }
    else{
        //DbgLog()<<"doWork -> bRet = true";
        buffer.remove(0,1);
        if(((USB_CmdDat *)buffer.data())->CmdCode == USB_ACK_MEAS_RESULT)
            QCoreApplication::postEvent(m_thread->m_receiver,new MeaEvent(buffer));
    }
}


FileNameThread::FileNameThread(QObject *Receiver, const QString &DevPath)
    :ThreadBase(Receiver,DevPath)
{
    
}

void FileNameThread::run()
{
    int total;
    int cur = 1;
    FileNameWorker w(this);
    HidOper hidOper;
    /* 发送获取文件名命令 */
    if(!hidOper.send(USB_GET_FILE_NAME)){
        
        QCoreApplication::postEvent(m_receiver,new DisconEvent("发送 [USB_GET_FILE_NAME] 失败!"));
#ifdef DEBUG_TEST
        QCoreApplication::postEvent(m_receiver,new DbgEvent("发送 [USB_GET_FILE_NAME] 失败!"));
#endif
        return;
    }
#ifdef DEBUG_TEST
        QCoreApplication::postEvent(m_receiver,new DbgEvent("发送 [USB_GET_FILE_NAME] OK!"));
#endif
        
    read_handle = CreateFileA(m_dev_path.toLocal8Bit().data(),
                                GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_OVERLAPPED,
                                NULL);
    
    if(INVALID_HANDLE_VALUE == read_handle){
        
        QCoreApplication::postEvent(m_receiver,new DisconEvent("连接已断开!"));
#ifdef DEBUG_TEST
        QCoreApplication::postEvent(m_receiver,new DbgEvent(QString("获取文件名任务 -> CreateFile fail,error :%1").arg(GetLastError())));
#endif
        DbgLog()<<"CreateFile fail,error :"<<GetLastError();
        return;
    }
   
    total = w.readOne();
    
    if(-1 == total)
        goto _Cleaner;
    
    /* 这里逻辑上还有问题！ */
    while(cur < total){
        w.readOne();
        QThread::msleep(3);
        cur++;
    }
    
_Cleaner:
    QCoreApplication::postEvent(m_receiver,new FileNameFinishEvent());
    CloseHandle(read_handle);
    read_handle = INVALID_HANDLE_VALUE;
}


FileNameWorker::FileNameWorker(FileNameThread *Thread, QObject *parent)
    :WorkerBase(parent)
    ,m_thread(Thread)
{
    
}

void FileNameWorker::doWork()
{

}

int FileNameWorker::readOne()
{
    int total = -1;
    QByteArray buffer;
    USB_CmdDat *usb;
    DISKFILEINFO_Dat *nameDat;
    bool bRet;
    buffer.resize(64 + 1);
   
    bRet = ReadFile(read_handle
             ,buffer.data()
             ,(64 + 1)
             ,NULL
             ,&read_overlapped
             );
    
    if(!bRet){
        DWORD dwErr = GetLastError();
        if(dwErr == ERROR_IO_PENDING){
            //DbgLog()<<"dwErr == ERROR_IO_PENDING";
            DWORD dwNumberOfRead;
            bool bResult = GetOverlappedResult(read_handle
                                ,&read_overlapped
                                ,&dwNumberOfRead
                                ,true
                                );
            
            if(bResult && dwNumberOfRead == 64 + 1){
                
                if(read_handle != INVALID_HANDLE_VALUE){
                    DbgLog()<<"dwNumberOfRead :"<<dwNumberOfRead;
                    
                    /* 把第一个字节去除，留下后面64个字节 */
                    buffer.remove(0,1);
                    usb = (USB_CmdDat *)buffer.data();
                    if(usb->CmdCode == USB_ACK_FILE_NAME){
                        nameDat = (DISKFILEINFO_Dat *)usb->Buf;
                        total = nameDat->sumfile;
                        QCoreApplication::postEvent(m_thread->m_receiver,new FileNameEvent(buffer));
                    }
                    //
                }
                ResetEvent(read_overlapped.hEvent);   
            }
        }
        else{
            /* 读取失败，连接已断开 */
            DisconEvent *dis = new DisconEvent(QString("获取文件名任务失败,Error = %1").arg(dwErr));
            QCoreApplication::postEvent(m_thread->m_receiver,dis);
        }
    }
    else{
        buffer.remove(0,1);
        usb = (USB_CmdDat *)buffer.data();
        if(usb->CmdCode == USB_ACK_FILE_NAME){
            nameDat = (DISKFILEINFO_Dat *)usb->Buf;
            total = nameDat->sumfile;
            QCoreApplication::postEvent(m_thread->m_receiver,new FileNameEvent(buffer));
        }
    }
    return total;
}


//
//获取文件内容任务
//
FileThread::FileThread(QObject *Receiver, const QString &DevPath, const QStringList &NameList)
    :ThreadBase(Receiver,DevPath)
{
    m_name_list = NameList;
}

void FileThread::run()
{
    if(m_name_list.isEmpty())
        return;
    
    unsigned int curFile    = 0;
    unsigned int total      = m_name_list.count();
    unsigned int size       = 0;
    unsigned int curCnt,cnt,remain,sent;
    USB_CmdDat usb;
    QByteArray nameArray;
    QByteArray recv;
    QString curFileName;
    
    HidOper hidOper;
    hidOper.setDevPath(m_dev_path);
    
    while (curFile < total) {
        
        memset(&usb,0,sizeof(USB_CmdDat));
        recv.clear();
        usb.CmdCode = USB_GET_FILE;
        curFileName = m_name_list.at(curFile);
        DbgLog()<<"当前文件名："<<curFileName;
        nameArray   = curFileName.toLatin1();
        memcpy(usb.Buf,nameArray.data(),nameArray.size());
        
        /* 发送获取文件命令*/
        if(!hidOper.send(USB_GET_FILE,(char *)usb.Buf,sizeof(USB_CmdDat) - sizeof(Uint16))){
            QCoreApplication::postEvent(m_receiver,new DisconEvent("发送 [USB_GET_FILE] 失败!"));
#ifdef DEBUG_TEST
            QCoreApplication::postEvent(m_receiver,new DbgEvent("发送 [USB_GET_FILE] 失败!"));
#endif
            goto _Cleaner;
        }
#ifdef DEBUG_TEST
            QCoreApplication::postEvent(m_receiver,new DbgEvent("发送 [USB_GET_FILE] OK!"));
#endif
            
        if(!hidOper.openRead(m_dev_path)){
            QCoreApplication::postEvent(m_receiver,new DisconEvent("获取文件任务打开设备失败!"));
#ifdef DEBUG_TEST
            QCoreApplication::postEvent(m_receiver,new DbgEvent("获取文件任务打开设备失败!"));
#endif
            goto _Cleaner;
        }
        /* 每个文件返回的第一帧中包含 USB_ACK_FILE，buf中是file的大小 */
        if(!hidOper.readUsbFrame((char*)&usb)){
            QCoreApplication::postEvent(m_receiver,new DisconEvent("获取文件任务读取失败!"));
#ifdef DEBUG_TEST
            QCoreApplication::postEvent(m_receiver,new DbgEvent("获取文件任务读取失败!"));
#endif
            goto _Cleaner;
        }
        /* 验证 ack  */
        if(USB_ACK_FILE != usb.CmdCode){
            QCoreApplication::postEvent(m_receiver,new DisconEvent("获取文件任务读取错误!"));
#ifdef DEBUG_TEST
            QCoreApplication::postEvent(m_receiver,new DbgEvent("获取文件任务读取错误!"));
#endif
            goto _Cleaner;
        }
#ifdef DEBUG_TEST
            QCoreApplication::postEvent(m_receiver,new DbgEvent("开始读取文件内容..."));
#endif
        size = *(int *)usb.Buf;
        
        DbgLog()<<"File Size: " << size;
        
        /* 处理文件大小为 0 */
        if(0 == size){
            
            QCoreApplication::postEvent(m_receiver,new FileSizeZeroEvent(curFileName));
            curFile++;
            continue;
        }
        
        /* 文件读开始 */
        QCoreApplication::postEvent(m_receiver,new FileReadStartEvent(0,size));
        /* 剩余读取次数 */
        curCnt   = 0;
        sent     = 0;
        cnt      = (size / sizeof(USB_CmdDat)) + 1;
        remain   = size % sizeof(USB_CmdDat);
        
        while(curCnt < 1){
            
            SAVE_Dat *d1,*d2;
            if(!hidOper.readUsbFrame((char *)&usb)){
                QCoreApplication::postEvent(m_receiver,new DisconEvent("获取文件任务读取失败!"));
#ifdef DEBUG_TEST
                QCoreApplication::postEvent(m_receiver,new DbgEvent("获取文件任务读取失败!"));
#endif
                goto _Cleaner;
            }
            recv.append((const char *)&usb,sizeof(USB_CmdDat));
            
            d1 = (SAVE_Dat *)&usb;
            d2 = d1 + 1;
            DbgLog()<<"d1点号："<<d1->PointNum;
            DbgLog()<<"d2点号："<<d2->PointNum;
            curCnt++;
            sent = ((sizeof(USB_CmdDat) * curCnt) >= size) ? size : (sizeof(USB_CmdDat) * curCnt);
            /* 发送实时进度 */
            QCoreApplication::postEvent(m_receiver,new FileReadStartEvent(sent,size));
        }
        
        /* 从recv中删除多读出来的字节 */
        //recv.remove(size,sizeof(USB_CmdDat) - remain);
        QCoreApplication::postEvent(m_receiver,new FileReadEndEvent(size,curFileName,recv));
        curFile++;
    }
    
_Cleaner:
    hidOper.closeRead();
}

