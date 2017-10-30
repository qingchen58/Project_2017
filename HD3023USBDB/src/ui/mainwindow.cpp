#include"mainwindow.h"
#include"ui_mainwindow.h"
#include"hdebug.h"
#include<dbt.h>
#include<QDateTime>
#include"hdebug.h"
#include"utils.h"
#include"cfgmgr.h"
#include"taskevent.h"
#include<QDesktopServices>
#include<QFileDialog>
#include<QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_has_find(false)
{
    ui->setupUi(this);

    /* 初始化成员 */
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    /* 清理工作 */
    clean();
    DbgLog()<<"closeEvent";
}
//----------------------------------------------------
//设备插拔事件
//----------------------------------------------------
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    //Q_UNUSED(eventType);
    Q_UNUSED(result);
    if(eventType == "windows_generic_MSG"){
        MSG *msg = static_cast<MSG *>(message);

        if(msg->message == WM_DEVICECHANGE){
            DEV_BROADCAST_HDR * lpdb = (DEV_BROADCAST_HDR*)(msg->lParam);
            if(msg->wParam == DBT_DEVICEREMOVECOMPLETE){
                if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE){
                    if(m_has_find){
                        findHidDevice();
                    }
                    DbgLog("HID 设备拔出！");
                }
            }
            else if(msg->wParam == DBT_DEVICEARRIVAL){
                if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE){
                    if(!m_has_find){
                        findHidDevice();
                    }
                    DbgLog("HID 设备插入！");
                }
            }
        }
    }
    return false;
}
//----------------------------------------------------
//接收多线程事件
//----------------------------------------------------
bool MainWindow::event(QEvent *event)
{
    if(event->type() == DisconEvent::eventType()){

            DisconEvent *e = (DisconEvent *)event;
            log(e->m_desc);
            return true;
        }
        else if( event->type() == CmdResEvent::eventType()){

            CmdResEvent *e   = (CmdResEvent *)event;
            cmdResHandler(e->m_res_frame);
            return true;
        }
    #ifdef DEBUG_TEST
        else if( event->type() == DbgEvent::eventType()){

            DbgEvent *e   = (DbgEvent *)event;
            log(e->m_dbg_info);
            return true;
        }
    #endif
        else if( event->type() == MeaEvent::eventType()){

            MeaEvent *e   = (MeaEvent *)event;
            meaHandler(e->m_mea_dat);
            return true;
        }
        else
//        if( event->type() == FileNameEvent::eventType()){

//            FileNameEvent *e   = (FileNameEvent *)event;
//            fileNameHandler(e->m_name_dat);
//            return true;
//        }
//        else
            if( event->type() == FileReadStartEvent::eventType()){

            /* 文件开始读 */
            //ui->fileProcess->setValue(0);
            setBtnEnable(false);
            return true;
        }
        else if( event->type() == FileSizeZeroEvent::eventType()){
            /* 文件size为0 */
            fileSizeZeroHandler();
            return true;
        }
        else if( event->type() == FileReadProgEvent::eventType()){

            FileReadProgEvent *e   = (FileReadProgEvent *)event;

            ui->fileProcess->setRange(0,e->m_total_size);
            ui->fileProcess->setValue(e->m_cur_size);
            return true;
        }
        else if( event->type() == FileReadEndEvent::eventType()){

            FileReadEndEvent *e   = (FileReadEndEvent *)event;

            fileReadEndHandler(e->m_file_content);
            return true;
        }
        return QWidget::event(event);
}

