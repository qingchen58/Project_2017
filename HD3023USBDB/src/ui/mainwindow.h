#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QCloseEvent>
#include<QTableWidgetItem>
#include<QMenu>
#include<QTimer>
#include"hidoper.h"
#include"readtask.h"
#include"user_cfg.h"
#include"user_type.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    bool nativeEvent(const QByteArray &eventType, void *message, long *result)Q_DECL_OVERRIDE;
    bool event(QEvent *event)Q_DECL_OVERRIDE;

private slots:


//自定义函数
    void on_btnSysPar_clicked();

    void on_btnDbgHvSet_clicked();

    void on_btnRunPar_clicked();

    void on_btnRtc_clicked();

    void on_btnCmdHalt_clicked();

    void on_btnDbgDecHv_clicked();

    void on_btnDbgIncHv_clicked();

    void on_btnDbgShortTime_clicked();

    void on_btnDbgAutoSend_clicked();

    void on_btnCmdStart_clicked();

    void on_BtnCmdStop_clicked();

    void on_btnGetFileName_clicked();

    void on_btnOpenStore_clicked();

    void on_btnSavePar_clicked();

    void on_btnLoadPar_clicked();

//自定义槽
private slots:
    void onItemDoubleClickedSlot(QTableWidgetItem*);
    void onCustomContextMenuRequestedSlot(QPoint);
    void onActionTriggedSlot();
    /* 定时器槽 */
    void onTimeoutSlot();

    void onGetSysParSlot();

    void on_btnCmdFormat_clicked();

    void on_btnDbgCoefft_clicked();

private:
    void init();
    void clean();
    void setInputLimit();
    void registerHidNotification();
    void unRegisterHidNotification();
    void findHidDevice();
    void log(const QString &Text);
    void send(ushort Cmd);
    void send(ushort Cmd,char *Dat,int Len);
    void startReadTask(const QString &DevPath);
    void abortReadTask();

    void cmdResHandler(const QByteArray &ResFrame);
    void meaHandler(const QByteArray &MeaFrame);
    void fileNameHandler(const QByteArray &NameFrame);
    void fileSizeZeroHandler();
    void fileReadEndHandler(QByteArray &Content);


    /* 显示数据到控件 */
    void displaySysPar();
    void displayRunPar();
    void displayRtc();
    void displayFileToTable(QByteArray &Content);

    /* 清除控件信息 */
    void controlClear();

    void setBtnEnable(bool Enabled);

private:
    Ui::MainWindow *ui;
    QMenu * m_download_menu;
    QAction * m_download_action;
private:
    HidOper * m_hid_oper;
    HDEVNOTIFY m_hid_notify;
    bool m_has_find;
    SYSPAR_Dat *m_sys_par;
    RUNPAR_Dat *m_run_par;
    ushort m_recent_cmd;

    /* 接收任务 */
    ReadTask * m_reader;
    QTimer m_timer;

    /* 接收文件数据成员 */
    QStringList m_file_list;
};




#endif // MAINWINDOW_H
