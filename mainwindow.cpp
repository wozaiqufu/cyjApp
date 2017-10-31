#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
m_isNeutralGear(false),
m_isBraking(true),
m_isEmergencyStop(false),
m_isMainLight(false),
m_isHorning(false),
m_controlMode(Local),
m_isEngineStarted(false),
m_isEngineswitchMedium(true),
m_isEarthFault(false),
m_isIntegratedFault(false),
m_isOverTemperutureFault(false),
m_bucketUp(0),
m_bucketDown(0),
m_tipingBucket(0),
m_backBucket(0),
m_turnLeft(0),
m_turnRight(0),
m_accelerator(0),
m_deaccelerator(0),
m_velocity(0),
m_engineSpeed(0),
m_spliceAngle(0),
m_waterTemperature(0),
m_alarm(0),
/*************************/
m_courseAngle(0),
m_lateralOffset(0),
m_mileMeterPulse(0),
m_calibratedMile(0),
//test only
_light(0),
_CANReady(false)
{
    ui->setupUi(this);
    initTable();
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
    connect(ui->pushButton_PID,SIGNAL(clicked()),this,SLOT(slot_on_setAlgorithm_PID()));
    connect(ui->pushButton_bothLight,SIGNAL(clicked()),this,SLOT(slot_on_setAlgorithm_TrackMemory()));
    connect(ui->pushButton_openFile,SIGNAL(clicked()),this,SLOT(slot_on_openFile()));
    connect(ui->pushButton_savedata,SIGNAL(clicked()),this,SLOT(slot_on_savedata()));
    connect(ui->pushButton_readData,SIGNAL(clicked()),this,SLOT(slot_on_loadData()));
    connect(ui->pushButton_closeFile,SIGNAL(clicked()),this,SLOT(slot_on_closeFile()));
    m_timer_main.start(100);
    //signals and slots mainwindow and autoAlgorithm
    connect(this,SIGNAL(sig_autoInfo2Algorithm(bool)),&m_algorithm,SLOT(slot_on_updateControlMode(bool)));
    //signals and slots SICK and autoAlgorithm
    connect(&m_sickObj,SIGNAL(sigUpdateDIST(QVector<int>)),&m_algorithm,SLOT(slot_on_updateSICKDIS(QVector<int>)));
    connect(&m_sickObj,SIGNAL(sigUpdataRSSI(QVector<int>)),&m_algorithm,SLOT(slot_on_updateSICKRSSI(QVector<int>)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTable()
{
    ui->tableWidget->setColumnCount(2);
    QStringList headers;
    headers<<"Time"<<"Message";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    //ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    //ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    //connect(&m_can,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
    connect(&m_sickObj,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
    connect(&m_algorithm,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
    connect(&m_surfaceComm,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
    connect(this,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
}

void MainWindow::slot_on_connectSICK()
{
    m_sickObj.connectSensor();//if failed,sigal will be sent and this->slot_on_tcpSocketError triggered
    //m_timer_SICK.setInterval(2000);
}

void MainWindow::slot_on_requestSICK_Permanent()
{
//    m_sickObj.moveToThread(&m_thread_SICK);
//    connect(&m_thread_SICK,SIGNAL(started()),&m_sickObj,SLOT(slot_on_requestContinousRead()));
//    connect(&m_thread_SICK,SIGNAL(finished()),&m_thread_SICK,SLOT(deleteLater()));
//    connect(this,SIGNAL(sig_stopPermanentReq()),&m_sickObj,SLOT(slot_on_requestContinousRead_Stop()));
//    connect(this,SIGNAL(sig_informDirection(int)),&m_sickObj,SLOT(slot_on_updateDirection(int)));
//    connect(&m_sickObj,SIGNAL(sigUpdateCourseAngle(int)),this,SLOT(slot_on_updateCourseAngle(int)));
//    connect(&m_sickObj,SIGNAL(sigUpdateLateralOffset(int)),this,SLOT(slot_on_updateLateralOffset(int)));
//    m_thread_SICK.start(QThread::HighestPriority);

    m_sickObj.slot_on_requestContinousRead();
    connect(this,SIGNAL(sig_stopPermanentReq()),&m_sickObj,SLOT(slot_on_requestContinousRead_Stop()));
    connect(this,SIGNAL(sig_informDirection(int)),&m_sickObj,SLOT(slot_on_updateDirection(int)));
    connect(&m_sickObj,SIGNAL(sigUpdateCourseAngle(int)),this,SLOT(slot_on_updateCourseAngle(int)));
    connect(&m_sickObj,SIGNAL(sigUpdateLateralOffset(int)),this,SLOT(slot_on_updateLateralOffset(int)));
}

void MainWindow::slot_on_requestSICK_PermanentStop()
{
    emit sig_stopPermanentReq();
}

/*void MainWindow::slot_on_initCAN()
{
    m_can.moveToThread(&m_thread_CAN);
    m_timer_CAN.setInterval(5);
    m_timer_CAN.moveToThread(&m_thread_CAN);
    connect(&m_thread_CAN,SIGNAL(started()),&m_timer_CAN,SLOT(start()));
    connect(&m_can,SIGNAL(sigUpdateCAN306(QVector<int>)),this,SLOT(slot_on_updateCAN306(QVector<int>)));
    connect(&m_can,SIGNAL(sigUpdateCAN307(QVector<int>)),this,SLOT(slot_on_updateCAN307(QVector<int>)));
    connect(&m_timer_CAN,SIGNAL(timeout()),&m_can,SLOT(slot_dowork()));
    connect(&m_thread_CAN,SIGNAL(finished()),&m_thread_CAN,SLOT(deleteLater()));
    m_can.initCAN(0);
    //for test only
    _CANReady = true;
     //_can8900.CAN_Init(0);
}
*/

void MainWindow::slot_on_initSurface()
{
    m_surfaceComm.init();
    connect(&m_timer_surface,SIGNAL(timeout()),&m_surfaceComm,SLOT(slot_doWork()));
    connect(this,SIGNAL(sig_informInfo2surface(QVector<int>)),&m_surfaceComm,SLOT(slot_on_mainwindowUpdate(QVector<int>)));
    connect(&m_surfaceComm,SIGNAL(sig_informMainwindow(QVector<int>)),this,SLOT(slot_on_surfaceUpdate(QVector<int>)));

    m_timer_surface.start(2000);
    emit sig_statusTable("init surface!");
}

void MainWindow::slot_on_readFrame()
{
    m_thread_CAN.start();
    //_can8900.read_message();
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
    //m_can.slot_on_sendFrame(0x0161,8,data);
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
}
/****************************************************************************/
/****************************************************************************/
/*********************update data and send to CR0032*************************/
/*
TO:SICK obj~~
INFORM:direction
TO:CAN obj~~
INFORM:
*************************/
void MainWindow::slot_on_mainTimer_timeout()
{
    /*
     * UDP to surface
     * *********************************************/
    QVector<int> vec;
    int data = 0;
     /************1th byte********************/
    //direction
    if(m_direction==Forward)
    {
        data += 1;
    }
    else
    {
        data += 2;
    }
    //is neutralGear
    data += 4*m_isNeutralGear;
    //brake
    data += 8*m_isBraking;
    //emergency brake
    data += 16*m_isEmergencyStop;
    //main light
    data += 32*m_isMainLight;
    //horning
    data += 64*m_isHorning;
    vec.push_back(data);
    /************2th byte********************/
    data = 0;
    //control mode
    switch (m_controlMode)
    {
    case Local:
         data += 0;//00
        break;
    case Visible:
         data += 1;//01
        break;
    case Remote:
         data += 2;//10
        break;
    case Auto:
         data += 3;//11
        break;
    default:
        break;
    }
    //engine start or not
    if(m_isEngineStarted)
    {
        data += 4;
    }
    else
    {
        data += 8;
    }
    //engine switch medium
    if(m_isEngineswitchMedium)
    {
        data += 16;
    }
    //earth fault
    if(m_isEarthFault)
    {
        data += 32;
    }
    //integrated fault
    if(m_isIntegratedFault)
    {
        data += 64;
    }
    //over temperature fault
    if(m_isOverTemperutureFault)
    {
        data += 128;
    }
    vec.push_back(data);
    /************3th byte*************  *******/
    vec.push_back(m_bucketUp);
    /************4th byte********************/
    vec.push_back(m_bucketDown);
    /************5th byte********************/
    vec.push_back(m_tipingBucket);
    /************6th byte********************/
    vec.push_back(m_backBucket);
    /************7th byte********************/
    vec.push_back(m_turnLeft);
    /************8th byte********************/
    vec.push_back(m_turnRight);
    /************9th byte********************/
    vec.push_back(m_accelerator);
    /************10th byte********************/
    vec.push_back(m_deaccelerator);
    /************11th byte********************/
    vec.push_back(m_velocity);
    /************12th byte********************/
    vec.push_back(m_engineSpeed);
    /************13th byte********************/
    vec.push_back(m_spliceAngle);
    /************14th byte********************/
    vec.push_back(m_waterTemperature);
    /************15th byte********************/
    vec.push_back(m_alarm);
    emit sig_informInfo2surface(vec);
    /***************surface end*********************/
/************************************************************************/
    /*
     *notify algorithm
     * *********************************************/
    if(m_controlMode==Auto)
    {
         emit sig_autoInfo2Algorithm(true);
    }
    else
    {
        emit sig_autoInfo2Algorithm(false);
    }
    /***************notify algorithm end**************/
 /************************************************************************/
    /*
     *control
     * *********************************************/
    switch (m_controlMode)
    {
    case Remote:
        {
        uchar data[8] = {0,0,0,0,0,0,0,0};
        data[0] = 0x55;
        data[1] = m_surface_control_vec.at(1);//include forward,neutralGear etc.
        int temp = m_surface_control_vec.at(1)/4;
        data[2] = 4*temp;
        data[3] = m_surface_control_vec.at(3);
        data[4] = m_surface_control_vec.at(4);
        data[5] = m_surface_control_vec.at(5);
        data[6] = m_surface_control_vec.at(6);
        data[7] = m_surface_control_vec.at(7);
        m_can.slot_on_sendFrame(0x304,8,data);
        data[0] = m_surface_control_vec.at(8);
        data[1] = m_surface_control_vec.at(9);
        data[2] = m_surface_control_vec.at(10);
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        m_can.slot_on_sendFrame(0x305,8,data);
        break;
        }
    case Auto:
        {
        m_algorithm.update();
        uchar data[8] = {0,0,0,0,0,0,0,0};
        data[0] = 42;//Forward??WHY
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = m_algorithm.left();//left and right with one is zero!
        data[7] = m_algorithm.right();
        //m_can.slot_on_sendFrame(0x191,8,data);
        data[0] = m_algorithm.accelerator();//accelerator and deaccelerator with one is zero!
        data[1] = m_algorithm.deaccelerator();
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
       // m_can.slot_on_sendFrame(0x291,8,data);
        break;
        }
        break;
    default:
        break;
    }
    /***************control end**************/
/************************************************************************/
    //if(send forward)
    emit sig_informDirection(Forward);
    //if(send backward)
    //emit sig_informDirection(Backward);

    //UI Update:update vehicle params
    ui->label_spliceAngle->setText(QString::number(m_spliceAngle));
    ui->label_velocity->setText(QString::number(m_velocity));
    ui->label_courseAngle->setText(QString::number(m_courseAngle));
    ui->label_engineSpeed->setText(QString::number(m_engineSpeed));
    ui->label_lateralOffset->setText(QString::number(m_lateralOffset));
    ui->label_gear->setText(QString::number(m_isNeutralGear));
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
}

void MainWindow::slot_on_setAlgorithm_PID()
{
    m_algorithm.setAlgorithmType(0);//0 for PID
}

void MainWindow::slot_on_setAlgorithm_TrackMemory()
{
    m_algorithm.setAlgorithmType(1);//1 for track memory
}

void MainWindow::slot_on_savedata()
{
    QVector<int> vector;
    vector.push_back(3);
    vector.push_back(12);
    vector.push_back(11);
    vector.push_back(16);
    m_algorithm.saveData(vector,"path.txt");
}

void MainWindow::slot_on_openFile()
{
    emit sig_statusTable("slot_on_openFile");
    m_algorithm.initWriting("path.txt");
    m_algorithm.initWriting("beacon.txt");
}

void MainWindow::slot_on_loadData()
{
    m_algorithm.loadData();
}

void MainWindow::slot_on_closeFile()
{
    m_algorithm.closeFile("path.txt");
    m_algorithm.closeFile("beacon.txt");
}

void MainWindow::slot_on_updateCourseAngle(int angle)
{
    m_courseAngle = angle;
}

void MainWindow::slot_on_updateLateralOffset(int offset)
{
    m_lateralOffset = offset;
}

void MainWindow::slot_on_updateCAN306(QVector<int> vec)
{
    if(vec.size()<8)
    {
        return;
    }
    //extract control mode
    switch(vec.at(2)%4)
    {
    case 0:
        m_controlMode = Local;
        break;
    case 1:
        m_controlMode = Visible;
        break;
    case 2:
        m_controlMode = Remote;
        break;
    case 3:
        m_controlMode = Auto;
        break;
    default:
        break;
    }
    //extract engine start
    if((vec.at(2)/4)%2==1)
    {
        m_isEngineStarted = true;
    }
    else
    {
        m_isEngineStarted = false;
    }
    //extract engine stop
    if((vec.at(2)/8)%2==1)
    {
        m_isEngineStarted = false;
    }
    else
    {
        m_isEngineStarted = true;
    }
    //extract engine switch medium
    if((vec.at(2)/16)%2==1)
    {
        m_isEngineswitchMedium = true;
    }
    else
    {
        m_isEngineStarted = false;
    }
    m_bucketUp = vec.at(3);
    m_bucketDown = vec.at(4);
    m_tipingBucket = vec.at(5);
    m_backBucket = vec.at(6);
    m_turnLeft = vec.at(7);


}

void MainWindow::slot_on_updateCAN307(QVector<int> vec)
{
    if(vec.size()<8)
    {
        return;
    }
    m_turnRight = vec.at(0);
    m_accelerator = vec.at(1);
    m_deaccelerator = vec.at(2);
    m_velocity = vec.at(3);
    m_engineSpeed = vec.at(4);
    m_spliceAngle = vec.at(5);
    m_waterTemperature = vec.at(6);
    m_alarm = vec.at(7);
}

void MainWindow::slot_on_surfaceUpdate(QVector<int> vec)
{
    if(vec.size()!=11)//receive control from surface
    {
        return;
    }
    if(vec.at(0)!=85)//first data is const 0x55
    {
        return;
    }
    m_surface_control_vec = vec;
}
*/

void MainWindow::slot_on_surfaceUpdate(QVector<int> vec)
{
    if(vec.size()==0)
    {
        return;
    }
    m_vector_surface = vec;
}

//easy to debug:all info shows into the statusBar
void MainWindow::slot_on_updateStatusTable(QString qstr)
{
    qDebug()<<"slot_on_updateStatusBar:"<<qstr;
    qDebug()<<"rowcount:"<<ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    QTableWidgetItem *newItem = new QTableWidgetItem(QTime::currentTime().toString());
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,newItem);
    QTableWidgetItem *newItem2 = new QTableWidgetItem(qstr);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,newItem2);
}
