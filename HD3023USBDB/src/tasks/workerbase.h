#ifndef WORKERBASE_H
#define WORKERBASE_H

#include <QThread>
#include<QMutex>
#include<windows.h>
#include<QPointer>
#include"../hid/hidoper.h"
#include"taskevent.h"

class MeaThread;
class FileNameThread;
class FileThread;

class WorkerBase : public QObject
{
    Q_OBJECT
public:
    explicit WorkerBase(QObject *parent = 0);
    virtual void doWork() = 0;
};


class MeaWorker : public WorkerBase
{
    Q_OBJECT
public:
    explicit MeaWorker(MeaThread * Thread,QObject *parent = 0);
    void doWork();
private:
    MeaThread * m_thread;
};

class FileNameWorker : public WorkerBase
{
    Q_OBJECT
public:
    explicit FileNameWorker(FileNameThread * Thread,QObject *parent = 0);
    void doWork();
    int readOne();
private:
    FileNameThread * m_thread;
};


class ThreadBase:public QThread
{
    Q_OBJECT
public:
    explicit ThreadBase(QObject *Receiver,const QString &DevPath);
    ~ThreadBase();
    void exitThread();
protected:
    void myPostEvent(QEvent *event);
    bool isExit();
    QPointer<QObject> m_receiver;
    QString m_dev_path;
private:
    bool m_is_exit;
    QMutex mutex;
};

//
//测量线程
//
class MeaThread :public ThreadBase
{
    friend class MeaWorker;
    Q_OBJECT
public:
    explicit MeaThread(QObject *Receiver,const QString &DevPath);
protected:
    void run();
};

//
//获取文件名线程
//
class FileNameThread :public ThreadBase
{
    friend class FileNameWorker;
    Q_OBJECT
public:
    explicit FileNameThread(QObject *Receiver,const QString &DevPath);
protected:
    void run();
};

//
//获取文件内容
//
class FileThread :public ThreadBase
{
    //friend class FileNameWorker;
    Q_OBJECT
public:
    explicit FileThread(QObject *Receiver,const QString &DevPath,const QStringList &NameList);
protected:
    void run();
private:
    QStringList m_name_list;
};

#endif // WORKERBASE_H