void MainWindow::init() //系统初始化
{
    m_hid_oper = new HidOper(this);

    /* 初始化变量 */
    m_sys_par        = new SYSPAR_Dat; //系统设置参数初始化
    m_run_par        = new RUNPAR_Dat; //系统运行参数初始化

    m_reader         = NULL;

    memset(m_sys_par,0,sizeof(SYSPAR_Dat));
    memset(m_run_par,0,sizeof(RUNPAR_Dat));

    /* 程序标题 */
    this->setWindowTitle(tr("HD3023-USB调试版1.0"));
    /* 设置样式表 */
    this->setStyleSheet("QPushButton{border: 2px solid #8f8f91;border-radius: 6px;"
                        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                        "stop: 0 #f6f7fa, stop: 1 #dadbde);min-width: 80px;}"
                        "QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                                "stop: 0 #dadbde, stop: 1 #f6f7fa);}"
                                                "QPushButton:flat {border: none;}"
                                                "QPushButton:default {border-color: navy;}"
                        );
    /*  控件初始化 */
    ui->fileProcess->setValue(0);
    //文件信息部分布局
    QStringList header1 = QStringList()<<tr("文件名");
    ui->listFileName->setColumnCount(1);
    //运行多选文件名批量下载
    ui->listFileName->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listFileName->setHorizontalHeaderLabels(header1);

    QStringList header2;
//    for(int i = 0;i < TABLE_HEADERS_COUNT; i++)
//        header2.append(tr("%1").arg(table_headers[i]));

//    ui->tableFile->setColumnCount(header2.count());
//    ui->tableFile->setHorizontalHeaderLabels(header2);

//    for(int k = 0;k < TABLE_HEADERS_COUNT; k++)
//    ui->tableFile->setColumnWidth(k,100);

//    ui->tableFile->setColumnWidth(TABLE_COLUMN_DATE,200);
//    ui->tableFile->setSelectionMode(QAbstractItemView::ExtendedSelection);

    /* 创建批量下载菜单 */
    m_download_menu   = new QMenu(this);
    m_download_action = new QAction(tr("批量下载"),this);
    m_download_menu->addAction(m_download_action);
    connect(m_download_menu,SIGNAL(triggered(QAction*)),this,SLOT(onActionTriggedSlot()));

    /* 文件下载操作的 信号 和 槽 */
    //文件名列表双击
    connect(ui->listFileName,
    SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
    SLOT(onItemDoubleClickedSlot(QTableWidgetItem*)));
    connect(ui->listFileName,
    SIGNAL(customContextMenuRequested(QPoint)),
    SLOT(onCustomContextMenuRequestedSlot(QPoint)));

    /* 连接定时器 */
    connect(&m_timer,SIGNAL(timeout()),SLOT(onTimeoutSlot()));
    m_timer.setInterval(1000);
    /* 日志字体颜色 */
    QPalette pe;
    pe.setColor(QPalette::Text,Qt::red);
    ui->etLog->setPalette(pe);


    /* 限制输入框的输入范围 */
    setInputLimit();
    /* 注册事件通知设备 */
    registerHidNotification();

    /* 查找hid设备 */
    findHidDevice();
}
void MainWindow::clean()
{
    /* 检查测量线程是否运行 */
    abortReadTask();

    unRegisterHidNotification();

    delete m_sys_par;
    delete m_run_par;
}

//----------------------------------------------------
//限制输入框，防止输入非法数值
//----------------------------------------------------
void MainWindow::setInputLimit()
{
    ui->etDevHv->setValidator(new QIntValidator(1,HV_MAX,this));
    ui->etDbgHv->setValidator(new QIntValidator(1,HV_MAX,this));
    ui->etMeasTime->setValidator(new QIntValidator (1,MEASTIME_MAX,this));
    ui->etMeasGroup->setValidator(new QIntValidator(1,MEASTIME_MAX,this));
    ui->etMeasNo->setValidator(new QIntValidator(1,MEASTIME_MAX,this));
}


void MainWindow::registerHidNotification()
{
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    GUID guid;
    m_hid_oper->getHidGuid(&guid);
    NotificationFilter.dbcc_classguid = guid;
    m_hid_notify = RegisterDeviceNotification((HANDLE)this->winId(),
                                              &NotificationFilter,
                                              DEVICE_NOTIFY_WINDOW_HANDLE
                                              );
}

void MainWindow::unRegisterHidNotification()
{
    UnregisterDeviceNotification(m_hid_notify);
}

