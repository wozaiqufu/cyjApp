#include "surfacecommunication.h"
#include <QDebug>

SurfaceCommunication::SurfaceCommunication(QObject *parent) : QObject(parent),
    m_hostAddr("192.168.1.100")//surface ip
{
}

void SurfaceCommunication::init()
{
//    m_pUdpSocket_receiver = new QUdpSocket();
//    m_pUdpSocket_sender = new QUdpSocket();
    connect(&m_UdpSocket_receiver,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));
    m_UdpSocket_receiver.bind(QHostAddress::LocalHost,8889);
}

void SurfaceCommunication::slot_doWork()
{
    //send data to surface
    QByteArray ba("0xaa 0x55");
    if(m_SICKdata.size()==181)
    {
        for(int i = 0;i<m_SICKdata.size();i++)
        {
            ba = ba + " " + QByteArray::number(m_SICKdata[i],16);
        }
        ba = ba + " " + "0xFF";
    }
    //qDebug()<<"SurfaceComm slot_doWork:data:"<<ba;
    m_UdpSocket_sender.writeDatagram(ba,ba.size(),m_hostAddr,6665);
}

void SurfaceCommunication::slot_on_SICKdataUpdate(QVector<int> vec)
{
    if(vec.size()!=181)
    {
        qDebug()<<"Error in surfaceComm::slot_on_SICKdataUpdate:vector size is not 181";
    }
    else
    {
        m_SICKdata = vec;
    }
}

void SurfaceCommunication::slot_on_mainwindowUpdate(QVector<int> vec)
{
    m_mainwindowData = vec;
}

void SurfaceCommunication::readPendingDatagrams()
{
     QByteArray datagram;
    while(m_UdpSocket_receiver.hasPendingDatagrams())
    {
        datagram.resize(m_UdpSocket_receiver.pendingDatagramSize());
        m_UdpSocket_receiver.readDatagram(datagram.data(),datagram.size());
    }
    qDebug()<<"Surface received datagrams are:"<<datagram;
}

