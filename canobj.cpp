#include "canobj.h"
#include <QDebug>

CANobj::CANobj(QObject *parent) : QObject(parent)
  ,m_s(0) {
}

bool CANobj::initCAN(const int portIndex){
    int ret;
    if(0 == portIndex) {//CAN0 is used!
   /*********************step 1*******************************************************************/
        //create the CAN Socket
        srand(time(NULL));
        m_s = socket(PF_CAN,SOCK_RAW,CAN_RAW);
        if(m_s<0) {
            qDebug()<<"CAN initialization failed!";
            return false;
        }

    /*********************step 2*******************************************************************/
            //bind socket to specific CAN port
     //strcpy(ifr.ifr_name, "can0");
    strcpy(m_ifr.ifr_name,"can0");
    qDebug()<<"m_ifr.ifr_name:"<<m_ifr.ifr_name;
    ret = ioctl(m_s,SIOCGIFINDEX,&m_ifr);
    if(ret<0) {
        qDebug()<<"ioctl failed!";
        return false;
    }
    m_addr.can_family = PF_CAN;
    m_addr.can_ifindex = m_ifr.ifr_ifindex;
    ret = bind(m_s,(struct sockaddr*)&m_addr,sizeof(m_addr));
    if(ret<0) {
        qDebug()<<"bind failed!";
        return false;
    }
    qDebug()<<"init CAN succeed!";
    /*******************setup CAN filters***********************************************************************************/
    m_filter[0].can_id = 0x305|CAN_EFF_FLAG;
    m_filter[0].can_mask = 0xFFF;
    m_filter[1].can_id = 0x306|CAN_EFF_FLAG;
    m_filter[1].can_mask = 0xFFF;
    ret = setsockopt(m_s,SOL_CAN_RAW,CAN_RAW_FILTER,&m_filter,sizeof(m_filter));
    if(ret<0) {
        qDebug()<<"filter setup failed!";
    }
    }
}

void CANobj::getMutex(QMutex *mutex)
{
    m_pMutex = mutex;
}

void CANobj::slot_on_receiveFrame()
{
    m_tv.tv_sec = 1;
    m_tv.tv_usec = 0;
    FD_ZERO(&m_rset);
    FD_SET(m_s,&m_rset);
    int ret = select(m_s+1,&m_rset,NULL,NULL,NULL);
    if(0 == ret) {
        qDebug()<<"select timeout!";
    }
    ret = read(m_s,&m_frameRecv,sizeof(m_frameRecv));
    if(0 == ret) {
        qDebug()<<"sead failed!";
    }
    printFrame(&m_frameRecv);
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
    qDebug()<<"begin to print m_frameSend";
    printFrame(&m_frameSend);
    int nbytes=write(s,&m_frameSend,sizeof(m_frameSend));
    if (nbytes!=sizeof(m_frameSend)) {
        qDebug()<<"Send message error senddata\n";
    }
}

void CANobj::slot_on_timeout()
{
    qDebug()<<"slot_on_timeout";
    m_tv.tv_sec = 1;
    m_tv.tv_usec = 0;
    FD_ZERO(&m_rset);
    FD_SET(m_s,&m_rset);
    int ret = select(m_s+1,&m_rset,NULL,NULL,NULL);
    if(0 == ret) {
        qDebug()<<"select timeout!";
    }
    ret = read(m_s,&m_frameRecv,sizeof(m_frameRecv));
    if(0 == ret) {
        qDebug()<<"sead failed!";
    }
    printFrame(&m_frameRecv);
}

void CANobj::extractFrame()
{

}

void CANobj::printFrame(can_frame *frame)
{
    printf("%08x\n",frame->can_id & CAN_EFF_MASK);
    printf("dlc=%d\n",frame->can_dlc);
    printf("data = ");
    for(int i=0;i<frame->can_dlc;i++) {
        //printf("%02x",frame->data[i]);
        qDebug()<<frame->data[i];
    }
    qDebug()<<"printFrame";
    emit sigUpdateData(QString(frame->data[4]));
}

