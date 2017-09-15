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
  m_vehicleControlMode(0),
  m_command_accelerator(0),
  m_command_angle(0)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(slot_on_connectSICK()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(slot_on_requestSICK_once()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(slot_on_requestSICK_Permanent()));
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(slot_on_requestSICK_PermanentStop()));
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(slot_on_initCAN()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(slot_on_readFrame()));
    connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(slot_on_sendFrame()));
    connect(&m_sickObj,SIGNAL(sigUpdateData(QString)),ui->label_SICKData,SLOT(setText(QString)));
    connect(&m_timer_main,SIGNAL(timeout()),this,SLOT(slot_on_mainTimer_timeout()));
    m_timer_main.setInterval(50);
    m_pMutex_CAN = new QMutex;
    m_pMutex_SICK = new QMutex;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pMutex_CAN;
    delete m_pMutex_SICK;
}

void MainWindow::slot_on_connectSICK(){
    m_sickObj.connectSensor();//if failed,sigal will be sent and this->slot_on_tcpSocketError triggered
    //m_timer_SICK.setInterval(2000);
}
void MainWindow::slot_on_requestSICK_once()
{
    QString qstr("\x2sRN LMDscandata\x3");
    m_sickObj.requestSensor(qstr);
    qDebug()<<"request :"<<qstr<<"has been sent!";
}

void MainWindow::slot_on_requestSICK_Permanent()
{
    m_sickObj.moveToThread(&m_thread_SICK);
    //m_timer_SICK.moveToThread(&m_thread_SICK);
    //connect(&m_thread_SICK,SIGNAL(started()),&m_timer_SICK,SLOT(start()));
    connect(&m_thread_SICK,SIGNAL(started()),&m_sickObj,SLOT(slot_on_requestContinousRead()));
    //connect(&m_timer_SICK,SIGNAL(timeout()),&m_sickObj,SLOT(slot_on_timeout()));
    connect(&m_thread_SICK,SIGNAL(finished()),&m_thread_SICK,SLOT(deleteLater()));
    connect(&m_sickObj,SIGNAL(sigUpdateData(QString)),ui->label_SICKData,SLOT(setText(QString)));
    connect(this,SIGNAL(sig_stopPermanentReq()),&m_sickObj,SLOT(slot_on_requestContinousRead_Stop()));
    m_thread_SICK.start(QThread::HighestPriority);
}

void MainWindow::slot_on_requestSICK_PermanentStop(){
    emit sig_stopPermanentReq();
}

void MainWindow::slot_on_initCAN()
{
    m_can.moveToThread(&m_thread_CAN);
    m_timer_CAN.setInterval(50);
    m_timer_CAN.moveToThread(&m_thread_CAN);
    connect(&m_thread_CAN,SIGNAL(started()),&m_timer_CAN,SLOT(start()));
    //connect(&m_thread_CAN,SIGNAL(started()),&m_can,SLOT(slot_on_requestContinousRead()));
    connect(&m_timer_CAN,SIGNAL(timeout()),&m_can,SLOT(slot_on_timeout()));
    connect(&m_thread_CAN,SIGNAL(finished()),&m_thread_CAN,SLOT(deleteLater()));
    connect(&m_can,SIGNAL(sigUpdateData(QString)),ui->label_CANdata,SLOT(setText(QString)));
    m_can.initCAN(0);
}

void MainWindow::slot_on_readFrame()
{
    qDebug()<<"slot_on_readFrame";
    m_thread_CAN.start();

    //m_can.slot_on_receiveFrame();
}

void MainWindow::slot_on_sendFrame()
{
    int data[2];
    data[0] = 10;
    data[1] = 8;
    m_can.slot_on_sendFrame(0x20,2,data);
}

void MainWindow::slot_on_mainTimer_timeout()
{

}
