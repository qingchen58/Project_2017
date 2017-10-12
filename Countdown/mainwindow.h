#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPointer>
#include <QThread>

class SecondFirer:public QObject
{
    Q_OBJECT

public:
    SecondFirer(int seconds):m_nSeconds(seconds) {}

signals:
    void secondleft(int sec, unsigned int id);

public slots:
    void onTimeout();

private:
    int m_nSeconds;
};


class CountThread:public QThread
{
public:
    CountThread(QWidget *receiver);
    ~CountThread();

protected:
    void run();

private:
    QPointer<QWidget> m_receiver;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onSencondLeft(int sec, unsigned int id);

private:
    QLabel * m_label;
};

#endif // MAINWINDOW_H
