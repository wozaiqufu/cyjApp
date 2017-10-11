#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
  m_velocity(0),
  m_engineSpeed(0),
  m_gear(0),
  m_courseAngle(0),
  m_spliceAngle(0),
  m_lateralOffset(0),
  m_command_accelerator(0),
  m_command_angle(0),
  m_controlMode(Local),
  m_direction(Forward),
  _light(0),
  _CANReady(false)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(slot_on_connectSICK()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(slot_on_requestSICK_Permanent()));
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(slot_on_requestSICK_PermanentStop()));
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(slot_on_initCAN()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(slot_on_readFrame()));
    connect(ui->pushButton_light1,SIGNAL(clicked()),this,SLOT(slot_on_sendFrame()));
    connect(ui->pushButton_light2,SIGNAL(clicked()),this,SLOT(slot_on_sendFrame2()));
    connect(ui->pushButton_bothLight,SIGNAL(clicked()),this,SLOT(slot_on_sendFrame3()));
    connect(ui->pushButton_initSurface,SIGNAL(clicked()),this,SLOT(slot_on_initSurface()));
    connect(&m_timer_main,SIGNAL(timeout()),this,SLOT(slot_on_mainTimer_timeout()));
   m_timer_main.start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_on_connectSICK(){
    m_sickObj.connectSensor();//if failed,sigal will be sent and this->slot_on_tcpSocketError triggered
    //m_timer_SICK.setInterval(2000);
}

void MainWindow::slot_on_requestSICK_Permanent()
{
    m_sickObj.moveToThread(&m_thread_SICK);
    connect(&m_thread_SICK,SIGNAL(started()),&m_sickObj,SLOT(slot_on_requestContinousRead()));
    connect(&m_thread_SICK,SIGNAL(finished()),&m_thread_SICK,SLOT(deleteLater()));
    connect(this,SIGNAL(sig_stopPermanentReq()),&m_sickObj,SLOT(slot_on_requestContinousRead_Stop()));
    connect(this,SIGNAL(sig_informDirection(int)),&m_sickObj,SLOT(slot_on_updateDirection(int)));
    connect(&m_sickObj,SIGNAL(sigUpdateCourseAngle(int)),this,SLOT(slot_on_updateCourseAngle(int)));
    connect(&m_sickObj,SIGNAL(sigUpdateLateralOffset(int)),this,SLOT(slot_on_updateLateralOffset(int)));
    //communication between surfaceComm and SICK
    connect(&m_sickObj,SIGNAL(sigUpdateData(QVector<int>)),&m_surfaceComm,SLOT(slot_on_SICKdataUpdate(QVector<int>)));
    m_thread_SICK.start(QThread::HighestPriority);
}

void MainWindow::slot_on_requestSICK_PermanentStop()
{
    emit sig_stopPermanentReq();
}

void MainWindow::slot_on_initCAN()
{
    m_can.moveToThread(&m_thread_CAN);
    m_timer_CAN.setInterval(500);
    m_timer_CAN.moveToThread(&m_thread_CAN);
    connect(&m_thread_CAN,SIGNAL(started()),&m_timer_CAN,SLOT(start()));
    connect(this,SIGNAL(sig_CAN(ulong,uchar,uchar*)),&m_can,SLOT(slot_on_sendFrame(ulong,uchar,uchar*)));
    connect(&m_can,SIGNAL(sigUpdateCAN304(QVector<int>)),this,SLOT(slot_on_updateCAN304(QVector<int>)));
    connect(&m_can,SIGNAL(sigUpdateCAN305(QVector<int>)),this,SLOT(slot_on_updateCAN305(QVector<int>)));
    connect(&m_timer_CAN,SIGNAL(timeout()),&m_can,SLOT(slot_dowork()));
    connect(&m_thread_CAN,SIGNAL(finished()),&m_thread_CAN,SLOT(deleteLater()));
    m_can.initCAN(0);


    //for test only
    _CANReady = true;
}

void MainWindow::slot_on_initSurface()
{
    m_surfaceComm.init();
    connect(&m_timer_surface,SIGNAL(timeout()),&m_surfaceComm,SLOT(slot_doWork()));
    m_timer_surface.start(2000);
}

