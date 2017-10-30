#ifndef READTASK_H
#define READTASK_H

#include "taskbase.h"
#include<windows.h>

class HidReader;
class ReadTask : public TaskBase
{
    friend class HidReader;
    Q_OBJECT
public:
    explicit ReadTask(QObject *Receiver,const QString &DevPath);
    ~ReadTask();
    void exitThread();
protected:
    void run();
};


class HidReader
{
public:
    HidReader(ReadTask *Task);
    void doWork();
private:
    void parseDataFrame(QByteArray &DataFrame);
    bool isReadFile()const;
    void resetFileDataMember();
    void readyToRecvFile(unsigned int FileSize);
    void recvFile(QByteArray &DataFrame);
private:
    ReadTask *m_task;
    bool m_is_read_file;            //是否是读取文件
    unsigned int m_file_size;       //文件大小
    unsigned int m_has_recv_count;  //已经读取个数
    QByteArray m_file_content;      //存放文件内容
};

class HidWriter
{
public:
    HidWriter(const QString &DevPath);
    ~HidWriter();
    bool write(const char *Data,unsigned int Length);
private:
    QString m_dev_path;
};

#endif // READTASK_H