void MainWindow::findHidDevice()
{
    bool bFind = false;
    QList<HID_INFO *> hids;
    HID_INFO * myHid;
    hids = m_hid_oper->getHidDevices();

    /* 比较是否有我的hid */
    foreach (HID_INFO *hid, hids) {
        if(m_hid_oper->isMyHidDevice(hid->ProductString)){
            bFind       = true;
            myHid       = hid;
            break;
        }
    }

    m_has_find = bFind;

    if(m_has_find){
        m_hid_oper->setDevPath(myHid->DevicePath);
        ui->lbDevState->setText("设备状态：已发现");
        ui->lbDevName->setText(QString("设备名称：%1").arg(myHid->ProductString));

        /* 开启接收线任务 */
        startReadTask(myHid->DevicePath);
        /* 获取设备系统参数和运行参数 */
        send(USB_CMD_ACCESS_RIGHT);
        send(USB_GET_SYS_PAR);
    }
    else{
        /* 传入一个错误的地址 */
        m_hid_oper->setDevPath(" ");
        ui->lbDevState->setText("设备状态：未发现");
        ui->lbDevName->setText(QString("设备名称：无"));

        /* 终止接收任务 */
        abortReadTask();

        /* 停止更新rtc定时器 */
        if(m_timer.isActive())
            m_timer.stop();

        controlClear();
        setEnabled(true);
    }
}

void MainWindow::log(const QString &Text)
{
    if(ui->etLog->toPlainText().size() > 0xffff)
        ui->etLog->clear();

    ui->etLog->appendPlainText(Text);
}

void MainWindow::send(ushort Cmd)
{
    bool m = m_hid_oper->send(Cmd);
    m_recent_cmd = Cmd;
    qDebug()<<"命令发送状态"<<m;
}
void MainWindow::send(ushort Cmd, char *Dat, int Len)
{
    m_hid_oper->send(Cmd,Dat,Len);
    m_recent_cmd = Cmd;
}

void MainWindow::startReadTask(const QString &DevPath)
{
    m_reader = new ReadTask(this,DevPath);
    m_reader->start();
}

void MainWindow::abortReadTask()
{
    if(m_reader!= NULL && m_reader->isRunning()){
        m_reader->exitThread();
        m_reader->wait();
        m_reader = NULL;
    }
}

void MainWindow::cmdResHandler(const QByteArray &ResFrame)
{
    USB_CmdDat *usb = (USB_CmdDat *)ResFrame.data();
    SYSPAR_Dat *sysPar;
    RUNPAR_Dat *runPar;
//    FileMgr *fm     = FileMgr::getInstance();
    Utils ut;
    if(USB_ACK_FAIL == usb->CmdCode){
        log(QString("发送[%1]失败!").arg(ut.cmdToCmdString(m_recent_cmd)));
        return;
    }
    log(QString("发送[%1] OK!").arg(ut.cmdToCmdString(m_recent_cmd)));
    switch (m_recent_cmd) {
    case USB_GET_SYS_PAR:

        sysPar = (SYSPAR_Dat *)usb->Buf;
        memcpy(m_sys_par,sysPar,sizeof(SYSPAR_Dat));
        displaySysPar();
        displayRtc();
        /* 设置仪器序列号到文件管理器 */
//        fm->setSerialNumber(QString("%1").arg(m_sys_par->SN));
        /* 开启定时器更新rtc */
        if(!m_timer.isActive())
            m_timer.start();
        send(USB_GET_RUN_PAR);
        break;
    case USB_GET_RUN_PAR:
        runPar = (RUNPAR_Dat *)usb->Buf;
        memcpy(m_run_par,runPar,sizeof(RUNPAR_Dat));
        displayRunPar();
        break;
    case USB_SET_SYS_PAR:
        ui->etDbgHv->setText(QString("%1").arg(m_sys_par->HVSDat));
        ui->etDevCofftDbg->setText(QString("%1").arg(m_sys_par->Cofft));
        ui->etDevDeadTimeDbg->setText(QString("%1").arg(m_sys_par->DTCofft));

        break;

    case USB_SET_RUN_PAR:

        break;

    case USB_SET_COFFT:
        /* 更新系统参数中的系数 */
        ui->etDevCofft->setText(QString("%1").arg(m_sys_par->Cofft));
        ui->etDevDeadTime->setText(QString("%1").arg(m_sys_par->DTCofft));

        break;
    case USB_SET_HV:
    {
        /* 更新系统参数高压 */
        ui->etDevHv->setText(QString("%1").arg(m_sys_par->HVSDat));
        Uint16 meavalue = *(Uint16 *)usb->Buf;
        ui->lbHvDigiValue->setText(QString("高压测量数字值：%1").arg(meavalue << 2));
//        ui->lbHvAnaValue->setText(QString::number(meavalue >> 2));
    }
        break;
//    case USB_GET_HV:
//    {
//        Uint16 meavalue = *(Uint16 *)usb->Buf;
//        ui->lbHvAnaValue->setText(QString("高压测量模拟值:%1").arg(meavalue >> 2));
//        ui->lbHvAnaValue->setText(QString::number(meavalue >> 2));
//    }
        break;
    case USB_SET_RTC:
        break;
    case USB_CMD_HALT:
        break;
    case USB_CMD_ACCESS_RIGHT:
        qDebug()<<"获得USB主动权";
        break;

    case USB_CMD_DEBUG_RESULT:

        break;
    case USB_CMD_DEBUG_TIME:

        break;
    case USB_CMD_MEAS_START:
        setBtnEnable(false);
        break;
    case USB_CMD_MEAS_STOP:
        setBtnEnable(true);
        break;
    default:
        break;
    }
}

