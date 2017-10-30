#include "filemgr.h"
#include"hdebug.h"
#include<QDir>

//
//保存文件后缀
//
const char file_suffix[][20] = {".csv",".xlsx"};

//csv格式的列使用逗号来分开
const char csv_separator[10] = ",";
//csv如果保存为utf-8格式的话，用excel打开之后中文会出现乱码现象。
//因此改为以ansi格式来保存文件

FileMgr * FileMgr::m_file_mgr = new FileMgr();

FileMgr::~FileMgr()
{
    DbgLog()<<"~FileMgr";
}

FileMgr *FileMgr::getInstance()
{
    return m_file_mgr;
}

void FileMgr::destroyInstance()
{
    delete m_file_mgr;
}

void FileMgr::saveFile(const QString &LineNumber, QByteArray &Content)
{
    TIME_Dat td;
    unsigned int total    = Content.count() / sizeof(SAVE_Dat);
    SAVE_Dat *filePointer = (SAVE_Dat *)Content.data();
    QString subDir        = QString("%1%2").arg(m_store_dir).arg(m_serial_number);
    
    QString filePath      =
            QString("%1%2%3%4").
            arg(subDir).
            arg('/').
            arg(LineNumber).
            arg(file_suffix[0]);
    
    
    QFile file(filePath);

    /* 文件存在的话就删除 */
    if(file.exists())file.remove();
    
    create(LineNumber);
    
    /* 创建文件后，这个文件内容有可能是 0，但是还需要创建一个空文件 */
    if(Content.isEmpty())
        return;
    
    if(file.open(QFile::WriteOnly | QFile::Text | QFile::Append)){
        
        QTextStream out(&file);
        //out.setCodec("UTF-8");
        out.setCodec("ansi");
        
        for(unsigned int i = 0; i < total; i++){
            QString item;
            
            //当前点号
            item.append(QString("%1%2")
                        .arg(filePointer->PointNum)
                        .arg(csv_separator));
            //日期
            Utils ut;
            ut.longToTime(&td,filePointer->SecondCnt);
            item.append(QString("%1-%2-%3 %4:%5:%6 %7%8")
                        .arg(td.Year)
                        .arg(td.Month)
                        .arg(td.Date)
                        .arg(td.Hour)
                        .arg(td.Minute)
                        .arg(td.Second)
                        .arg(ut.numberToWeek(td.Day))
                        .arg(csv_separator));
            //测量模式
            item.append(QString("%1%2")
                        .arg(ut.numberToMode(filePointer->MeasResDat.MeasMode))
                        .arg(csv_separator));
            //测量时间
            item.append(QString("%1%2")
                        .arg(filePointer->MeasResDat.MeasTime)
                        .arg(csv_separator));
            //测量计数
            item.append(QString("%1%2")
                        .arg(filePointer->MeasResDat.ResultCnt)
                        .arg(csv_separator));
            //本底计数
            item.append(QString("%1%2")
                        .arg(filePointer->MeasResDat.BkgdCnt)
                        .arg(csv_separator));
            
            //温度
            item.append(QString("%1%2")
                        .arg(filePointer->MEMSDat.TempDat/(float)10)
                        .arg(csv_separator));
            
            //湿度
            item.append(QString("%1%2")
                        .arg(ut.rawToHumidity(filePointer->MEMSDat.RHDat))
                        .arg(csv_separator));
            //气压
            item.append(QString("%1%2")
                        .arg(ut.rawToAirPressure(filePointer->MEMSDat.PresDat))
                        .arg(csv_separator));
            //每行之后换行
            item.append('\n');
            out<<item;
            filePointer++;
        }
        DbgLog()<<"file save finished";
        file.close();
    }
}

QString FileMgr::storeDir() const
{
    return m_store_dir;
}

FileMgr::FileMgr()
{
    DbgLog()<<"FileMgr";
    init();
}

void FileMgr::setSerialNumber(const QString &SerialNumber)
{
    m_serial_number = SerialNumber;
}

void FileMgr::create(const QString &LineNumber)
{
    //构造指定文件目录，根据仪器编号在store目录下创建
    QString fullDir = QString("%1%2").arg(m_store_dir).arg(m_serial_number);
    //确保子目录存在
    QDir dir0(fullDir);
    dir0.mkpath(fullDir);
    
    QString filePath =
            QString("%1%2%3%4").
            arg(fullDir).
            arg('/').
            arg(LineNumber).
            arg(file_suffix[0]);
    
    QFile file(filePath);
    if(file.exists())
        return;
    
    
    bool ret = file.open(QIODevice::ReadWrite);
    if(!ret)return;
    
    
    //构造表头
    QString headers;
    int i;
    for(i = 0; i < TABLE_HEADERS_COUNT; i++){

        headers.append(QString("%1%2").
                       arg(table_headers[i]).
                       arg(csv_separator));
    }
    /* 加入换行符 */
    headers.append('\n');

    QTextStream out(&file);
    out.setCodec("ansi");
    out<<headers;
    file.close();
}

void FileMgr::init()
{
    m_store_dir = QDir::currentPath() + "/store/";
    
    makeDir();
    
}

void FileMgr::makeDir()
{
    QDir dir(m_store_dir);
    if(!dir.exists()){
        dir.mkpath(m_store_dir);
    }
}
