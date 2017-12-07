#include "surfacecommunication.h"
#include <QDebug>
#include <QThread>

SurfaceCommunication::SurfaceCommunication(QObject *parent) : QObject(parent),
    m_isOn(false)
{
    //qDebug()<<"surface construction current thread is:"<<QThread::currentThread();
}

void SurfaceCommunication::init(const QString ip, const int port)
{
    m_hostIp = ip;
    m_port = port;
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(slot_on_readMessage()));
    connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_on_error(QAbstractSocket::SocketError)));
    m_tcpSocket.abort();
    m_tcpSocket.connectToHost(m_hostIp, m_port);
    if (m_tcpSocket.waitForConnected(CONNECTMAXDELAY))
    {
        emit sig_statusTable("Surface Tcp Init Succeeded");
        m_isOn = true;
    }
}

void SurfaceCommunication::slot_doWork()
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    for(int i = 0;i<NUMBERONEFRAME;i++)
    {
        if(i==0) out<<quint8(0xAA);
        if(i==1) out<<quint8(0x55);
        if(i==2)
        {
            int aa = 0;
            aa += m_cyjData_actual.forward;
            aa += m_cyjData_actual.backward*2;
            aa += m_cyjData_actual.neutral*4;
            aa += m_cyjData_actual.stop*8;
            aa += m_cyjData_actual.scram*16;
            aa += m_cyjData_actual.light*32;
            aa += m_cyjData_actual.horn*64;
            aa += m_cyjData_actual.zero*128;
            out<<quint8(aa);
        }
        if(i==3)
        {
            int aa = 0;
            aa += m_cyjData_actual.automanual;
            aa += m_cyjData_actual.remoteLocal*2;
            aa += m_cyjData_actual.start*4;
            aa += m_cyjData_actual.flameout*8;
            aa += m_cyjData_actual.middle*16;
            aa += m_cyjData_actual.warn1*32;
            aa += m_cyjData_actual.warn2*64;
            aa += m_cyjData_actual.warn3*128;
            out<<quint8(aa);
        }
        if(i==4) out<<quint8(m_cyjData_actual.rise);
        if(i==5) out<<quint8(m_cyjData_actual.fall);
        if(i==6) out<<quint8(m_cyjData_actual.turn);
        if(i==7) out<<quint8(m_cyjData_actual.back);
        if(i==8) out<<quint8(m_cyjData_actual.left);
        if(i==9) out<<quint8(m_cyjData_actual.right);
        if(i==10) out<<quint8(m_cyjData_actual.acc);
        if(i==11) out<<quint8(m_cyjData_actual.deacc);
        if(i==12) out<<quint8(m_cyjData_actual.speed);
        if(i==13) out<<quint8(m_cyjData_actual.engine);
        if(i==14) out<<quint8(m_cyjData_actual.spliceAngle);
        if(i==15) out<<quint8(m_cyjData_actual.oil);
        if(i==16) out<<quint8(m_cyjData_actual.temperature);
        if(i==17) out<<quint8(0xFF);
    }
    qDebug()<<"=========================actual data are:";
    qDebug()<<"neutral:"<<m_cyjData_actual.neutral;
    qDebug()<<"stop:"<<m_cyjData_actual.stop;
    qDebug()<<"scram:"<<m_cyjData_actual.scram;
    qDebug()<<"light:"<<m_cyjData_actual.light;
    qDebug()<<"horn:"<<m_cyjData_actual.horn;
    qDebug()<<"start:"<<m_cyjData_actual.start;
    qDebug()<<"flameout:"<<m_cyjData_actual.flameout;
    qDebug()<<"middle:"<<m_cyjData_actual.middle;
    qDebug()<<"rise:"<<m_cyjData_actual.rise;
    qDebug()<<"fall:"<<m_cyjData_actual.fall;
    qDebug()<<"turn:"<<m_cyjData_actual.turn;
    qDebug()<<"back:"<<m_cyjData_actual.back;
    qDebug()<<"left:"<<m_cyjData_actual.left;
    qDebug()<<"right:"<<m_cyjData_actual.right;
    qDebug()<<"acc:"<<m_cyjData_actual.acc;
    qDebug()<<"deacc:"<<m_cyjData_actual.deacc;
    qDebug()<<"speed:"<<m_cyjData_actual.speed;
    qDebug()<<"engine:"<<m_cyjData_actual.engine;
    qDebug()<<"splice:"<<m_cyjData_actual.spliceAngle;
    m_tcpSocket.write(block);
//    QDataStream in(&m_tcpSocket);
//    in<<quint8(0xAA)<<quint8(0x55)<<quint8(0x61)<<quint8(0x06)
//     <<quint8(0x01)<<quint8(0x01)<<quint8(0x01)<<quint8(0x01)<<quint8(0x10)<<quint8(0x00)
//    <<quint8(0x10)<<quint8(0x00)<<quint8(0x64)<<quint8(0x32)<<quint8(0x16)<<quint8(0x64)
//    <<quint8(0x10)<<quint8(0xFF);
    //QByteArray ba("AA 55 ");