void MainWindow::meaHandler(const QByteArray &MeaFrame)
{
    USB_CmdDat *usb;
    SAVE_Dat *d;
    usb = (USB_CmdDat *)MeaFrame.data();
    d   = (SAVE_Dat *)usb->Buf;
    TIME_Dat td;
    ui->lbMeaCurPoint->setText(QString("当前点号：%1").arg(d->PointNum));
    Utils ut;
    ut.longToTime(&td,d->SecondCnt);

    /* 当前时间 */
    ui->lbMeaCurTime->setText(QString("当前时间：%1-%2-%3 %4:%5:%6 %7")
                              .arg(td.Year)
                              .arg(td.Month)
                              .arg(td.Date)
                              .arg(td.Hour)
                              .arg(td.Minute)
                              .arg(td.Second)
                              .arg(ut.numberToWeek(td.Day)));

//    ui->lbMeaCurMode->setText(QString("测量模式：%1").arg(ut.numberToMode(d->MeasResDat.MeasMode)));
//    ui->lbMeaTime->setText(QString("测量时间：%1").arg(d->MeasResDat.MeasTime));
//    ui->lbMeaCurCnt->setText(QString("测量值(10-8Gy/h)：%1").arg(d->MeasResDat.ResultCnt));
//    ui->lbMeaBgCnt->setText(QString("本底计数值：%1").arg(d->MeasResDat.BkgdCnt));

}

//void MainWindow::fileNameHandler(const QByteArray &NameFrame)
//{
//    USB_CmdDat *usb             = (USB_CmdDat *)NameFrame.data();
//    DISKFILEINFO_Dat *nameDat   = (DISKFILEINFO_Dat *)usb->Buf;

//    QString fileName((const char*)nameDat->fname);

//    Uint16 index    = nameDat->findex;
//    Uint16 total    = nameDat->sumfile;

//    //DbgLog()<<"index: "<<index;
//    //显示到listwidget，假设前面已有N行 count = N
//    int count       = ui->listFileName->rowCount();

//    ui->listFileName->setRowCount(count+1);
//    QTableWidgetItem *item = new QTableWidgetItem(fileName);
//    item->setTextAlignment(Qt::AlignCenter);
//    ui->listFileName->setItem(count,0,item);

//    if(1 == index && index < total)
//        setBtnEnable(false);
//    else if(index == total)
//        setBtnEnable(true);
//}

