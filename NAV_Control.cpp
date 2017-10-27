#include <QtCore>
#include "NAV_Control.h"

NAV_Control::NAV_Control(QObject* parent)
    : QObject(parent)
{
    NAV_CAN.CAN_Init(0);              // 打开CAN1   
    kp=25;
    ki=0;
    kd=0;            //石工给的默认参数
    k1=0.15;
    k2=0.425;
    k3=0.425;
    L1=1.270;
    L2=1.270;
    T=0.2;

    e1=0;
    e2=0;
    e=0;
    u1=0;                                                  // 偏差初始值为0
    head_init_err = 0*3.1415/180;            // 航向角偏差初始值
    pos_init_err  = 0;                                 // 横向位移偏差初始值
    nav_k=0;                                             // 规划路径斜率始终不变
    direct =1;                                            //  铲运机转弯方向 1左0右
    distance    = 0;                                   //  正向行驶里程
    waitdata  = 0;
     Delta_x = 0;
     Delta_y = 0 ;
     fan_bit  = 0;            //  正反向标志
    dis_tui     = 0;
    Pulse_sum   = 0;
    Youmen      = 0;                         // 油门开关量
    hang_angle  = 0;
    zhuan_angle = 0;
    x = 0;
    y = 0;
    dis_x       = 0;
    dis_y       = 0;
    dis_y_err   = 0;
}
/**********************************************************************************************************/
void NAV_Control::NAV_SystemStart()        //每隔200毫秒程序循环执行一次
{
   //NAV_SICK.SendCommandToSICK(0);   //发送控制指令，该例中发送0号指令，即开始传输数据指令
    TimeCounter.start(20);
    connect(&TimeCounter, SIGNAL(timeout()), this, SLOT(NAV_ControlCommandSend()));
}
/**********************************************************************************************************/
void NAV_Control::NAV_ControlCommandSend()
{


    static int first_read= 0;                     //第一次读取CAN数据
 //   IdentifyFence();
     NAV_CAN.read_message();
    Can_data();                                     // 数据处理
      NAV_CAN.read_message();
    NAV_CalV();                                                      // 计算铲运机实际速度
    if(NAV_CAN.AutoNavStart_flag==1)        // 判断是自主导航控制还是遥控控制
    {
          if(first_read==0)                                   // 读当前传感器数值
          {
                 angle_last   =  zhuan_angle;                  // 上一时刻转角值
                 angle_now   =  zhuan_angle;                  // 当前时刻转向角
                 head_init_err= hang_angle;                   // 上一时刻航向角偏差
                 pos_init_err = dis_y_err;                    // 上一时刻横向位移偏差
                 first_read   = 1;
           }
          else                                                        // 进入导航
          {
              if(fan_bit==0)
               {
                   NAV_nav_zhen();                                           // 进入自主导航控制程序
                }
              else if(fan_bit ==1)
              {
                   NAV_nav_fan();
              }
        }
    }
    //         fprintf(stdout, "hang_angle = %f\n",hang_angle);
                fprintf(stdout, "vecity  = %f\n",Vaverage);
                fprintf(stdout, "vecity_cal  = %f\n",Vaverage_Cal);
      //      fprintf(stdout, "pos_init_err = %f\n",pos_init_err);
      //      fprintf(stdout, "head_init_err = %f\n",head_init_err);
     //           fprintf(stdout, "The dis_x is %f\n",dis_x);
                fprintf(stdout, "Leiji  xingshi licheng  = %f\n",distance);
                 fprintf(stdout, "Distance_li = %f\n",distance_li);
   //         fprintf(stdout, "The dis_tui value %f\n",dis_tui);
               fprintf(stdout, "The Jiaojie AngleData  is %d\n", NAV_CAN.AngleData);
    //           fprintf(stdout, "THE Heading Angle = %d\n",NAV_SICK.Angle);
   //            fprintf(stdout, "The TargetID is %d\n",Fence);
             printf("###################################### \n\n");



    }


/**********************************************************************************************************/
//void NAV_Control::Cal_licheng(void)
//{

//}
/**********************************************************************************************************/
void NAV_Control::Can_data(void)
{
    NAV_CAN.read_message();              // 读CAN总线，接收主控单元传输过来的控制指令
    //hang_angle      =  NAV_SICK.Angle*3.1415/180.0;
    //zhuan_angle     =  NAV_CAN.AngleData*3.1415/180.0;
    maichong_data =  NAV_CAN.PulseNum;
 // dis_x  = x;
 // dis_y  = y ;
      //dis_y_err   =  NAV_SICK.dis_err;
 // dis_y_err     =  (dis_y-2000)/1000.0;
//  if((dis_y<2020)&&(dis_y>1980))             //  扰动误差修正
//     {
 //      dis_y_err= 0;
 //    }
}

/**********************************************************************************************************/
void NAV_Control::NAV_CalV(void)
{
    Vaverage_Cal =(maichong_data*0.5)/1000.0;
    Vaverage =NAV_CAN.lhd_v;
    distance_li =distance_li+Vaverage*0.3*360;   //见数据分析；
    distance=distance+(maichong_data*0.5);         // 计算正向行驶里程
}
/**********************************************************************************************************/

