#include <QtCore>
#include "NetAccess.h"
#include <QDebug>


NetAccess_SICK::NetAccess_SICK(QObject* parent)
    : QObject(parent)
    ,m_address_SICK_forward("192.168.1.50")
    ,m_address_SICK_backward("192.168.1.51")
    ,m_address_Host("192.168.1.136")
    , m_bIsForwardConnected(false)
    ,m_bIsBackwardConnected(false)
    ,m_numberDataOneSocket(0)
    ,m_currentDirection(Forward)
{
    connect(&m_tcpSocket_forward,SIGNAL(readyRead()),this,SLOT(slot_on_readMessage_forward()));
    connect(&m_tcpSocket_forward,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slot_on_forward_error(QAbstractSocket::SocketError)));

    connect(&m_tcpSocket_backward,SIGNAL(readyRead()),this,SLOT(slot_on_readMessage_backward()));
    connect(&m_tcpSocket_backward,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slot_on_backward_error(QAbstractSocket::SocketError)));
}

NetAccess_SICK::~NetAccess_SICK(){

}

void NetAccess_SICK::extractData()
{
    /***********************forward************************************/
    if(m_dataRecieved_forward.isEmpty()){
        return;
    }
    //if using the permanent mode,the first diagram is the confirmation,not the DATA
    if(!m_dataRecieved_forward.contains("00000000")){
        return;
    }
    QString eightZeros("00000000");
    int zerosStartPos = m_dataRecieved_forward.indexOf(eightZeros);
    zerosStartPos = zerosStartPos + 9;//index of Starting Angle
    //crop Starting Angle
    int startAngle_End = m_dataRecieved_forward.indexOf(" ",zerosStartPos);
    QByteArray startAngle = m_dataRecieved_forward.mid(zerosStartPos,startAngle_End-zerosStartPos);
    //qDebug()<<"startAngle:"<<startAngle;
    //crop Angular step width
    int AngularStep_Begin = startAngle_End + 1;
    QByteArray angularStep = m_dataRecieved_forward.mid(AngularStep_Begin,m_dataRecieved_forward.indexOf(" ",AngularStep_Begin)-AngularStep_Begin);
    //qDebug()<<"angular step:"<<angularStep;
    //crop Number Data
    int NumberData_Begin = m_dataRecieved_forward.indexOf(" ",AngularStep_Begin) + 1;
    QByteArray numberData = m_dataRecieved_forward.mid(NumberData_Begin,m_dataRecieved_forward.indexOf(" ",NumberData_Begin)-NumberData_Begin);
    //qDebug()<<"number data in Hex:"<<numberData;
    //crop Data:in order to completely crop data,we need to transform Hex to Decimal
    bool ok;
    m_numberDataOneSocket = numberData.toInt(&ok,16);
    int data_index = m_dataRecieved_forward.indexOf(" ",NumberData_Begin) +1;
     //qDebug()<<"number data in decimal :"<<m_numberDataOneSocket;
    //crop data begins
     m_data_forward.clear();//CAUTION!m_data_forward vector must be cleaned before new data is pushed!
     for(int i=0;i<m_numberDataOneSocket;i++)
     {
         QByteArray iData = m_dataRecieved_forward.mid(data_index,m_dataRecieved_forward.indexOf(" ",data_index)-data_index);
         bool ok;
         int iData_Dec = iData.toInt(&ok,16);
         if(ok){
             m_data_forward.push_back(iData_Dec);
             //qDebug()<<"the "<<i<<" th data is: "<<iData_Dec;
         }
         data_index = m_dataRecieved_forward.indexOf(" ",data_index) + 1;
     }
     //qDebug()<<"\n"<<"m_data_forward in decimal:"<<m_data_forward;
     //qDebug()<<"size of m_data_forward:"<<m_data_forward.size();

     /***********************backward************************************/
     if(m_dataRecieved_backward.isEmpty()){
         qDebug()<<"m_dataRecieved_backward is empty";
         return;
     }
     //if using the permanent mode,the first diagram is the confirmation,not the DATA
     if(!m_dataRecieved_backward.contains("00000000")){
         return;
     }
     zerosStartPos = m_dataRecieved_backward.indexOf(eightZeros);
     zerosStartPos = zerosStartPos + 9;//index of Starting Angle
     //crop Starting Angle
     startAngle_End = m_dataRecieved_backward.indexOf(" ",zerosStartPos);
     startAngle = m_dataRecieved_backward.mid(zerosStartPos,startAngle_End-zerosStartPos);
     //qDebug()<<"startAngle:"<<startAngle;
     //crop Angular step width
     AngularStep_Begin = startAngle_End + 1;
     angularStep = m_dataRecieved_backward.mid(AngularStep_Begin,m_dataRecieved_backward.indexOf(" ",AngularStep_Begin)-AngularStep_Begin);
     //qDebug()<<"angular step:"<<angularStep;
     //crop Number Data
     NumberData_Begin = m_dataRecieved_backward.indexOf(" ",AngularStep_Begin) + 1;
     numberData = m_dataRecieved_backward.mid(NumberData_Begin,m_dataRecieved_forward.indexOf(" ",NumberData_Begin)-NumberData_Begin);
     //qDebug()<<"number data in Hex:"<<numberData;
     //crop Data:in order to completely crop data,we need to transform Hex to Decimal
     m_numberDataOneSocket = numberData.toInt(&ok,16);
     data_index = m_dataRecieved_backward.indexOf(" ",NumberData_Begin) +1;
      //qDebug()<<"number data in decimal :"<<m_numberDataOneSocket;
     //crop data begins
      m_data_backward.clear();//CAUTION!m_data_forward vector must be cleaned before new data is pushed!
      for(int i=0;i<m_numberDataOneSocket;i++)
      {
          QByteArray iData = m_dataRecieved_backward.mid(data_index,m_dataRecieved_backward.indexOf(" ",data_index)-data_index);
          bool ok;
          int iData_Dec = iData.toInt(&ok,16);
          if(ok){
              m_data_backward.push_back(iData_Dec);
              //qDebug()<<"the "<<i<<" th data is: "<<iData_Dec;
          }
          data_index = m_dataRecieved_backward.indexOf(" ",data_index) + 1;
      }
      //qDebug()<<"\n"<<"m_data_backward in decimal:"<<m_data_backward;
}

