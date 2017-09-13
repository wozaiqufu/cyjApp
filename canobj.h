#ifndef CANOBJ_H
#define CANOBJ_H
#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <time.h>
#include <QObject>

#ifndef AF_CAN
#define AF_CAN 29
#endif

#ifndef PF_CAN
#define AF_CAN AF_CAN
#endif

class CANobj : public QObject
{
    Q_OBJECT
public:
    explicit CANobj(QObject *parent = 0);
     bool initCAN(const int portIndex);
signals:

public slots:
    void slot_on_receiveFrame();
    void slot_on_sendFrame(const int id,const int length,int *data);
private:
    void extractFrame();
private:
    struct ifreq m_ifr;
    struct sockaddr_can m_addr;
    struct can_frame m_frameSend;
    struct can_frame m_frameRecv;
};

#endif // CANOBJ_H