void NAV_Control::NAV_nav_zhen(void)
{  

     uchar ConToCR_191[8] = {0,0,0,0,0,0,0,0};
     uchar ConToCR_291[8] = {0,0,0,0,0,0,0,0};
    float vec_temp;
  //          float nav_k_temp,head_err,head_k,head_kk,pos_err;
 //  float temp,temp1,temp2,temp3,temp4;
   float u,u_err,pwm_value;
     float u_value;
     float pwm_larger;
     vec_temp   =  Vaverage;                      // 替换成现在的实际速度
     e=dis_y_err;
 //    nav_k_temp =  nav_k;
  //   head_err   =  head_init_err;
    // pos_err    =  pos_init_err;
     
//     temp       =  vec_temp*nav_k_temp*cos(head_err);
  //   head_k     =  temp/(1+nav_k_temp*pos_err);      //计算目标路径航向角变化率

 //    temp1      =  vec_temp*sin(angle_now)/(L2+L1*cos(angle_now));
  //   temp2      =  L2*(angle_now-angle_last);
   //  temp3      =  temp2/((L2+L1*cos(angle_now))*T);
   //  head_kk    =  temp1+temp3;                //计算跟踪轨迹航向角变化率
 //    temp4 = head_k-head_kk;                   //航向角偏差变化率
 //    e=k1*pos_err+k2*head_err+k3*temp4; //计算综合反馈

  //   u_err=kp*(e-e1)+ki*e+kd*(e-2*e1+e2);      //计算控制增量
     u_err=kp*(e-e1)+ki*e+kd*(e-2*e1+e2);
   
     u=u1+u_err;                               //导航控制器输出
      fprintf(stdout, "uuuuuuuuuuuuuuuuu = %f\n",u);

   //  dis_tui  = dis_tui+(maichong_data*1.08*cos(head_err)/(1+nav_k_temp*pos_err));

/************输出限幅处理**************************/
     u_value=u;   
     if(u_value>12)
        {u_value=12;}
     if(u_value<-12)
        {u_value=-12;}
     if(u_value>=0)
        {
         direct=1;
         u_value=u_value+6;
       pwm_value = (1.0777-0.00925*u_value)*u_value*100/24.0;
  //       pwm_value  = u_value*5;
         pwm_larger=50+75*vec_temp;

          printf ("000000000000 pwm_value  is %f\n",pwm_value);
         if(pwm_larger>=127)
              {pwm_larger=127;}
         if(pwm_value>pwm_larger)
              {pwm_value=pwm_larger;}
         angle_new =int(pwm_value);
         } 
         
   if(u_value<0)
        {
         direct=0;
         u_value=-u_value+6;

     //    pwm_value = (1.0777-0.00925*u_value)*u_value*100/24.0;
         pwm_value  = u_value*5;
               printf ("000000000000 pwm_value  is %f\n",pwm_value); //-0.448
         pwm_larger=55+75*vec_temp;
         if(pwm_larger>=127)
              {pwm_larger=127;}
         if(pwm_value>pwm_larger)
              {pwm_value=pwm_larger;}
         angle_new =int(pwm_value);   //45
         }      
/************迭代处理**************************/
     e2=e1;
     e1=e;
     u1=u;
     angle_last  =  angle_now; 
     angle_now   =  zhuan_angle;

     printf ("The direct   is %d\n",direct);
     printf ("@@@@@@@ angle_new  is %d\n",angle_new);


/************输出处理**************************/
         if(distance <= 3000)                           // 第一个直线段
        {
         head_init_err = hang_angle;
         pos_init_err  = dis_y_err;
         nav_k    =  0;
         //191      backup  speaker light stop   release  kongdang  back forward
         //                0           0        1     0          1          0            0         1
         ConToCR_191[0]=42;                //forward
         ConToCR_191[1]=0;
         ConToCR_191[2]=0;
         ConToCR_191[3]=0;
         ConToCR_191[4]=0;
         ConToCR_191[5]=0;

          ConToCR_291[0]=60;                     // 油门加速（0～127）
          ConToCR_291[1]=0;                       // 油门减速（0～127）
/*左转和右转*/
         if(direct==1)
         {
            ConToCR_191[6]=angle_new;                     //  油门左传（0～127）
            ConToCR_191[7]=0;
         }
         if(direct==0)
         {
         ConToCR_191[6]=0;
         ConToCR_191[7]=angle_new;                     //  油门右传（0～127）
         }
      }
         if((distance>3000) &&(distance <= 5000)  )                         // 第一个直线段
         {
             head_init_err = hang_angle;
             pos_init_err  = dis_y_err;
             nav_k    =  0;
             //191      backup  speaker light stop   release  kongdang  back forward
             //                0           0        1     0          1          0            0         1
             ConToCR_191[0]=42;                //forward
             ConToCR_191[1]=0;
             ConToCR_191[2]=0;
             ConToCR_191[3]=0;
             ConToCR_191[4]=0;
             ConToCR_191[5]=0;
              ConToCR_291[0]=60;                     // 油门加速（0～127）
              ConToCR_291[1]=0;                       // 油门减速（0～127）
    /*左转和右转*/
             if(direct==1)
             {
                ConToCR_191[6]=angle_new;                     //  油门左传（0～127）
                ConToCR_191[7]=0;
             }
             if(direct==0)
             {
             ConToCR_191[6]=0;
             ConToCR_191[7]=angle_new;                     //  油门右传（0～127）
             }
          }
         if((distance>5000) &&(distance <= 30000)  )                         // 第一个直线段
         {
             head_init_err = hang_angle;
             pos_init_err  = dis_y_err;
             nav_k    =  0;
             //191      backup  speaker light stop   release  kongdang  back forward
             //                0           0        1     0          1          0            0         1
             ConToCR_191[0]=42;                //forward
             ConToCR_191[1]=0;
             ConToCR_191[2]=0;
             ConToCR_191[3]=0;
             ConToCR_191[4]=0;
             ConToCR_191[5]=0;
              ConToCR_291[0]=60;                     // 油门加速（0～127）
              ConToCR_291[1]=0;                       // 油门减速（0～127）
    /*左转和右转*/
             if(direct==1)
             {
                ConToCR_191[6]=angle_new;                     //  油门左传（0～127）
                ConToCR_191[7]=0;
             }
             if(direct==0)
             {
             ConToCR_191[6]=0;
             ConToCR_191[7]=angle_new;                     //  油门右传（0～127）
             }
          }

               if((distance>30000) &&(distance <= 40000)  )                          // 第一个直线段
              {
                  head_init_err = hang_angle;
                  pos_init_err  = dis_y_err;
                  nav_k    =  0;
                  //191      backup  speaker light stop   release  kongdang  back forward
                  //                0           0        1     0          1          0            0         1
                  ConToCR_191[0]=42;                //forward
                  ConToCR_191[1]=0;
                  ConToCR_191[2]=0;
                  ConToCR_191[3]=0;
                  ConToCR_191[4]=0;
                  ConToCR_191[5]=0;

                   ConToCR_291[0]=60;                     // 油门加速（0～127）
                   ConToCR_291[1]=0;                       // 油门减速（0～127）
         /*左转和右转*/
                  if(direct==1)
                  {
                     ConToCR_191[6]=angle_new;                     //  油门左传（0～127）
                     ConToCR_191[7]=0;
                  }
                  if(direct==0)
                  {
                  ConToCR_191[6]=0;
                  ConToCR_191[7]=angle_new;                     //  油门右传（0～127）
                  }
               }
            if(distance>40000)
             {
                   //191      backup  speaker light stop   release  kongdang  back forward
                   //                0           0        1     0          1          1           0         0
                   ConToCR_191[0]=44;                //kongdang
                   ConToCR_291[0]=0;                     // 油门加速（0～127）
                   ConToCR_291[1]=0;                       // 油门减速（0～127）
              }
            if(distance>45000)
             {
                   //191      backup  speaker light stop   release  kongdang  back forward
                   //                0           0        1     0          1          1           0         0
                   ConToCR_191[0]=36;                //驻车制动
                   ConToCR_291[0]=0;                     // 油门加速（0～127）
                   ConToCR_291[1]=0;                       // 油门减速（0～127）
              }

              NAV_CAN.config_send_message(0x0191, 8, ConToCR_191);
              NAV_CAN.send_message();
              NAV_CAN.config_send_message(0x0291, 8, ConToCR_291);
              NAV_CAN.send_message();
         }


