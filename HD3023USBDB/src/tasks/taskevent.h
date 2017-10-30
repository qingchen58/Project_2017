#ifndef TASKEVENT_H
#define TASKEVENT_H
#include<QEvent>
#include<QString>

//
//命令响应事件
//
class CmdResEvent : public QEvent
{
public:
    CmdResEvent(QByteArray &CmdResFrame);
    static Type eventType();
    QByteArray m_res_frame;
protected:
    static Type m_ev_type;
};

class DisconEvent : public QEvent
{
public:
    DisconEvent(const QString &ErrDesc);
    static Type eventType();
    QString m_desc;
protected:
    static Type m_ev_type;
};

class DbgEvent : public QEvent
{
public:
    DbgEvent(const QString &DbgInfo);
    static Type eventType();
    QString m_dbg_info;
protected:
    static Type m_ev_type;
};


class MeaEvent : public QEvent
{
public:
    MeaEvent(QByteArray &MeaDat);
    static Type eventType();
    QByteArray m_mea_dat;
protected:
    static Type m_ev_type;
};

class FileNameEvent : public QEvent
{
public:
    FileNameEvent(QByteArray &NameDat);
    static Type eventType();
    QByteArray m_name_dat;
protected:
    static Type m_ev_type;
};

class FileNameFinishEvent : public QEvent
{
public:
    FileNameFinishEvent();
    static Type eventType();
protected:
    static Type m_ev_type;
};

//
//文件大小为0
//
class FileSizeZeroEvent : public QEvent
{
public:
    FileSizeZeroEvent();
    static Type eventType();
protected:
    static Type m_ev_type;
};
//
//文件读开始
//
class FileReadStartEvent : public QEvent
{
public:
    FileReadStartEvent();
    static Type eventType();
protected:
    static Type m_ev_type;
};

//
//文件读进度
//
class FileReadProgEvent : public QEvent
{
public:
    FileReadProgEvent(unsigned int CurSize,unsigned int TotalSize);
    static Type eventType();
    unsigned int m_cur_size;
    unsigned int m_total_size;
protected:
    static Type m_ev_type;
};

class FileReadEndEvent : public QEvent
{
public:
    FileReadEndEvent(QByteArray &FileContent);
    static Type eventType();
    QByteArray m_file_content;
protected:
    static Type m_ev_type;
};

#endif // TASKEVENT_H
