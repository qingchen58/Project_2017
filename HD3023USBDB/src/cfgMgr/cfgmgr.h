#ifndef CFGMGR_H
#define CFGMGR_H
#include<QString>
#include"user_type.h"

class CfgMgr
{
public:
    ~CfgMgr();
    static CfgMgr * getInstance();
    static void destroyInstance();
    void savePar(PAR_Dat *Par);
    void loadPar(const QString &ParPath,PAR_Dat *Par);
    QString cfgDir()const;
private:
    CfgMgr();
    void init();
    
    static CfgMgr * m_cfg_mgr;
    QString m_cfg_dir;
    QString m_cfg_path;
};

#endif // CFGMGR_H
