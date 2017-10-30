#include "taskbase.h"
#include"hdebug.h"

#include<QCoreApplication>

TaskBase::TaskBase(QObject *Receiver, const QString &DevPath)
    :m_is_exit(false)
{
    m_receiver = Receiver;
    m_dev_path = DevPath;
    connect(this,SIGNAL(finished()),SLOT(deleteLater()));
    DbgLog()<<"TaskBase";
}

TaskBase::~TaskBase()
{
    DbgLog()<<"~TaskBase";
}

void TaskBase::exitThread()
{
    
}

void TaskBase::postMsg(QEvent *Event)
{
    if(m_receiver.isNull())
        return;
    
    QCoreApplication::postEvent(m_receiver,Event);
}

bool TaskBase::isExit()
{
    QMutexLocker locker(&mutex);
    return m_is_exit;
}
