#ifndef _NETACCESS_H_
#define _NETACCESS_H_

#include <QDir>
#include <QStringList>
#include <QByteArray>
#include <QTcpSocket>
#include <math.h>
#include <QtNetwork/QHostAddress>
#include <QMutex>

class SICK : public QObject
{
    Q_OBJECT
    public:
        enum Direction{Forward = 0,Backward};//default Forward==0
        SICK(QObject *parent = 0);
        bool connectSensor();
        //void getMutex(QMutex *mutex);
        void requestSensor(const QString& req);

        ~SICK();
private:
        void extractDISTData();
        void extractRSSIData();
        int courseAngle();
        int lateralOffset();
signals:
    void sigUpdateCourseAngle(int);
    void sigUpdateLateralOffset(int);
    void sigUpdateDIST(QVector<int>);
    void sigUpdataRSSI(QVector<int>);
    void sig_statusTable(QString);
public slots:
    void slot_on_requestContinousRead();
    void slot_on_requestContinousRead_Stop();
    void slot_on_updateDirection(int direction);
private slots:
    void slot_on_readMessage_forward();
     void slot_on_readMessage_backward();
    void slot_on_forward_error(QAbstractSocket::SocketError);
    void slot_on_backward_error(QAbstractSocket::SocketError);
private:
    QMutex m_Mutex;
//    static const int m_hostPort_SICK = 2111;
//    static const double m_PI = 3.141592653;
//    static const int m_milSecondsWait = 5000;//wait maximum seconds when establish the TCP connection
    const int m_hostPort_SICK = 2111;
    const double m_PI = 3.141592653;
    const int m_milSecondsWait = 5000;//wait maximum seconds when establish the TCP connection
    QByteArray m_dataRecieved_forward;
    QByteArray m_dataRecieved_backward;
    QTcpSocket m_tcpSocket_forward;
    QTcpSocket m_tcpSocket_backward;
    QVector<int> m_DISTdata_forward;//final data is here!
    QVector<int> m_RSSIdata_forward;//final RSSI data is here!
    QVector<int> m_DISTdata_backward;//final data is here!
    QVector<int> m_RSSIdata_backward;//final RSSI data is here!
    int m_numberDataOneSocket;
    QHostAddress m_address_SICK_forward;
    QHostAddress m_address_SICK_backward;
    QHostAddress m_address_Host;
    bool m_bIsForwardConnected;
    bool m_bIsBackwardConnected;
    Direction m_currentDirection;
//    static const double m_pi = 3.141592653;
//    static const double m_Angle_degree2Radian = 0.0174532925;
//    static const int m_angleStart = 0;
//    static const int m_anglel1l2 = 30;
//    static const int m_angleDeltaMax = 30;
//    static const int m_angleResolution = 1;
    const double m_pi = 3.141592653;
    const double m_Angle_degree2Radian = 0.0174532925;
    const int m_angleStart = 0;
    const int m_anglel1l2 = 30;
    const int m_angleDeltaMax = 30;
    const int m_angleResolution = 1;
    int m_courseAngle;

};
#endif
