#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include "NetAccess.h"
//class NetAccess_SICK;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void sig_stopPermanentReq();
private slots:
    void slot_on_connectSICK();
    void slot_on_requestSICK_once();
    void slot_on_requestSICK_Permanent();
    void slot_on_requestSICK_PermanentStop();
    void slot_on_tcpSocketError(QAbstractSocket::SocketError);
private:
    Ui::MainWindow *ui;
    NetAccess_SICK m_sickObj;
    QTimer m_timer_SICK;
    QThread m_thread_SICK;
};

#endif // MAINWINDOW_H