void MainWindow::fileSizeZeroHandler()
{
    if(m_file_list.isEmpty())
        return;

    /* 保存到文件 */
    QString curFile = m_file_list.at(0);

    log(QString("%1读取完成!").arg(curFile));
    /* 去除文件名中的 .B */
    curFile = curFile.remove(".B");
//    FileMgr *fm = FileMgr::getInstance();
    QByteArray noData;
//    fm->saveFile(curFile,noData);

    /* 从列表中移除 */
    m_file_list.removeFirst();

    setBtnEnable(true);
    /* 如果list中还有文件，启动下一次 文件读 */
    if(!m_file_list.isEmpty()){
        QString next      = m_file_list.at(0);
        QByteArray buffer = next.toLatin1();
        send(USB_GET_FILE,buffer.data(),buffer.length());
    }
}

//----------------------------------------------------
//文件读结束
//----------------------------------------------------
void MainWindow::fileReadEndHandler(QByteArray &Content)
{
    if(m_file_list.isEmpty())
        return;
    /* 显示到table */
    displayFileToTable(Content);

    QString curFile = m_file_list.at(0);

    log(QString("%1读取完成!").arg(curFile));
    /* 保存到文件 */
    /* 去除文件名中的 .B */
    curFile = curFile.remove(".B");
//    FileMgr *fm = FileMgr::getInstance();
//    fm->saveFile(curFile,Content);

    /* 从列表中移除 */
    m_file_list.removeFirst();

    setBtnEnable(true);

    /* 进行next read */
    if(!m_file_list.isEmpty()){
        QString next      = m_file_list.at(0);
        QByteArray buffer = next.toLatin1();
        send(USB_GET_FILE,buffer.data(),buffer.length());
    }
}

void MainWindow::displaySysPar()
{
    /* 系统参数 */
    ui->etDevHv->setText(QString("%1").arg(m_sys_par->HVSDat));
    ui->etDevBG->setText(QString("%1").arg(m_sys_par->Bkgd));
    ui->etDevCofft->setText(QString("%1").arg(m_sys_par->Cofft));
    ui->etDevDeadTime->setText(QString("%1").arg(m_sys_par->DTCofft));
    ui->etDevNo->setText(QString("%1").arg(m_sys_par->SN));

    /* 调试参数 */
    ui->etDbgHv->setText(QString("%1").arg(m_sys_par->HVSDat));
    ui->etDevCofftDbg->setText(QString("%1").arg(m_sys_par->Cofft));
    ui->etDevDeadTimeDbg->setText(QString("%1").arg(m_sys_par->DTCofft));

}
//*********************************************
//**运行参数显示函数
//*********************************************
void MainWindow::displayRunPar()
{
    ui->etDevAutoOffTime->setText(QString("%1").arg(m_run_par->AtuoOffTime));
    ui->etDevAutoPrint->setText(QString("%1").arg(m_run_par->AutoPrint));
    ui->etMeasGroup->setText(QString("%1").arg(m_run_par->MeasGroup));
    ui->etMeasNo->setText(QString("%1").arg(m_run_par->MeasNo));
    ui->etMeasTime->setText(QString("%1").arg(m_run_par->MeasTime));
}
//*********************************************
//**时间RTC显示函数
//*********************************************
void MainWindow::displayRtc()
{

    int year = QDateTime::currentDateTime().date().year();
    int mon  = QDateTime::currentDateTime().date().month();
    int date = QDateTime::currentDateTime().date().day();
    int day  = QDateTime::currentDateTime().date().dayOfWeek();

    int hour = QDateTime::currentDateTime().time().hour();
    int min  = QDateTime::currentDateTime().time().minute();
    int sec  = QDateTime::currentDateTime().time().second();

    ui->etRtcY->setText(QString("%1").arg(year));
    ui->etRtcM->setText(QString("%1").arg(mon));
    ui->etRtcDay->setText(QString("%1").arg(day));
    ui->etRtcDate->setText(QString("%1").arg(date));
    ui->etRtcH->setText(QString("%1").arg(hour));
    ui->etRtcMin->setText(QString("%1").arg(min));
    ui->etRtcSec->setText(QString("%1").arg(sec));

}

