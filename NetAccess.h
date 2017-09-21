#ifndef _NETACCESS_H_
#define _NETACCESS_H_

#include <QDir>
#include <QStringList>
#include <QByteArray>
#include <QTcpSocket>
#include <math.h>
#include <QtNetwork/QHostAddress>
#include <QMutex>

enum DrivingDir{Forward,Backward};

class NetAccess_SICK : public QObject
{
    Q_OBJECT
    public:
        NetAccess_SICK(QObject *parent = 0);
        bool connectSensor();
        //void getMutex(QMutex *mutex);
        void requestSensor(const QString& req);

        ~NetAccess_SICK();
private:
        void extractData();
        int courseAngle();
        int lateralOffset();
signals:
    void sigUpdateCourseAngle(int);
    void sigUpdateLateralOffset(int);
    void sigUpdateData(QVector<int>);
public slots:
    void slot_on_requestContinousRead();
    void slot_on_requestContinousRead_Stop();
private slots:
    void slot_on_readMessage_forward();
     void slot_on_readMessage_backward();
    void slot_on_forward_error(QAbstractSocket::SocketError);
    void slot_on_backward_error(QAbstractSocket::SocketError);
private:
    QMutex m_Mutex;
    static const int m_hostPort_SICK = 2111;
    static const double m_PI = 3.141592653;
    static const int m_milSecondsWait = 5000;//wait maximum seconds when establish the TCP connection
    QByteArray m_dataRecieved_forward;
    QByteArray m_dataRecieved_backward;
    QTcpSocket m_tcpSocket_forward;
    QTcpSocket m_tcpSocket_backward;
    QVector<int> m_data_forward;//final data is here!
    QVector<int> m_data_backward;//final data is here!
    int m_numberDataOneSocket;
    QHostAddress m_address_SICK_forward;
    QHostAddress m_address_SICK_backward;
    QHostAddress m_address_Host;
    bool m_bIsForwardConnected;
    bool m_bIsBackwardConnected;
};
#endif