//calculate course angle
int NetAccess_SICK::courseAngle()
{
    //qDebug()<<"courseAngle is triggered!";
     //qDebug()<<"m_currentDirection:"<<m_currentDirection;
    int courseAngle = 1;
    if(m_currentDirection==Forward)//forward:use forward SICK
    {
        if(m_data_forward.size()==181)
        {
            courseAngle = m_data_forward[0];
        }
        //after some calculation
       return courseAngle;
    }
    else//backward:use backward SICK
    {
        if(m_data_backward.size()!=181)
        {
            //qDebug()<<"Error in courseAngle Backward";
        }
        //after some calculation
       return courseAngle;
    }
}

//calculate lateral offset use both
int NetAccess_SICK::lateralOffset()
{
    qDebug()<<"courseAngle is triggered!";
     qDebug()<<"m_currentDirection:"<<m_currentDirection;
    int lateral = 1;
    if(m_currentDirection==Forward)//forward:use forward SICK
    {
        if(m_data_forward.size()==181)
        {
            lateral = m_data_forward[180]-m_data_forward[0];
        }
        //after some calculation
       return lateral;
    }
    else//backward:use backward SICK
    {
        if(m_data_backward.size()==181)
        {
            lateral = m_data_backward[180]-m_data_backward[0];
        }
        //after some calculation
       return lateral;
    }

}

