#ifndef FILEMGR_H
#define FILEMGR_H
#include<QString>
#include"utils.h"


//表头信息
#define TABLE_HEADERS_COUNT         (9)

#define TABLE_COLUMN_POINTNUM       (0)
#define TABLE_COLUMN_DATE           (1)
#define TABLE_COLUMN_MEAMODE        (2)
#define TABLE_COLUMN_MEATIME        (3)
#define TABLE_COLUMN_MEACNT         (4)
#define TABLE_COLUMN_BGCNT          (5)
#define TABLE_COLUMN_TEMP           (6)
#define TABLE_COLUMN_HUMIDITY       (7)
#define TABLE_COLUMN_PRESSURE       (8)

const char table_headers[][50]={"当前点号","日期","测量模式",
                                "测量时间(min)","测量计数","本底计数",
                                "温度(℃)","湿度(%rh)","气压(kPa)"};

class FileMgr
{
public:
    ~FileMgr();
    static FileMgr * getInstance();
    static void destroyInstance();
    void setSerialNumber(const QString &SerialNumber);
    void saveFile(const QString &LineNumber, QByteArray &Content);
    QString storeDir()const;
private:
    FileMgr();
    void create(const QString &LineNumber);
    void init();
    void makeDir();
private:
    static FileMgr * m_file_mgr;
    QString m_store_dir;
    QString m_serial_number;
};

#endif // FILEMGR_H