void NAV_Control::NAV_nav_fan(void)
{
     uchar ConToCR_191[8] = {0,0,0,0,0,0,0,0};
     uchar ConToCR_291[8] = {0,0,0,0,0,0,0,0};
     if(distance<12000)
     {
              //191      backup  speaker light stop   release  kongdang  back forward
              //                0           0        1     0          1          0            0       1
             ConToCR_191[0]=42;                //back
             ConToCR_191[1]=0;
             ConToCR_191[2]=0;
             ConToCR_191[3]=0;
             ConToCR_191[4]=0;
             ConToCR_191[5]=0;
             ConToCR_291[0]=40;                     // 油门加速（0～127）
             ConToCR_291[1]=0;                       // 油门减速（0～127）
            /*左转和右转*/
              if(direct==1)
                         {
                           ConToCR_191[6]=angle_new;                     //  油门左传（0～127）
                     //          ConToCR_191[6]=0;
                               ConToCR_191[7]=0;
                         }
              if(direct==0)
                         {
                             ConToCR_191[6]=0;
                  //           ConToCR_191[7]=0;
                         ConToCR_191[7]=angle_new;                     //  油门右传（0～127）
                          }
              printf("    GO TO THE  START POINT !!!! \n\n");
      }
     NAV_CAN.config_send_message(0x0191, 8, ConToCR_191);
     NAV_CAN.send_message();
     NAV_CAN.config_send_message(0x0291, 8, ConToCR_291);
     NAV_CAN.send_message();
  }
/**********************************************************************************************************/
/**********************************************************************************************************/
/*****************************************************************************************************/