void NetAccess_SICK::slot_on_requestContinousRead()//request permanent diagram
{
    if(m_bIsForwardConnected){
        QString qstr("\x2sEN LMDscandata 1\x3");
        qDebug()<<"request at:"<<QTime::currentTime();
        requestSensor(qstr);
    }

    if(m_bIsBackwardConnected){
        QString qstr("\x2sEN LMDscandata 1\x3");
        qDebug()<<"request at:"<<QTime::currentTime();
        requestSensor(qstr);
    }
}

void NetAccess_SICK::slot_on_requestContinousRead_Stop()
{
    if(m_bIsForwardConnected){
        QString qstr("\x2sEN LMDscandata 0\x3");
        qDebug()<<"request at:"<<QTime::currentTime();
        requestSensor(qstr);
    }
}

void NetAccess_SICK::slot_on_updateDirection(int direction)
{
    m_currentDirection = static_cast<Direction>(direction);
}

bool NetAccess_SICK::connectSensor()
{
    //qDebug()<<"connectSensor is triggered!";
    if(m_bIsForwardConnected){
        qDebug()<<" forward tcp client is already connected!";
        return false;
    }
    else{
        m_tcpSocket_forward.abort();
        m_tcpSocket_forward.connectToHost(m_address_SICK_forward,m_hostPort_SICK);
        if(m_tcpSocket_forward.waitForConnected(m_milSecondsWait))
        {
            qDebug()<<"forward socket connected!";
            m_bIsForwardConnected = true;
        }
        else
        {
            qDebug()<<"connect forward SICK failed!";
        }
    }

    if(m_bIsBackwardConnected){
        qDebug()<<" forward tcp client is already connected!";
        return false;
    }
    else{
        m_tcpSocket_backward.abort();
        m_tcpSocket_backward.connectToHost(m_address_SICK_backward,m_hostPort_SICK);
        if(m_tcpSocket_backward.waitForConnected(m_milSecondsWait))
        {
            qDebug()<<"backward socket connected!";
            m_bIsBackwardConnected = true;
            return true;
        }
        else
        {
            qDebug()<<"connect backward SICK failed!";
        }
    }
}

void NetAccess_SICK::requestSensor(const QString& req)
{
    if(!m_tcpSocket_forward.isValid())
    {
        qDebug()<<"unable request forward SICK!";
    }
    else
    {
        m_dataRecieved_forward.clear();
        m_tcpSocket_forward.write(req.toLocal8Bit());
    }

    if(!m_tcpSocket_backward.isValid())
        {
            qDebug()<<"unable request backward SICK!";
        }
        else
        {
            m_dataRecieved_backward.clear();
            m_tcpSocket_backward.write(req.toLocal8Bit());
        }
}

void NetAccess_SICK::slot_on_readMessage_forward(){
    //QDataStream in(m_tcpSocket);
    //qDebug()<<"slot_on_readMessage is triigerred!";
    m_dataRecieved_forward = m_tcpSocket_forward.readAll();
    //one data has 2 bytes
    //qDebug()<<"m_dataRecieved:"<<m_dataRecieved<<"at"<<QTime::currentTime();
    extractData();//vector m_data stores the final 180 data
    emit sigUpdateCourseAngle(courseAngle());
    emit sigUpdateLateralOffset(lateralOffset());
}

void NetAccess_SICK::slot_on_readMessage_backward()
{
    m_dataRecieved_backward = m_tcpSocket_backward.readAll();
    //one data has 2 bytes
    //qDebug()<<"m_dataRecieved:"<<m_dataRecieved<<"at"<<QTime::currentTime();
    extractData();//vector m_data stores the final 180 data
    emit sigUpdateCourseAngle(courseAngle());
     emit sigUpdateLateralOffset(lateralOffset());
}

void NetAccess_SICK::slot_on_forward_error(QAbstractSocket::SocketError)
{
     qDebug()<<"forward error:"<<m_tcpSocket_forward.errorString();
}

void NetAccess_SICK::slot_on_backward_error(QAbstractSocket::SocketError)
{
    qDebug()<<"backward error:"<<m_tcpSocket_backward.errorString();
}
