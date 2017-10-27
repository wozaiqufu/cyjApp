#include "surfacecommunication.h"
#include <QDebug>
#include <QThread>

SurfaceCommunication::SurfaceCommunication(QObject *parent) : QObject(parent),
    m_hostAddr("192.168.1.100")//surface ip
  ,m_port(8889)
{
    //qDebug()<<"surface construction current thread is:"<<QThread::currentThread();
}

void SurfaceCommunication::init()
{
//    m_pUdpSocket_receiver = new QUdpSocket();
//    m_pUdpSocket_sender = new QUdpSocket();
    connect(&m_UdpSocket,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));
    m_UdpSocket.bind(m_port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
}

void SurfaceCommunication::slot_doWork()
{
    QByteArray ba("0xaa 0x55");
    if(m_mainwindowData.size()==15)
    {
        for(int i = 0;i<m_mainwindowData.size();i++)
        {
            ba = ba + QByteArray::number(m_mainwindowData[i],16) + " ";
        }
        ba = ba +"0xFF";
    }
    m_UdpSocket_sender.writeDatagram(ba,ba.size(),m_hostAddr,6665);
}

void SurfaceCommunication::slot_on_mainwindowUpdate(QVector<int> vec)
{
    m_mainwindowData = vec;
}

void SurfaceCommunication::readPendingDatagrams()
{
     QByteArray datagram;
    while(m_UdpSocket.hasPendingDatagrams())
    {
        datagram.resize(m_UdpSocket.pendingDatagramSize());
        m_UdpSocket.readDatagram(datagram.data(),datagram.size());
        //qDebug()<<"Surface received datagrams are:"<<datagram;
    }
}

