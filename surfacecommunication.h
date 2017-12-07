#ifndef SURFACECOMMUNICATION_H
#define SURFACECOMMUNICATION_H

#include <QObject>
#include <QTcpSocket>
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
public slots:
    void slot_doWork();//send data to surface
    void slot_on_mainwindowUpdate(CYJData cyj);
private slots:
    void slot_on_readMessage();
private:
    QTcpSocket          m_tcpSocket;
    QString             m_hostIp;
    int                 m_port;
    bool                m_isOn;
    CYJData             m_cyjData_surface;
    CYJData             m_cyjData_actual;
    QVector<int>        m_mainwindowData;//to be sent to surface
    static const int    CONNECTMAXDELAY = 2000;
    static const int    NUMBERONEFRAME = 18;//18 bytes of data
};

#endif // SURFACECOMMUNICATION_H