void MainWindow::displayFileToTable(QByteArray &Content)
{
    /* Content中是文件的全部内容 */
    Utils ut;
    SAVE_Dat *d = (SAVE_Dat *)Content.data();
    int size    = Content.size();
    int count   = size/sizeof(SAVE_Dat);
    ui->tableFile->setRowCount(count);


    for(int i = 0; i < count; i++,d++){

        /* 1. 点号 */
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(d->PointNum));
//        ui->tableFile->setItem(i,TABLE_COLUMN_POINTNUM,item);

        /* 2. 日期 */
        TIME_Dat td;
        ut.longToTime(&td,d->SecondCnt);
        QString date = QString("%1-%2-%3 %4:%5:%6 %7")
                .arg(td.Year)
                .arg(td.Month)
                .arg(td.Date)
                .arg(td.Hour)
                .arg(td.Minute)
                .arg(td.Second)
                .arg(ut.numberToWeek(td.Day));
         item         = new QTableWidgetItem(date);
//        ui->tableFile->setItem(i,TABLE_COLUMN_DATE,item);

        /* 3.测量模式 */
//        QString mode = ut.numberToMode(d->MeasResDat.MeasMode);
//        item         = new QTableWidgetItem(mode);
//        ui->tableFile->setItem(i,TABLE_COLUMN_MEAMODE,item);

        /* 4.测量时间 */
//        item         = new QTableWidgetItem(QString::number(d->MeasResDat.MeasTime));
//        ui->tableFile->setItem(i,TABLE_COLUMN_MEATIME,item);

        /* 5.测量计数 */
//        item         = new QTableWidgetItem(QString::number(d->MeasResDat.ResultCnt));
//        ui->tableFile->setItem(i,TABLE_COLUMN_MEACNT,item);

        /* 6.本底计数 */
//        item         = new QTableWidgetItem(QString::number(d->MeasResDat.BkgdCnt));
//        ui->tableFile->setItem(i,TABLE_COLUMN_BGCNT,item);

        /* 7.温度 */
//        item         = new QTableWidgetItem(QString::number(d->MEMSDat.TempDat/10,'f',1));
//        ui->tableFile->setItem(i,TABLE_COLUMN_TEMP,item);

        /* 8.湿度 */
//        item         = new QTableWidgetItem(QString::number(ut.rawToHumidity(d->MEMSDat.RHDat)));
//        ui->tableFile->setItem(i,TABLE_COLUMN_HUMIDITY,item);
        /* 9.气压 */
//        item         = new QTableWidgetItem(QString::number(ut.rawToAirPressure(d->MEMSDat.PresDat)));
//        ui->tableFile->setItem(i,TABLE_COLUMN_PRESSURE,item);
    }
}

void MainWindow::controlClear()
{
    /* 系统参数 */
    ui->etDevHv->setText("");
    ui->etDevNo->setText("");
    ui->etDevBG->setText("");
    ui->etDevCofft->setText("");
    ui->etDevDeadTime->setText("");

    /* 调试参数 */
    ui->etDbgHv->setText("");
    ui->etDevCofftDbg->setText("");
    ui->etDevDeadTimeDbg->setText("");

    //运行参数
    ui->etDevAutoOffTime->setText("");
    ui->etDevAutoPrint->setText("");
    ui->etMeasGroup->setText("");
    ui->etMeasNo->setText("");
    ui->etMeasTime->setText("");

    //rtc
    ui->etRtcY->setText("");
    ui->etRtcM->setText("");
    ui->etRtcDay->setText("");
    ui->etRtcDate->setText("");
    ui->etRtcH->setText("");
    ui->etRtcMin->setText("");
    ui->etRtcSec->setText("");
}

void MainWindow::setBtnEnable(bool Enabled)
{
    ui->btnSysPar->setEnabled(Enabled);
    ui->btnSavePar->setEnabled(Enabled);
    ui->btnLoadPar->setEnabled(Enabled);
    ui->btnRtc->setEnabled(Enabled);
    ui->btnRunPar->setEnabled(Enabled);

    ui->btnDbgDecHv->setEnabled(Enabled);
    ui->btnDbgIncHv->setEnabled(Enabled);
    ui->btnDbgHvSet->setEnabled(Enabled);

    ui->btnDbgCoefft->setEnabled(Enabled);

    ui->btnCmdHalt->setEnabled(Enabled);

    ui->btnCmdFormat->setEnabled(Enabled);

    ui->btnGetFileName->setEnabled(Enabled);

    ui->listFileName->setEnabled(Enabled);

    ui->btnCmdStart->setEnabled(Enabled);
}