void MainWindow::slot_on_readFrame()
{
    m_thread_CAN.start();
}
//only for test
void MainWindow::slot_on_sendFrame()
{
//    uchar data[8] = {0,0,0,0,0,0,0,0};
//    data[0] = 0x60;
//    data[1] = 0x00;
//    data[2] = 0x00;
//    data[3] = 0x00;
//    data[4] = 0x00;
//    data[5] = 0x00;
//    data[6] = 0x00;
//    data[7] = 8;
//    m_can.slot_on_sendFrame(0x0161,8,data);

    uchar data[8] = {0,0,0,0,0,0,0,0};
    data[0] = 32;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    //m_can.slot_on_sendFrame(0x0161,8,data);
    emit sig_CAN(0x0161,8,data);
}

void MainWindow::slot_on_sendFrame2()
{
    uchar data[8] = {0,0,0,0,0,0,0,0};
    data[0] = 64;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
   // m_can.slot_on_sendFrame(0x0161,8,data);
    emit sig_CAN(0x0161,8,data);
}

void MainWindow::slot_on_sendFrame3()
{
    uchar data[8] = {0,0,0,0,0,0,0,0};
    data[0] = 96;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    //m_can.slot_on_sendFrame(0x0161,8,data);
    emit sig_CAN(0x0161,8,data);
}

void MainWindow::slot_on_mainTimer_timeout()
{
    //test CAN send!
//    if(m_aa>100)
//    {
//        m_aa = 0;
//    }
//    uchar data[8] = {0,0,0,0,0,0,0,0};
//    data[0] = 10;
//    data[1] = 8;
//    data[2] = 10;
//    data[3] = 8;
//    data[4] = 10;
//    data[5] = 8;
//    data[6] = 10;
//    data[7] = m_aa;
//    m_can.slot_on_sendFrame(0x0161,8,data);
//    m_aa++;
    /***************************************************************************************
     * ************************************************************************************/
    //send command to PLC,be careful

    //if(send forward)
    emit sig_informDirection(Forward);
    //if(send backward)
    //emit sig_informDirection(Backward);
    //timer info
    if(m_timer_CAN.isActive())
    {
        ui->label_CANTimer->setText("Timer ON");
    }
    else
    {
        ui->label_CANTimer->setText("Timer OFF");
    }
    //update vehicle params
    ui->label_spliceAngle->setText(QString::number(m_spliceAngle));
    ui->label_velocity->setText(QString::number(m_velocity));
    ui->label_courseAngle->setText(QString::number(m_courseAngle));
    ui->label_engineSpeed->setText(QString::number(m_engineSpeed));
    ui->label_lateralOffset->setText(QString::number(m_lateralOffset));
    ui->label_gear->setText(QString::number(m_gear));
    switch (m_controlMode)
    {
    case Local:
         ui->label_controlMode->setText("Local");
        break;
    case Visible:
         ui->label_controlMode->setText("Visible");
        break;
    case Remote:
         ui->label_controlMode->setText("Remote");
        break;
    case Auto:
         ui->label_controlMode->setText("Auto");
        break;
    default:
        break;
    }

    if(m_direction ==Forward)
    {
        ui->label_direction->setText("Forward");
    }
    if(m_direction ==Backward)
    {
        ui->label_direction->setText("Backward");
    }

//    if(_CANReady)
//    {
//        switch (_light)
//        {
//        case 0:
//            slot_on_sendFrame();
//            _light = 1;
//            break;
//        case 1:
//            slot_on_sendFrame2();
//            _light = 2;
//            break;
//        case 2:
//            slot_on_sendFrame3();
//            _light = 0;
//            break;
//        default:
//            break;
//        }
//    }
}

void MainWindow::slot_on_updateCourseAngle(int angle)
{
    m_courseAngle = angle;
}

void MainWindow::slot_on_updateLateralOffset(int offset)
{
    m_lateralOffset = offset;
}

void MainWindow::slot_on_updateCAN304(QVector<int> vec)
{
    if(vec.size()<8)
    {
        return;
    }
    m_vector_CAN304 = vec;
    qDebug()<<"CAN304 data are:"<<m_vector_CAN304;

}

void MainWindow::slot_on_updateCAN305(QVector<int> vec)
{
    if(vec.size()<8)
    {
        return;
    }
    m_vector_CAN305 = vec;
    qDebug()<<"CAN305 data are:"<<m_vector_CAN305;
    //extract splice angle
    if(m_vector_CAN305.at(4) == 0)
    {
        return;
    }
    else
    {
        m_spliceAngle = m_vector_CAN305.at(4);
    }
    //extract velocity
    m_velocity = m_vector_CAN305.at(2);
    //extract engine speed
    m_engineSpeed = m_vector_CAN305.at(3);
    //extract
}
