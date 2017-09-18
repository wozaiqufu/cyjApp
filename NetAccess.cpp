#include <QtCore>
#include "NetAccess.h"
#include <QDebug>


NetAccess_SICK::NetAccess_SICK(QObject* parent)
    : QObject(parent)
    ,m_address_SICK("192.168.1.50")
    ,m_address_Host("192.168.1.136")
    , m_bIsCoonected(false)
    ,m_numberDataOneSocket(0)
{
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slot_on_readMessage()));
    connect(&m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slot_on_error(QAbstractSocket::SocketError)));
}

NetAccess_SICK::~NetAccess_SICK(){

}

void NetAccess_SICK::extractData()
{
    if(m_dataRecieved.isEmpty()){
        return;
    }
    //if using the permanent mode,the first diagram is the confirmation,not the DATA
    if(!m_dataRecieved.contains("00000000")){
        return;
    }
    QString eightZeros("00000000");
    int zerosStartPos = m_dataRecieved.indexOf(eightZeros);
    zerosStartPos = zerosStartPos + 9;//index of Starting Angle
    //crop Starting Angle
    int startAngle_End = m_dataRecieved.indexOf(" ",zerosStartPos);
    QByteArray startAngle = m_dataRecieved.mid(zerosStartPos,startAngle_End-zerosStartPos);
    qDebug()<<"startAngle:"<<startAngle;
    //crop Angular step width
    int AngularStep_Begin = startAngle_End + 1;
    QByteArray angularStep = m_dataRecieved.mid(AngularStep_Begin,m_dataRecieved.indexOf(" ",AngularStep_Begin)-AngularStep_Begin);
    qDebug()<<"angular step:"<<angularStep;
    //crop Number Data
    int NumberData_Begin = m_dataRecieved.indexOf(" ",AngularStep_Begin) + 1;
    QByteArray numberData = m_dataRecieved.mid(NumberData_Begin,m_dataRecieved.indexOf(" ",NumberData_Begin)-NumberData_Begin);
    qDebug()<<"number data in Hex:"<<numberData;
    //crop Data:in order to completely crop data,we need to transform Hex to Decimal
    bool ok;
    m_numberDataOneSocket = numberData.toInt(&ok,16);
    int data_index = m_dataRecieved.indexOf(" ",NumberData_Begin) +1;
     qDebug()<<"number data in decimal :"<<m_numberDataOneSocket;
    //crop data begins
     m_data.clear();//CAUTION!m_data vector must be cleaned before new data is pushed!
     for(int i=0;i<m_numberDataOneSocket;i++)
     {
         QByteArray iData = m_dataRecieved.mid(data_index,m_dataRecieved.indexOf(" ",data_index)-data_index);
         bool ok;
         int iData_Dec = iData.toInt(&ok,16);
         if(ok){
             m_data.push_back(iData_Dec);
             //qDebug()<<"the "<<i<<" th data is: "<<iData_Dec;
         }
         data_index = m_dataRecieved.indexOf(" ",data_index) + 1;
     }
     //qDebug()<<"\n"<<"data in decimal:"<<m_data;
}

void NetAccess_SICK::slot_on_timeout()//request one diagram
{
    if(m_bIsCoonected){
        QString qstr("\x2sRN LMDscandata\x3");
        qDebug()<<"request at:"<<QTime::currentTime();
        requestSensor(qstr);
    }
}

void NetAccess_SICK::slot_on_requestContinousRead()//request permanent diagram
{
    if(m_bIsCoonected){
        QString qstr("\x2sEN LMDscandata 1\x3");
        qDebug()<<"request at:"<<QTime::currentTime();
        requestSensor(qstr);
    }
}

void NetAccess_SICK::slot_on_requestContinousRead_Stop()
{
    if(m_bIsCoonected){
        QString qstr("\x2sEN LMDscandata 0\x3");
        qDebug()<<"request at:"<<QTime::currentTime();
        requestSensor(qstr);
    }
}

void NetAccess_SICK::slot_on_updateSICK()
{

}

bool NetAccess_SICK::connectSensor(){
    qDebug()<<"connectSensor is triggered!";
    if(m_bIsCoonected){
        qDebug()<<"tcp client is already connected!";
        return false;
    }
    else{
        m_tcpSocket.abort();
        m_tcpSocket.connectToHost(m_address_SICK,m_hostPort_SICK);
        if(m_tcpSocket.waitForConnected(m_milSecondsWait)){
            qDebug()<<"socket connected!";
            m_bIsCoonected = true;
            return true;
        }
    }
}

//void NetAccess_SICK::getMutex(QMutex *mutex) {
//     m_pMutex = mutex;
//}

void NetAccess_SICK::requestSensor(const QString& req){
    if(!m_tcpSocket.isValid()){
        qDebug()<<"unable request SICK!";
    }
    else{
        m_dataRecieved.clear();
        m_tcpSocket.write(req.toLocal8Bit());
    }
}

void NetAccess_SICK::slot_on_readMessage(){
    //QDataStream in(m_tcpSocket);
    //qDebug()<<"slot_on_readMessage is triigerred!";
    m_dataRecieved = m_tcpSocket.readAll();
    //qDebug()<<"m_dataRecieved:"<<m_dataRecieved<<"at"<<QTime::currentTime();
    extractData();//vector m_data stores the final 180 data
}

void NetAccess_SICK::slot_on_error(QAbstractSocket::SocketError)
{
    qDebug()<<"slot_on_error is triggered!";
    qDebug()<<m_tcpSocket.errorString();
}