//----------------------------------------------------
//处理设置按钮事件
//----------------------------------------------------
void MainWindow::on_btnSysPar_clicked()
{
    m_sys_par->HVSDat       = (Uint16)ui->etDevHv->text().toUShort();
    m_sys_par->SN           = ui->etDevNo->text().toUInt();
    m_sys_par->Cofft        = ui->etDevCofft->text().toFloat();
    m_sys_par->Bkgd         = ui->etDevBG->text().toFloat();
    m_sys_par->DTCofft      = ui->etDevDeadTime->text().toFloat();

    /* 发送系统参数 */
    send(USB_SET_SYS_PAR,(char *)m_sys_par,sizeof(SYSPAR_Dat));
}

void MainWindow::on_btnDbgHvSet_clicked()
{
    Uint16 hv = ui->etDbgHv->text().toUShort();

    if(hv < 1 || hv > HV_MAX){
        QMessageBox::warning(this,"警告","高压设置值超出范围!");
        return;
    }
    /* 这个hv 是用户设置的模拟量高压 跟 数字量高压 近似相等 */

    m_sys_par->HVSDat   = hv;


    /* 发送系统参数 */
    send(USB_SET_HV,(char *)m_sys_par,sizeof(SYSPAR_Dat));

    /* 开启获取高压测量值定时 */
    //QTimer::singleShot(2000,this,SLOT(onGetSysParSlot()));
}

void MainWindow::on_btnRunPar_clicked()
{
    /* 更新运行参数 */
    m_run_par->AtuoOffTime = (Uint8)(ui->etDevAutoOffTime->text().toUShort());
    m_run_par->AutoPrint   = (Uint8)(ui->etDevAutoPrint->text().toUShort());
    m_run_par->MeasGroup   = (Uint16)(ui->etMeasGroup->text().toUShort());
    m_run_par->MeasNo      = (Uint16)(ui->etMeasNo->text().toUShort());
    m_run_par->MeasTime    = (Uint16)(ui->etMeasTime->text().toUShort());

    /* 发送运行参数 */
    send(USB_SET_RUN_PAR,(char *)m_run_par,sizeof(RUNPAR_Dat));

}

void MainWindow::on_btnRtc_clicked()
{
    TIME_Dat td;
    Utils ut;
    memset(&td,0,sizeof(TIME_Dat));

    td.Year   = ut.cutLongYearToShort(ui->etRtcY->text().toUShort());
    td.Month  = (Uint8)ui->etRtcM->text().toUShort();
    td.Date   = (Uint8)ui->etRtcDate->text().toUShort();
    td.Day    = (Uint8)ui->etRtcDay->text().toUShort();
    td.Hour   = (Uint8)ui->etRtcH->text().toUShort();
    td.Minute = (Uint8)ui->etRtcMin->text().toUShort();
    td.Second = (Uint8)ui->etRtcSec->text().toUShort();

    DbgLog()<<"td.Year: "<<td.Year;
    send(USB_SET_RTC,(char *)&td,sizeof(TIME_Dat));
}
//执行关机操作
void MainWindow::on_btnCmdHalt_clicked()
{
    send(USB_CMD_HALT);
}

void MainWindow::on_btnDbgDecHv_clicked()
{
    Uint16 hv   = ui->etDbgHv->text().toUShort();

    hv          = (hv >= 2) ? (hv-1):hv;

    ui->etDbgHv->setText(QString("%1").arg(hv));

    on_btnDbgHvSet_clicked();
}

void MainWindow::on_btnDbgIncHv_clicked()
{
    Uint16 hv   = ui->etDbgHv->text().toUShort() + 1;

    ui->etDbgHv->setText(QString("%1").arg(hv));

    on_btnDbgHvSet_clicked();
}

void MainWindow::on_btnDbgShortTime_clicked()
{
    send(USB_CMD_DEBUG_TIME);
}

