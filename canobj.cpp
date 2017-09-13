#include "canobj.h"
#include <QDebug>

CANobj::CANobj(QObject *parent) : QObject(parent) {

}

bool CANobj::initCAN(const int portIndex){
    int s;
    int ret;
    if(0 == portIndex) {//CAN0 is used!
   /*********************step 1**********************/
        //create the CAN Socket
        srand(time(NULL));
        s = socket(PF_CAN,SOCK_RAW,CAN_RAW);
        if(s<0) {
            qDebug()<<"CAN initialization failed!";
            return false;
        }

    /*********************step 1**********************/
            //bind socket to specific CAN port
     //strcpy(ifr.ifr_name, "can0");
    strcpy(m_ifr.ifr_name,"can0");
    qDebug()<<"m_ifr.ifr_name:"<<m_ifr.ifr_name;
    ret = ioctl(s,SIOCGIFINDEX,&m_ifr);
    if(ret<0) {
        qDebug()<<"ioctl failed!";
        return false;
    }
    m_addr.can_family = PF_CAN;
    m_addr.can_ifindex = m_ifr.ifr_ifindex;
    ret = bind(s,(struct sockaddr*)&m_addr,sizeof(m_addr));
    if(ret<0) {
        qDebug()<<"bind failed!";
        return false;
    }
    qDebug()<<"init CAN succeed!";
    //setup CAN filters
    m_filter_AngularSensor.can_id = 0x1F1|CAN_EFF_FLAG;
    m_filter_AngularSensor.can_mask = 0xFFF;
    ret = setsockopt(s,SOL_CAN_RAW,CAN_RAW_FILTER,&m_filter_AngularSensor,sizeof(m_filter_AngularSensor));
    if(ret<0) {
        qDebug()<<"filter setup failed!";
    }
  }
}

void CANobj::slot_on_receiveFrame()
{
//    m_tv.tv_sec = 1;
//    m_tv.tv_usec = 0;
//    FD_ZERO(&m_rset);
//    FD_SET(m_fd)
    int s;
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

