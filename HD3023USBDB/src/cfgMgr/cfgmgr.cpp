#include"cfgmgr.h"
#include"hdebug.h"
#include<QDir>
#include<QDateTime>

CfgMgr * CfgMgr::m_cfg_mgr = new CfgMgr();

CfgMgr::~CfgMgr()
{
    DbgLog()<<"~CfgMgr";
}

CfgMgr *CfgMgr::getInstance()
{
    return m_cfg_mgr;
}

void CfgMgr::destroyInstance()
{
    delete m_cfg_mgr;
}

//----------------------------------------------------
//以日期为名称，".par"为后缀保存在 /config目录下
//----------------------------------------------------
void CfgMgr::savePar(PAR_Dat *Par)
{
    QString name = QDateTime::currentDateTime().toString("yy-MM-dd-HH-mm-ss") + ".par";
    QString fullPath = m_cfg_dir + name;
    QFile file(fullPath);
    
    if(file.open(QIODevice::WriteOnly)){
        
        file.write((const char *)Par,sizeof(PAR_Dat));
        file.close();
    }
}

void CfgMgr::loadPar(const QString &ParPath, PAR_Dat *Par)
{
    QFile file(ParPath);
    
    if(file.open(QIODevice::ReadOnly)){
        file.read((char *)Par,sizeof(PAR_Dat));
        file.close();
    }
}

QString CfgMgr::cfgDir() const
{
    return m_cfg_dir;
}

CfgMgr::CfgMgr()
{
    init();
}

void CfgMgr::init()
{
    m_cfg_dir  = QDir::currentPath() + "/config/";
    m_cfg_path = m_cfg_dir + "config.ini";
    QDir dir(m_cfg_dir);
    dir.mkpath(m_cfg_dir);
}
