#ifndef SURFACECOMMUNICATION_H
#define SURFACECOMMUNICATION_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include "cyjdata.h"

class SurfaceCommunication : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceCommunication(QObject *parent = 0);
    void init(const QString ip, const int port);
signals:
    void sig_statusTable(QString);
    void sig_informMainwindow(CYJData);
    void sig_connectState(bool);
public slots:
    void slot_doWork();//send data to surface
    void slot_on_mainwindowUpdate(CYJData cyj);
private slots:
    void slot_on_connected();
    void slot_on_readMessage();
    void slot_on_timeout();//self protect
    void slot_on_error(QAbstractSocket::SocketError socktError);
private:
    QTimer              m_timerDataInterval;
    QTcpSocket          m_tcpSocket;
    QString             m_hostIp;
    int                 m_port;
    bool                m_isOn;
    CYJData             m_cyjData_surface;
    CYJData             m_cyjData_actual;
    static const int    CONNECTMAXDELAY = -1;
    static const int    NUMBERONEFRAME = 18;//18 bytes of data
};

#endif // SURFACECOMMUNICATION_H
