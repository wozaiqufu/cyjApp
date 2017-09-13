#include <QCoreApplication>
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
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

class CAN_8900 : public QObject
{
    Q_OBJECT
    public:
        CAN_8900(QObject *parent = 0);
        void CAN_Init(uchar CAN_PortNum);
        void config_send_message(ulong TargetAddress, uchar dataLength, uchar *data);
        void send_message(void) ;
        void read_message(void);
        void DataProcess(void);
        int scan;
        int nbytes;
        int ret;
        int loopback;

        uchar AutoNavStart_flag;      // 1 自主  0 遥控    自主导航启动停止标志位 message.Data[0]
        uchar Forward_flag;           // 2前进  1后退 0空挡    前进后退标志位         message.Data[1]
        bool Break_flag;             // 1 刹车  0 不刹车  刹车标志位             message.Data[2]
        uchar AngleData_H;           // 转角A／D值高二位                          message.Data[４]
        uchar AngleData_L;           // 转角A／D值低八位                          message.Data[３]
        uchar  PulseData_H;           // 里程计高二位                          　　message.Data[６]
        uchar  PulseData_L;           // 里程计值低八位                          　　message.Data[５]
        float lhd_v;
        int   AngleTemp;
        int  AngleData;
        int  PulseNum;             // 脉冲计数值                                message.Data[7]

        uchar Pos_L_X;               //  x 坐标值  低八位  message.Data[0]
        uchar Pos_H_X;               //  x 坐标值  高八位  message.Data[1]
        uchar Pos_L_Y;               //  y 坐标值  低八位  message.Data[2]
        uchar Pos_H_Y;               //  y 坐标值  高八位  message.Data[3]
        int Pos_x;
        int Pos_y;
        float y_err;
        uchar Pos_target;            //  信标标志位
        bool Heading_flag;           // 0航向角为负, 1航向角为正;              message.Data[3]
        int  Heading_angle;         // 航向角大小，                              message.Data[4]

    private slots:
    private:
        timeval timer_select;
           struct sockaddr_can addr;
           struct ifreq ifr;
           struct can_frame  senddata, receivedata;
           struct can_filter rfilter[1];
            fd_set rset;

};


