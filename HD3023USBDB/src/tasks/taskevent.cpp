#include "taskevent.h"

QEvent::Type DisconEvent::m_ev_type = QEvent::None;

DisconEvent::DisconEvent(const QString &ErrDesc)
    :QEvent(eventType())
    ,m_desc(ErrDesc)
{
    
}

QEvent::Type DisconEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}


QEvent::Type MeaEvent::m_ev_type = QEvent::None;
MeaEvent::MeaEvent(QByteArray &MeaDat)
    :QEvent(eventType())
    ,m_mea_dat(MeaDat)
{
    
}

QEvent::Type MeaEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}

QEvent::Type FileNameEvent::m_ev_type = QEvent::None;
FileNameEvent::FileNameEvent(QByteArray &NameDat)
    :QEvent(eventType())
    ,m_name_dat(NameDat)
{
    
}

QEvent::Type FileNameEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}


QEvent::Type DbgEvent::m_ev_type = QEvent::None;
DbgEvent::DbgEvent(const QString &DbgInfo)
    :QEvent(eventType())
    ,m_dbg_info(DbgInfo)
{
    
}

QEvent::Type DbgEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}


QEvent::Type FileNameFinishEvent::m_ev_type = QEvent::None;
FileNameFinishEvent::FileNameFinishEvent()
    :QEvent(eventType())
{
    
}

QEvent::Type FileNameFinishEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}

QEvent::Type FileSizeZeroEvent::m_ev_type = QEvent::None;
FileSizeZeroEvent::FileSizeZeroEvent()
    :QEvent(eventType())
{
    
}

QEvent::Type FileSizeZeroEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}

QEvent::Type FileReadProgEvent::m_ev_type = QEvent::None;
FileReadProgEvent::FileReadProgEvent(unsigned int CurSize, unsigned int TotalSize)
        :QEvent(eventType())
        ,m_cur_size(CurSize)
        ,m_total_size(TotalSize)
{
    
}

QEvent::Type FileReadProgEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}


QEvent::Type FileReadEndEvent::m_ev_type = QEvent::None;
FileReadEndEvent::FileReadEndEvent(QByteArray &FileContent)
    :QEvent(eventType())
    ,m_file_content(FileContent)
{
    
}

QEvent::Type FileReadEndEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}

CmdResEvent::CmdResEvent(QByteArray &CmdResFrame)
    :QEvent(eventType())
    ,m_res_frame(CmdResFrame)
{
    
}
QEvent::Type CmdResEvent::m_ev_type = QEvent::None;
QEvent::Type CmdResEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}


QEvent::Type FileReadStartEvent::m_ev_type = QEvent::None;
FileReadStartEvent::FileReadStartEvent():QEvent(eventType())
{
    
}

QEvent::Type FileReadStartEvent::eventType()
{
    if(m_ev_type == QEvent::None){
        m_ev_type = (QEvent::Type)registerEventType();
    }
    return m_ev_type;
}
