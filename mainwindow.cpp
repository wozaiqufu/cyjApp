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
  m_vehicleControlMode(1),
  m_command_accelerator(0),
  m_command_angle(0),
  m_aa(0),
  m_direction(Forward)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(slot_on_connectSICK()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(slot_on_requestSICK_Permanent()));
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(slot_on_requestSICK_PermanentStop()));
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(slot_on_initCAN()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(slot_on_readFrame()));
    connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(slot_on_sendFrame()));
    connect(&m_timer_main,SIGNAL(timeout()),this,SLOT(slot_on_mainTimer_timeout()));
    m_timer_main.start(100);
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
    //acquire data once:
    //QString qstr("\x2sRN LMDscandata\x3");
    //m_sickObj.requestSensor(qstr);
    m_sickObj.moveToThread(&m_thread_SICK);
    connect(&m_thread_SICK,SIGNAL(started()),&m_sickObj,SLOT(slot_on_requestContinousRead()));
    connect(&m_thread_SICK,SIGNAL(finished()),&m_thread_SICK,SLOT(deleteLater()));
    connect(this,SIGNAL(sig_stopPermanentReq()),&m_sickObj,SLOT(slot_on_requestContinousRead_Stop()));
    connect(this,SIGNAL(sig_informDirection(int)),&m_sickObj,SLOT(slot_on_updateDirection(int)));
    connect(&m_sickObj,SIGNAL(sigUpdateCourseAngle(int)),this,SLOT(slot_on_updateCourseAngle(int)));
    connect(&m_sickObj,SIGNAL(sigUpdateLateralOffset(int)),this,SLOT(slot_on_updateLateralOffset(int)));
    m_thread_SICK.start(QThread::HighestPriority);
}

void MainWindow::slot_on_requestSICK_PermanentStop()
{
    emit sig_stopPermanentReq();
}

void MainWindow::slot_on_initCAN()
{
    m_can.moveToThread(&m_thread_CAN);
    m_timer_CAN.setInterval(50);
    m_timer_CAN.moveToThread(&m_thread_CAN);
    connect(&m_thread_CAN,SIGNAL(started()),&m_timer_CAN,SLOT(start()));
    connect(&m_can,SIGNAL(sigUpdateCAN304(QVector<int>)),this,SLOT(slot_on_updateCAN304(QVector<int>)));
    connect(&m_can,SIGNAL(sigUpdateCAN305(QVector<int>)),this,SLOT(slot_on_updateCAN305(QVector<int>)));
    connect(&m_timer_CAN,SIGNAL(timeout()),&m_can,SLOT(slot_on_timeout()));
    connect(&m_thread_CAN,SIGNAL(finished()),&m_thread_CAN,SLOT(deleteLater()));
    m_can.initCAN(0);
}

void MainWindow::slot_on_readFrame()
{
    qDebug()<<"slot_on_readFrame";
    m_thread_CAN.start();

    //m_can.slot_on_receiveFrame();
}
//only for test
void MainWindow::slot_on_sendFrame()
{
    uchar data[8] = {0,0,0,0,0,0,0,0};
    data[0] = 10;
    data[1] = 8;
    data[2] = 10;
    data[3] = 8;
    data[4] = 10;
    data[5] = 8;
    data[6] = 10;
    data[7] = 8;
    m_can.slot_on_sendFrame(0x0161,8,data);
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

    //update vehicle params
    ui->label_spliceAngle->setText(QString::number(m_spliceAngle));
    ui->label_velocity->setText(QString::number(m_velocity));
    ui->label_courseAngle->setText(QString::number(m_courseAngle));
    ui->label_engineSpeed->setText(QString::number(m_engineSpeed));
    ui->label_lateralOffset->setText(QString::number(m_lateralOffset));
    ui->label_gear->setText(QString::number(m_gear));
    switch (m_vehicleControlMode)
    {
    case 1:
         ui->label_controlMode->setText("Local");
        break;
    case 2:
         ui->label_controlMode->setText("Visible");
        break;
    case 3:
         ui->label_controlMode->setText("Remote");
        break;
    case 4:
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