void MainWindow::on_btnDbgAutoSend_clicked()
{
    send(USB_CMD_DEBUG_RESULT);
}

void MainWindow::on_btnDbgCoefft_clicked()
{
    m_sys_par->Cofft   = ui->etDevCofftDbg->text().toFloat();
    m_sys_par->DTCofft = ui->etDevDeadTimeDbg->text().toFloat();

    /*发送系统参数*/
    send(USB_SET_COFFT,(char *)m_sys_par,sizeof(SYSPAR_Dat));
}
//-----------------------------------------
//开始测量
//-----------------------------------------
void MainWindow::on_btnCmdStart_clicked()
{
    send(USB_CMD_MEAS_START);
}

void MainWindow::on_BtnCmdStop_clicked()
{
    /* 发送停止命令 */
    send(USB_CMD_MEAS_STOP);
}
//----------------------------------------------------
//文件操作
//----------------------------------------------------
void MainWindow::on_btnGetFileName_clicked()
{
    //清空当前
    ui->listFileName->clearContents();
    ui->listFileName->setRowCount(0);

    /* 发送获取文件名命令 */
    send(USB_GET_FILE_NAME);
}

void MainWindow::on_btnOpenStore_clicked()
{
//    FileMgr *fm = FileMgr::getInstance();
    /* 打开仓库 */
//    QDesktopServices::openUrl(QUrl::fromLocalFile(fm->storeDir()));
}

void MainWindow::on_btnSavePar_clicked()
{
    PAR_Dat parDat;
    memset(&parDat,0,sizeof(PAR_Dat));
    memcpy(&parDat.RunPar, m_run_par,sizeof(RUNPAR_Dat));
    memcpy(&parDat.SysPar,m_sys_par,sizeof(SYSPAR_Dat));

    CfgMgr *cm = CfgMgr::getInstance();
    cm->savePar(&parDat);
}

void MainWindow::on_btnLoadPar_clicked()
{
    PAR_Dat parDat;
    CfgMgr *cm = CfgMgr::getInstance();
    QFileDialog fd;
    fd.setDirectory(cm->cfgDir());
    fd.setNameFilter("*.par");
    int res = fd.exec();
    if(QDialog::Accepted == res){
        QStringList files = fd.selectedFiles();
        QString file      = files.at(0);

        cm->loadPar(file,&parDat);

        memcpy(m_sys_par,&parDat.SysPar,sizeof(SYSPAR_Dat));
        memcpy(m_run_par,&parDat.RunPar,sizeof(RUNPAR_Dat));

        displaySysPar();
        displayRunPar();
    }
}

//
//获取HV测量值,高压测量值等于 获取到的值 >> 2
//
void MainWindow::onGetSysParSlot()
{
    send(USB_GET_SYS_PAR);
}

void MainWindow::onItemDoubleClickedSlot(QTableWidgetItem *Item)
{
    QByteArray buffer = Item->text().toLatin1();

    m_file_list.clear();
    m_file_list.append(Item->text());

    send(USB_GET_FILE,buffer.data(),buffer.length());

}

void MainWindow::onCustomContextMenuRequestedSlot(QPoint Point)
{
    QList<QTableWidgetItem *> list = ui->listFileName->selectedItems();
    if(list.count() <= 0)return;
    m_download_menu->popup(ui->listFileName->viewport()->mapToGlobal(Point));
}


void MainWindow::onActionTriggedSlot()
{
    m_file_list.clear();

    /* 启动批量下载 */
    QList<QTableWidgetItem *> list = ui->listFileName->selectedItems();
    foreach (QTableWidgetItem * item, list) {
        m_file_list.append(item->text());
    }

    QString first     = m_file_list.at(0);
    QByteArray buffer = first.toLatin1();
    send(USB_GET_FILE,buffer.data(),buffer.length());
}

void MainWindow::onTimeoutSlot()
{
    /* 更新RTC */
    displayRtc();
}

//
//格式化仪器存储
//
void MainWindow::on_btnCmdFormat_clicked()
{
    send(USB_CMD_FMKFS);
}


