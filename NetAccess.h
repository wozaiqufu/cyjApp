#ifndef _NETACCESS_H_
#define _NETACCESS_H_

#include <QDir>
#include <QStringList>
#include <QByteArray>
#include <QTcpSocket>
#include <math.h>
#include <QtNetwork/QHostAddress>
#include <QMutex>

class NetAccess_SICK : public QObject
{
    Q_OBJECT
    public:
        NetAccess_SICK(QObject *parent = 0);
        bool connectSensor();
        void getMutex(QMutex *mutex);
        void requestSensor(const QString& req);
        ~NetAccess_SICK();
private:
        void extractData();

signals:
    void sigUpdateData(QString);
public slots:
    void slot_on_timeout();
    void slot_on_requestContinousRead();
    void slot_on_requestContinousRead_Stop();
private slots:
    void slot_on_readMessage();
    void slot_on_error(QAbstractSocket::SocketError);
private:
     QMutex* m_pMutex;
    static const int m_hostPort_SICK = 2111;
    static const double m_PI = 3.141592653;
    static const int m_milSecondsWait = 5000;//wait maximum seconds when establish the TCP connection
    QByteArray m_dataRecieved;
    QTcpSocket m_tcpSocket;
    QVector<int> m_data;//final data is here!
    int m_numberDataOneSocket;
    QHostAddress m_address_SICK;
    QHostAddress m_address_Host;
    bool m_bIsCoonected;
};
#endif
