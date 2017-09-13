#include "canobj.h"
#include <QDebug>

CANobj::CANobj(QObject *parent) : QObject(parent) {

}

bool CANobj::initCAN(const int portIndex){
    int s(0);
    if(0 == portIndex) {//CAN0 is used!
   /*********************step 1**********************/
        //create the CAN Socket
        int s = socket(PF_CAN,SOCK_RAW,CAN_RAW);
        if(s<0) {
            qDebug()<<"CAN initialization failed!";
            return false;
        }
    }
    /*********************step 1**********************/
            //bind socket to specific CAN port
    strcpy(m_ifr.ifr_name,"can0");
    int ret = ioctl(s,SIOCGIFINDEX,&m_ifr);
    if(ret<0) {
        qDebug()<<"ioctl failed!";
        return false;
    }
    m_addr.can_family = PF_CAN;
    m_addr.can_ifindex = m_ifr.ifr_ifindex;
    ret = bind(s,(struct sockaddr*)&m_addr,sizeof(m_addr));
    if(ret<0) {
        qDebug()<<"bind failed!";
    }
    qDebug()<<"init CAN succeed!";
}

void CANobj::slot_on_receiveFrame()
{
    int s(0);
    int nbytes = read(s,&m_frameRecv,sizeof(struct can_frame));
    if(nbytes<(int)sizeof(m_frameRecv)) {
        qDebug()<<"not enough data in one frame!";
    }
    else {
        extractFrame();
    }
}

void CANobj::slot_on_sendFrame(const int id, const int length, int *data)
{
    int s(0);
    //make m_frameSend available
    m_frameSend.can_id   =   id;
    m_frameSend.can_dlc =   length;
    for(uchar i=0; i < length; i++)
    m_frameSend.data[i] = data[i];

    //send
    int nbytes=write(s,&m_frameSend,sizeof(m_frameSend));
    if (nbytes!=sizeof(m_frameSend)) {
        qDebug()<<"Send message error senddata\n";
    }
}

void CANobj::extractFrame()
{

}

