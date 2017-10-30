#ifndef TASKBASE_H
#define TASKBASE_H

#include <QThread>
#include<QMutex>
#include<QPointer>


class TaskBase : public QThread
{
    Q_OBJECT
public:
    explicit TaskBase(QObject *Receiver,const QString &DevPath);
    ~TaskBase();
    virtual void exitThread();
protected:
    void postMsg(QEvent *Event);
    bool isExit();
    QPointer<QObject> m_receiver;
    QString m_dev_path;
    QMutex mutex;
    bool m_is_exit;
};


#endif // TASKBASE_H