//    char aa[9];
//    aa[0] = 0xAA;
//    aa[1] = 0x55;
//    aa[2] = 0x61;
//    aa[3] = 0x06;
//    aa[4] = 0x01;
//    aa[5] = 0x01;
//    aa[6] = 0x01;
//    aa[7] = 0x01;
//    aa[8] = 0x10;
//    aa[9] = 0x00;
//    aa[10] = 0x10;
//    aa[11] = 0x00;
//    aa[12] = 0x64;
//    aa[13] = 0x32;
//    aa[14] = 0x16;
//    aa[15] = 0x64;
//    aa[16] = 0x10;
//    aa[17] = 0xFF;
}

void SurfaceCommunication::slot_on_mainwindowUpdate(CYJData cyj)
{
    m_cyjData_actual.startdata1 = cyj.startdata1;
    m_cyjData_actual.startdata2 = cyj.startdata2;
    m_cyjData_actual.forward = cyj.forward;
    m_cyjData_actual.backward = cyj.backward;
    m_cyjData_actual.neutral = cyj.neutral;
    m_cyjData_actual.stop = cyj.stop;
    m_cyjData_actual.scram = cyj.scram;
    m_cyjData_actual.light = cyj.light;
    m_cyjData_actual.horn = cyj.horn;
    m_cyjData_actual.zero = cyj.zero;

    m_cyjData_actual.automanual = cyj.automanual;
    m_cyjData_actual.remoteLocal = cyj.remoteLocal;
    m_cyjData_actual.start = cyj.start;
    m_cyjData_actual.flameout = cyj.flameout;
    m_cyjData_actual.middle = cyj.middle;
    m_cyjData_actual.warn1 = cyj.warn1;
    m_cyjData_actual.warn2 = cyj.warn2;
    m_cyjData_actual.warn3 = cyj.warn3;

    m_cyjData_actual.rise = cyj.rise;
    m_cyjData_actual.fall = cyj.fall;
    m_cyjData_actual.turn = cyj.turn;
    m_cyjData_actual.back = cyj.back;
    m_cyjData_actual.left = cyj.left;
    m_cyjData_actual.right = cyj.right;
    m_cyjData_actual.acc = cyj.acc;
    m_cyjData_actual.deacc = cyj.deacc;
    m_cyjData_actual.speed = cyj.speed;
    m_cyjData_actual.engine = cyj.engine;
    m_cyjData_actual.spliceAngle = cyj.spliceAngle;
    m_cyjData_actual.temperature = cyj.temperature;
    m_cyjData_actual.oil = 0;
    m_cyjData_actual.enddata = 0xFF;
}

void SurfaceCommunication::slot_on_readMessage()
{
    m_tcpSocket.read((char *)&m_cyjData_surface,sizeof(m_cyjData_surface));
    if(m_cyjData_surface.startdata1==0xAA&&m_cyjData_surface.startdata2==0x55&&m_cyjData_surface.enddata==0xFF)
        emit sig_informMainwindow(m_cyjData_surface);
//    qDebug()<<"===========================================>";
//    qDebug()<<"data from surface forward:"<<m_cyjData_surface.forward;
//    qDebug()<<"data from surface backward:"<<m_cyjData_surface.backward;
//    qDebug()<<"data from surface neutral:"<<m_cyjData_surface.neutral;
//    qDebug()<<"data from surface stop:"<<m_cyjData_surface.stop;
//    qDebug()<<"data from surface scram:"<<m_cyjData_surface.scram;
//    qDebug()<<"data from surface light:"<<m_cyjData_surface.light;
//    qDebug()<<"data from surface horn:"<<m_cyjData_surface.horn;
//    qDebug()<<"data from surface zero:"<<m_cyjData_surface.zero;
//    qDebug()<<"data from surface start:"<<m_cyjData_surface.start;
//    qDebug()<<"data from surface flameout:"<<m_cyjData_surface.flameout;
//    qDebug()<<"data from surface middle:"<<m_cyjData_surface.middle;
//    qDebug()<<"data from surface remoteLocal:"<<m_cyjData_surface.remoteLocal;
//    qDebug()<<"data from surface automanual:"<<m_cyjData_surface.automanual;
//    qDebug()<<"data from surface rise:"<<m_cyjData_surface.rise;
//    qDebug()<<"data from surface fall:"<<m_cyjData_surface.fall;
//    qDebug()<<"data from surface turn:"<<m_cyjData_surface.turn;
//    qDebug()<<"data from surface back:"<<m_cyjData_surface.back;
//    qDebug()<<"data from surface left:"<<m_cyjData_surface.left;
//    qDebug()<<"data from surface right:"<<m_cyjData_surface.right;
//    qDebug()<<"data from surface acc:"<<m_cyjData_surface.acc;
//    qDebug()<<"data from surface deacc:"<<m_cyjData_surface.deacc;
}


