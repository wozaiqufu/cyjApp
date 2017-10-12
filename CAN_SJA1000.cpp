
#include "CAN_SJA1000.h"

//#define AF_CAN 29
//#endif
//#ifndef PF_CAN
//#define PF_CAN AF_CAN
//#endif
//#define CAN_EFF_FLAG 0x80000000U    //扩展帧的标识
//#define CAN_RTR_FLAG 0x40000000U    //远程帧的标识
//#define CAN_ERR_FLAG 0x20000000U    //错误帧的标识，用于错误检查

static void control_c_handler (int signal_num)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Interrupted by user ... exiting.\n");
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}
/*******************************************************************/
CAN_8900::CAN_8900(QObject* parent)
    : QObject(parent)
{
    struct sigaction sig_action;
    /* Set up a way for the user to interrupt the program via Control-C */
    sig_action.sa_handler = control_c_handler;
    sigfillset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;
    if (sigaction(SIGINT, &sig_action, NULL) == -1)
    {
    error(EXIT_FAILURE, errno, "sigaction() FAILED");
    }
}
/**********Init CAN*******************/
void CAN_8900::CAN_Init(uchar CAN_PortNum)
{
    if(CAN_PortNum == 0)  /* Open CAN device ,只有0，1两个PortNum*/
     {
         srand(time(NULL));
         scan= socket(PF_CAN, SOCK_RAW, CAN_RAW);
         if (scan < 0)
         {   perror("socket PF_CAN failed"); }
         strcpy(ifr.ifr_name, "can0");
         ret = ioctl(scan, SIOCGIFINDEX, &ifr);
         if (ret < 0)
         {  perror("ioctl can0 failed"); }
         addr.can_family = PF_CAN;
         addr.can_ifindex = ifr.ifr_ifindex;
         ret = bind(scan, (struct sockaddr *)&addr, sizeof(addr));
         if (ret < 0)
         {  perror("bind  can0 failed");}
         else
         {   fprintf(stdout, "CAN%d initial successed!\n",CAN_PortNum); }
     }
    else
    {
        error(EXIT_FAILURE, errno, "no such device");
     }
}

/****************Configure********************************************/
void CAN_8900::config_send_message(ulong TargetAddress, uchar dataLength, uchar *data)
{
    senddata.can_id   =   TargetAddress;
    senddata.can_dlc =   dataLength;
    for(uchar i=0; i < dataLength; i++)
    senddata.data[i] = data[i];
}
/*****************SendData****************************************/
void CAN_8900::send_message(void)
{
        nbytes=write(scan,&senddata,sizeof(senddata));
        if (nbytes!=sizeof(senddata))
        {
            perror("Send message error senddata\n");
         }
}
/*****************ReadData**********************************************/
void CAN_8900::read_message(void)
{
    //设置过滤规则
 //      rfilter[0].can_id   = 0x306;
 //      rfilter[0].can_mask = CAN_SFF_MASK;
  //      rfilter[1].can_id   = 0x307;
 //       rfilter[1].can_mask = CAN_SFF_MASK;
  //      setsockopt(scan, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
 //       loopback = 0;
  //      setsockopt(scan, SOL_CAN_RAW,CAN_RAW_LOOPBACK,&loopback,sizeof(loopback));
        nbytes=read(scan,&receivedata,sizeof(receivedata));
        if (nbytes<(int)sizeof(receivedata))
       {
         printf("nbytes  receive = %d\n", nbytes);
         perror("Receive message error \n");
         //qDebug()<<"nbytes receive: "<<nbytes;
        }
        else
        {
            DataProcess();
         }
}
/*****************DataProcess***********************************************/
void CAN_8900::DataProcess(void)
{
    if(receivedata.can_id==0x0306)
     {
             AutoNavStart_flag    =   receivedata.data[1];          // 自主导航启动停止位
     }
    if(receivedata.can_id==0x0307)
    {
               lhd_v= receivedata.data[2]/36.0;  // convert to (m/s)
              AngleData = 89-receivedata.data[4];
             PulseData_H  =   receivedata.data[7];          // 高二位
             PulseData_L  =   receivedata.data[5];          // 低八位
             PulseNum    =   receivedata.data[7]*256+ receivedata.data[5];         // 脉冲累计数
             fprintf(stdout, "The  PulseNum is %d \n", PulseNum );
  //      fprintf(stdout, "The PulseData_L is %d (float)\n", PulseData_L );
   //         AngleData_H           =   receivedata.data[5];          // 转角A／D值高二位
   //         AngleData_L          =   receivedata.data[4];          // 转角A／D值低八位
   //          AngleTemp    =   receivedata.data[5]*256+ receivedata.data[4];
     //    fprintf(stdout, "The AngleData_H is %d (float)\n", AngleData_H );
    //     fprintf(stdout, "The AngleData_L is %d (float)\n", AngleData_L );
     //       if((AngleTemp>7800)&&(AngleTemp<7900))       // 限制波动
      //      AngleTemp=7864;
      //      AngleData    =  int( (0-(AngleTemp-7864)*0.022));  // 转换为转向角的树值
  //       AngleData        =  AngleTemp*0.022;
   //          fprintf(stdout, "The jiaojiejiao value is %d\n", AngleData);

             printf( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!/n" );
         }
}
