#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <QTimer>

#include "SICK.h"
#include "CAN_SJA1000.h"

class NAV_Control : public QObject
{
    Q_OBJECT
    public:
        NAV_Control(QObject *parent = 0); 

        void NAV_SystemStart(void);    
        void NAV_CalV(void);
        void SendCommandToNAV(void);
  //      void IdentifyFence(void);
 //       uchar Pulse_temp[4];
//*****************************
        int readtime;
        long int Delta_x ;
        long int Delta_y  ;
        float distance_li ;
        int relative_x;
        int relative_y;
        int Dis_all;
        int x;
        int y;
          int waitdata;
          int fan_bit;
        uchar Fence;
        int PulseNum0;
        int PulseNum1;
        int DELTA;
 //********************************************//    
        void NAV_nav_zhen(void);           // 正向导航算法  
        void NAV_nav_fan(void);            // 反向导航算法
        void Can_data(void);               // CAN数据处理   
        void Cal_licheng(void);               // 里程计数据处理
        void Angle_get(void);               //
        float kp,ki,kd,kp_f,ki_f,kd_f;        // 石工给的默认参数
        float k1,k2,k3,T;
        float L1,L2,L1_f,L2_f;
        float e1,e2,e,u1,e1_f,e2_f,e_f,u1_f;   // 偏差初始值
        float angle_last,angle_now;           // 转向角上一时刻数值和当前数值
        float angle_last_f,angle_now_f; 
        float head_init_err,pos_init_err;    // 航向角偏差初始值,横向位移偏差初始值
        float head_init_err_f,pos_init_err_f;
        float nav_k,nav_k_f;             // 铲运机速度,导航路径斜率
        float distance,distance_f;
        float dis_tui,dis_tui_f;
        int Pulse_sum,Pulse_sum_f;
        int direct,direct_f;
        int angle_new,angle_new_f;       // 新的转向角  
        int angle_zhuan;
//************************************************// 
        float Vaverage;      //铲运机平均速率（1秒）
              float Vaverage_Cal;
	int Youmen;           
        int maichong_data,TargetID;
        float hang_angle,zhuan_angle;
        float dis_x,dis_x_f,dis_y,dis_y_f;
        float dis_y_err,dis_y_err_f;

    private slots:
        void NAV_ControlCommandSend(void);
    private: 
        uchar CommandToNAV[8];

        uchar ControlData[4];

        CAN_8900 NAV_CAN;  
        NetAccess_SICK NAV_SICK;
        QTimer TimeCounter;                 
};

#endif

