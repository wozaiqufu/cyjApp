#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
}

MainWindow::~MainWindow()
{
    delete ui;
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
    m_can.initCAN(0);
}

void MainWindow::slot_on_readFrame()
{
    m_can.slot_on_receiveFrame();
}

void MainWindow::slot_on_sendFrame()
{
    //m_can.slot_on_sendFrame();
}
