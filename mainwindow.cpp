#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
m_controlMode(Local),
m_mileMeter(0),
/*************************/
m_direction(Forward),
m_courseAngle(0),
m_lateralOffset(0),
m_mileMeterPulse(0),
m_calibratedMile(0),
//test only
_light(0),
_CANReady(false)
{
    ui->setupUi(this);
	initStatusTable();
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(slot_on_initSICK511()));
	connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(slot_on_stopSICK511()));
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(slot_on_initCAN()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(slot_on_readFrame()));
    connect(ui->pushButton_light1,SIGNAL(clicked()),this,SLOT(slot_on_sendFrame()));
    connect(ui->pushButton_light2,SIGNAL(clicked()),this,SLOT(slot_on_sendFrame2()));
    connect(ui->pushButton_bothLight,SIGNAL(clicked()),this,SLOT(slot_on_sendFrame3()));
    connect(ui->pushButton_initSurface,SIGNAL(clicked()),this,SLOT(slot_on_initSurface()));
    connect(ui->pushButton_connect400,SIGNAL(clicked()),this,SLOT(slot_on_initSICK400()));
    connect(ui->pushButton_stop400,SIGNAL(clicked()),this,SLOT(slot_on_stopSICK400()));
    connect(&m_timer_main,SIGNAL(timeout()),this,SLOT(slot_on_mainTimer_timeout()));
	QButtonGroup* check_group[2];
	check_group[0] = new QButtonGroup(this);
	check_group[1] = new QButtonGroup(this);
	check_group[0]->addButton(ui->checkBox_auto);
	check_group[0]->addButton(ui->checkBox_teach);
	check_group[0]->setExclusive(true);
	check_group[1]->addButton(ui->checkBox_pid);
	check_group[1]->addButton(ui->checkBox_trackmemory);
	check_group[1]->addButton(ui->checkBox_mixed);
	check_group[1]->setExclusive(true);

	connect(ui->checkBox_pid, SIGNAL(clicked()), this, SLOT(slot_on_setAlgorithm()));
	connect(ui->checkBox_trackmemory, SIGNAL(clicked()), this, SLOT(slot_on_setAlgorithm()));
	connect(ui->checkBox_mixed, SIGNAL(clicked()), this, SLOT(slot_on_setAlgorithm()));
	connect(ui->checkBox_teach, SIGNAL(clicked()), this, SLOT(slot_on_setMode()));
	connect(ui->checkBox_auto, SIGNAL(clicked()), this, SLOT(slot_on_setMode()));
    connect(ui->pushButton_openFile,SIGNAL(clicked()),this,SLOT(slot_on_openFile()));
    connect(ui->pushButton_savedata,SIGNAL(clicked()),this,SLOT(slot_on_savedata()));
    connect(ui->pushButton_readData,SIGNAL(clicked()),this,SLOT(slot_on_loadData()));
    connect(ui->pushButton_closeFile,SIGNAL(clicked()),this,SLOT(slot_on_closeFile()));
    m_timer_main.start(30);
    //signals:mainwindow,slots:autoAlgorithm
    connect(this,SIGNAL(sig_autoInfo2Algorithm(bool)),&m_algorithm,SLOT(slot_on_updateControlMode(bool)));
    //signals:SICK,slots:autoAlgorithm

	//signals:SICK,slots:mainwindow
    connect(&m_sick511_f, SIGNAL(sigUpdateDIST(QVector<int>)), this, SLOT(slot_on_updateForwardDIST(QVector<int> vec)));
    connect(&m_sick511_f, SIGNAL(sigUpdateRSSI(QVector<int>)), this, SLOT(slot_on_updateForwardRSSI(QVector<int> vec)));
    connect(&m_sick511_b, SIGNAL(sigUpdateDIST(QVector<int>)), this, SLOT(slot_on_updateBackwardDIST(QVector<int> vec)));
    connect(&m_sick511_b, SIGNAL(sigUpdateRSSI(QVector<int>)), this, SLOT(slot_on_updateBackwardRSSI(QVector<int> vec)));
	connect(&m_sick511_f, SIGNAL(sigUpdateCourseAngle(int)), this, SLOT(slot_on_updateForwardCourseAngle(int)));
	connect(&m_sick511_f, SIGNAL(sigUpdateLateralOffset(int)), this, SLOT(slot_on_updateForwardLateralOffset(int)));
	connect(&m_sick511_b, SIGNAL(sigUpdateCourseAngle(int)), this, SLOT(slot_on_updateBackwardCourseAngle(int)));
	connect(&m_sick511_b, SIGNAL(sigUpdateLateralOffset(int)), this, SLOT(slot_on_updateBackwardLateralOffset(int)));
    //signals:MainWindow,slots:Algorithm
	connect(this, SIGNAL(sig_informAlgrithmMile(int)), &m_algorithm, SLOT(slot_on_updateMile(int))); 
	connect(this, SIGNAL(sig_2AlgorithmRSSI(QVector<int>)), &m_algorithm, SLOT(slot_on_updateSICKRSSI(QVector<int>)));
	connect(this, SIGNAL(sig_2AlgorithmDIST(QVector<int>)), &m_algorithm, SLOT(slot_on_updateSICKDIS(QVector<int>)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStatusTable()
{
    ui->tableWidget->setColumnCount(2);
    QStringList headers;
    headers<<"Time"<<"Message";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    //ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    //connect(&m_can,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
	connect(&m_sick511_f, SIGNAL(sig_statusTable(QString)), this, SLOT(slot_on_updateStatusTable(QString)));
    connect(&m_algorithm,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
    connect(&m_surfaceComm,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
    connect(this,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
}

void MainWindow::checkControlMode()
{
    if(m_cyjData_actual.remoteLocal==0&&m_cyjData_actual.automanual==0)
    {
        m_controlMode = Local;
    }
    else if(m_cyjData_actual.remoteLocal==1&&m_cyjData_actual.automanual==0)
    {
        m_controlMode = Visible;
    }
    else if(m_cyjData_actual.remoteLocal==1&&m_cyjData_actual.automanual==1)
    {
        if(m_cyjData_surface.remoteLocal==1&&m_cyjData_surface.automanual==0)
        {
            m_controlMode = Remote;
        }
        else if(m_cyjData_surface.remoteLocal==1&&m_cyjData_surface.automanual==1)
        {
            m_controlMode = Auto;
        }
    }
}

void MainWindow::slot_on_initSICK511()
{
//	connect(&m_sick511_f, SIGNAL(sigUpdateCourseAngle(int)), this, SLOT(slot_on_updateForwardCourseAngle(int)));
//	connect(&m_sick511_f, SIGNAL(sigUpdateLateralOffset(int)), this, SLOT(slot_on_updateForwardLateralOffset(int)));
//	connect(&m_sick511_b, SIGNAL(sigUpdateCourseAngle(int)), this, SLOT(slot_on_updateBackwardCourseAngle(int)));
//	connect(&m_sick511_b, SIGNAL(sigUpdateLateralOffset(int)), this, SLOT(slot_on_updateBackwardLateralOffset(int)));
	if (m_sick511_f.init("forward", "192.168.1.50", 2111))
	{
		m_sick511_f.continuousStart();
	}

	if (m_sick511_b.init("backward", "192.168.1.51", 2111))
	{
		m_sick511_b.continuousStart();
	}
}

void MainWindow::slot_on_stopSICK511()
{
    emit sig_stopPermanentReq();
}

void MainWindow::slot_on_initCAN()
{
    m_can.moveToThread(&m_thread_CAN);
    m_timer_CAN.setInterval(10);
    m_timer_CAN.moveToThread(&m_thread_CAN);
    connect(&m_thread_CAN,SIGNAL(started()),&m_timer_CAN,SLOT(start()));
    connect(&m_can,SIGNAL(sigUpdateCAN304(QVector<int>)),this,SLOT(slot_on_updateCAN304(QVector<int>)));
    connect(&m_can,SIGNAL(sigUpdateCAN305(QVector<int>)),this,SLOT(slot_on_updateCAN305(QVector<int>)));
    connect(&m_can,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
    connect(&m_timer_CAN,SIGNAL(timeout()),&m_can,SLOT(slot_dowork()));
    connect(&m_thread_CAN,SIGNAL(finished()),&m_thread_CAN,SLOT(deleteLater()));
    m_can.init(0);
//    //for test only
//    _CANReady = true;
    //     //_can8900.CAN_Init(0);
}

void MainWindow::slot_on_initSICK400()
{
    connect(&m_sick400, SIGNAL(sig_statusTable(QString)), this, SLOT(slot_on_updateStatusTable(QString)));
    if(m_sick400.init("SICK400","192.168.1.52",2111))
    {
        m_sick400.continuousStart();
    }
}

void MainWindow::slot_on_stopSICK400()
{
    m_sick400.continuousStop();
}

void MainWindow::slot_on_readFrame()
{
    m_thread_CAN.start();
    //_can8900.read_message();
}


void MainWindow::slot_on_initSurface()
{
    m_surfaceComm.init("192.168.1.3",6001);
    connect(&m_timer_surface,SIGNAL(timeout()),&m_surfaceComm,SLOT(slot_doWork()));
    connect(this,SIGNAL(sig_informInfo2surface(CYJData)),&m_surfaceComm,SLOT(slot_on_mainwindowUpdate(CYJData)));
    connect(&m_surfaceComm,SIGNAL(sig_informMainwindow(CYJData)),this,SLOT(slot_on_surfaceUpdate(CYJData)));

    m_timer_surface.start(50);
    //emit sig_statusTable("init surface!");
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
    //to surface,other are updated from CAN
    m_cyjData_actual.startdata1 = 0xAA;
    m_cyjData_actual.startdata2 = 0x55;
    m_cyjData_actual.forward = (m_direction==Forward);
    m_cyjData_actual.backward = (m_direction==Backward);
    m_cyjData_actual.enddata = 0xFF;
    emit sig_informInfo2surface(m_cyjData_actual);
    checkControlMode();
   /*
     *notify algorithm
     * *********************************************/
    if(m_controlMode==Auto)
    {
        emit sig_autoInfo2Algorithm(true);
        emit sig_informAlgrithmMile(m_mileMeter);
    }
    else
    {
        emit sig_autoInfo2Algorithm(false);
    }
    /***************notify algorithm end**************/
 /************************************************************************/
    /*
     *control
     * *******************************************************************/
    switch (m_controlMode)
    {
    case Remote:
        {
        uchar data[8] = {0,0,0,0,0,0,0,0};
        data[0] = m_cyjData_surface.forward +
                2*m_cyjData_surface.backward +
                4*m_cyjData_surface.neutral +
                8*m_cyjData_surface.stop +
                16*m_cyjData_surface.scram +
                32*m_cyjData_surface.light +
                64*m_cyjData_surface.horn +
                128*m_cyjData_surface.zero;
//        qDebug()<<"===========================================>";
//        qDebug()<<"data from surface forward:"<<m_cyjData_surface.forward;
//        qDebug()<<"data from surface backward:"<<m_cyjData_surface.backward;
//        qDebug()<<"data from surface neutral:"<<m_cyjData_surface.neutral;
//        qDebug()<<"data from surface stop:"<<m_cyjData_surface.stop;
//        qDebug()<<"data from surface scram:"<<m_cyjData_surface.scram;
//        qDebug()<<"data from surface light:"<<m_cyjData_surface.light;
//        qDebug()<<"data from surface horn:"<<m_cyjData_surface.horn;
//        qDebug()<<"data[0]:"<<data[0];
        data[1] = 4*m_cyjData_surface.start +
                8*m_cyjData_surface.flameout +
                16*m_cyjData_surface.middle +
                32*m_cyjData_surface.warn1 +
                64*m_cyjData_surface.warn2 +
                128*m_cyjData_surface.warn3;

        data[2] = m_cyjData_surface.rise;
        data[3] = m_cyjData_surface.fall;
        data[4] = m_cyjData_surface.turn;
        data[5] = m_cyjData_surface.back;
        data[6] = m_cyjData_surface.left;
        data[7] = m_cyjData_surface.right;
        m_can.slot_on_sendFrame(0x191,8,data);
        data[0] = m_cyjData_surface.acc;
        data[1] = m_cyjData_surface.deacc;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        m_can.slot_on_sendFrame(0x291,8,data);
        break;
        }
    case Auto:
        {
        m_algorithm.update();
        uchar data[8] = {0,0,0,0,0,0,0,0};
        data[0] = m_cyjData_surface.forward +
                2*m_cyjData_surface.back +
                4*m_cyjData_surface.neutral +
                8*m_cyjData_surface.stop +
                16*m_cyjData_surface.scram +
                32*m_cyjData_surface.light +
                64*m_cyjData_surface.horn +
                128*m_cyjData_surface.zero;
        data[1] = 4*m_cyjData_surface.start +
                8*m_cyjData_surface.flameout +
                16*m_cyjData_surface.middle +
                32*m_cyjData_surface.warn1 +
                64*m_cyjData_surface.warn2 +
                128*m_cyjData_surface.warn3;
        data[2] = m_cyjData_surface.rise;
        data[3] = m_cyjData_surface.fall;
        data[4] = m_cyjData_surface.turn;
        data[5] = m_cyjData_surface.back;
        data[6] = m_cyjData_surface.left;
        data[7] = m_algorithm.left();
       // m_can.slot_on_sendFrame(0x304,8,data);
        data[0] = m_algorithm.right();
        data[1] = m_algorithm.accelerator();
        data[2] = m_algorithm.deaccelerator();;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
       // m_can.slot_on_sendFrame(0x305,8,data);
        break;
        }
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
    ui->label_spliceAngle->setText(QString::number(m_cyjData_actual.spliceAngle));
    ui->label_velocity->setText(QString::number(m_cyjData_actual.speed));
    ui->label_courseAngle->setText(QString::number(m_courseAngle));
    ui->label_engineSpeed->setText(QString::number(m_cyjData_actual.engine));
    ui->label_lateralOffset->setText(QString::number(m_lateralOffset));
    ui->label_gear->setText(QString::number(m_cyjData_actual.neutral));
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
    //console output
 //   qDebug()<<"=========================actual data are:";
//    qDebug()<<"neutral:"<<m_cyjData_actual.neutral;
//    qDebug()<<"stop:"<<m_cyjData_actual.stop;
//    qDebug()<<"scram:"<<m_cyjData_actual.scram;
//    qDebug()<<"light:"<<m_cyjData_actual.light;
//    qDebug()<<"horn:"<<m_cyjData_actual.horn;
//    qDebug()<<"start:"<<m_cyjData_actual.start;
//    qDebug()<<"flameout:"<<m_cyjData_actual.flameout;
//    qDebug()<<"middle:"<<m_cyjData_actual.middle;
//    qDebug()<<"rise:"<<m_cyjData_actual.rise;
//    qDebug()<<"fall:"<<m_cyjData_actual.fall;
//    qDebug()<<"turn:"<<m_cyjData_actual.turn;
//    qDebug()<<"back:"<<m_cyjData_actual.back;
//    qDebug()<<"left:"<<m_cyjData_actual.left;
//    qDebug()<<"right:"<<m_cyjData_actual.right;
//    qDebug()<<"acc:"<<m_cyjData_actual.acc;
//    qDebug()<<"deacc:"<<m_cyjData_actual.deacc;
//    qDebug()<<"speed:"<<m_cyjData_actual.speed;
//    qDebug()<<"engine:"<<m_cyjData_actual.engine;
//    qDebug()<<"splice:"<<m_cyjData_actual.spliceAngle;
  //  qDebug()<<"=============================================";
}

void MainWindow::slot_on_setAlgorithm()
{
	if (ui->checkBox_pid->checkState())
	{
		m_algorithm.setAlgorithmType(0);
	}
	else if (ui->checkBox_trackmemory->checkState())
	{
		m_algorithm.setAlgorithmType(1);
	}
	else
		m_algorithm.setAlgorithmType(2);
}

void MainWindow::slot_on_setMode()
{
	if (ui->checkBox_teach->checkState())
	{
		m_algorithm.setStageType(0);
	}
	else
	{
		m_algorithm.setStageType(1);
	}
}

void MainWindow::slot_on_savedata()
{
    QVector<int> vector;
	vector.push_back(2);
    vector.push_back(3);
    vector.push_back(12);
    vector.push_back(11);
    vector.push_back(16);
	m_track.saveData("path.txt", vector);
	vector.clear();
	vector.push_back(20);
	vector.push_back(20);
	vector.push_back(200);
	m_track.saveData("beacon.txt", vector);
}

void MainWindow::slot_on_openFile()
{
    emit sig_statusTable("slot_on_openFile");
    //m_algorithm.initWriting("path.txt");
    //m_algorithm.initWriting("beacon.txt");
}

void MainWindow::slot_on_loadData()
{
	//m_track.loadData("path.txt");
	m_track.loadData("beaconRaw.txt");
	//m_track.loadData("beacon.txt");
}

void MainWindow::slot_on_closeFile()
{
    //m_algorithm.closeFile("path.txt");
    //m_algorithm.closeFile("beacon.txt");
}

void MainWindow::slot_on_updateForwardDIST(QVector<int> vec)
{
	if (m_direction == Forward)
	{
		emit sig_2AlgorithmDIST(vec);
	}
}
void MainWindow::slot_on_updateForwardRSSI(QVector<int> vec)
{
	if (m_direction == Forward)
	{
		emit sig_2AlgorithmRSSI(vec);
	}
}
void MainWindow::slot_on_updateBackwardDIST(QVector<int> vec)
{
    if (m_direction == Backward)
    {
        emit sig_2AlgorithmDIST(vec);
    }
}
void MainWindow::slot_on_updateBackwardRSSI(QVector<int> vec)
{
	if (m_direction == Backward)
	{
		emit sig_2AlgorithmRSSI(vec);
	}
}

void MainWindow::slot_on_updateForwardCourseAngle(int angle)
{
	if (m_direction == Forward)
		m_courseAngle = angle;
}

void MainWindow::slot_on_updateForwardLateralOffset(int offset)
{
	if (m_direction == Forward)
		m_lateralOffset = offset;
}

void MainWindow::slot_on_updateBackwardCourseAngle(int angle)
{
	if (m_direction == Backward)
		m_courseAngle = angle;
}

void MainWindow::slot_on_updateBackwardLateralOffset(int offset)
{
	if (m_direction == Backward)
		m_lateralOffset = offset;
}

void MainWindow::slot_on_updateCAN304(QVector<int> vec)
{
    //qDebug()<<"CAN304:"<<vec;
    if(vec.size()<8)
    {
        return;
    }
    //extract Data[0]
    if((vec.at(0)/4)%2==1)
    {
        m_cyjData_actual.neutral = 1;
    }
    else
    {
        m_cyjData_actual.neutral = 0;
    }
    if((vec.at(0)/4)%2==1)
    {
        m_cyjData_actual.stop = 1;
    }
    else
    {
        m_cyjData_actual.stop = 0;
    }

    if((vec.at(0)/8)%2==1)
    {
        m_cyjData_actual.scram = 1;
    }
    else
    {
        m_cyjData_actual.scram = 0;
    }

    if((vec.at(0)/16)%2==1)
    {
        m_cyjData_actual.light = 1;
    }
    else
    {
        m_cyjData_actual.light = 0;
    }

    if((vec.at(0)/32)%2==1)
    {
        m_cyjData_actual.horn = 1;
    }
    else
    {
        m_cyjData_actual.horn = 0;
    }
    //reserved
    m_cyjData_actual.zero = (vec.at(0)/64)%2;
    //extract control mode
    switch(vec.at(1)%4)
    {
    case 0:
        m_controlMode = Local;
        m_cyjData_actual.remoteLocal = 0;
        m_cyjData_actual.automanual = 0;
        break;
    case 1:
        m_controlMode = Visible;
        m_cyjData_actual.remoteLocal = 0;
        m_cyjData_actual.automanual = 0;
        break;
    case 2:
        m_controlMode = Remote;
        m_cyjData_actual.remoteLocal = 1;
        m_cyjData_actual.automanual = 0;
        break;
    case 3:
        m_controlMode = Auto;
        m_cyjData_actual.remoteLocal = 1;
        m_cyjData_actual.automanual = 1;
        break;
    default:
        break;
    }
    //extract engine start
    m_cyjData_actual.start = (vec.at(1)/2)%2;
    //extract engine stop
    m_cyjData_actual.flameout = (vec.at(1)/4)%2;
    //extract engine switch medium
    m_cyjData_actual.middle = (vec.at(1)/8)%2;
    //extract warn1
    m_cyjData_actual.warn1 = (vec.at(1)/16)%2;
    //extract warn2
    m_cyjData_actual.warn2 = (vec.at(1)/32)%2;
    //extract warn3
    m_cyjData_actual.warn3 = (vec.at(1)/64)%2;

    m_cyjData_actual.rise = vec.at(2);
    m_cyjData_actual.fall = vec.at(3);
    m_cyjData_actual.turn = vec.at(4);
    m_cyjData_actual.back = vec.at(5);
    m_cyjData_actual.left = vec.at(6);
    m_cyjData_actual.right = vec.at(7);
}

void MainWindow::slot_on_updateCAN305(QVector<int> vec)
{
    //qDebug()<<"CAN305:"<<vec;
    if(vec.size()<8)
    {
        return;
    }
    m_cyjData_actual.acc = vec.at(0);
    m_cyjData_actual.deacc = vec.at(1);
    m_cyjData_actual.speed = vec.at(2);
    m_cyjData_actual.engine = vec.at(3);
    m_cyjData_actual.spliceAngle = vec.at(4);
    m_cyjData_actual.oil = vec.at(5);
    m_mileMeter = vec.at(6)*256 + vec.at(7);
}

void MainWindow::slot_on_surfaceUpdate(CYJData cyj)
{
    m_cyjData_surface.forward = cyj.forward;
    m_cyjData_surface.backward = cyj.backward;
    m_cyjData_surface.neutral = cyj.neutral;
    m_cyjData_surface.stop = cyj.stop;
    m_cyjData_surface.scram = cyj.scram;
    m_cyjData_surface.light = cyj.light;
    m_cyjData_surface.horn = cyj.horn;
    m_cyjData_surface.zero = cyj.zero;
    m_cyjData_surface.automanual = cyj.automanual;
    m_cyjData_surface.remoteLocal = cyj.remoteLocal;
    m_cyjData_surface.start = cyj.start;
    m_cyjData_surface.flameout = cyj.flameout;
    m_cyjData_surface.middle = cyj.middle;
    m_cyjData_surface.rise = cyj.rise;
    m_cyjData_surface.fall = cyj.fall;
    m_cyjData_surface.turn = cyj.turn;
    m_cyjData_surface.back = cyj.back;
    m_cyjData_surface.left = cyj.left;
    m_cyjData_surface.right = cyj.right;
    m_cyjData_surface.acc = cyj.acc;
    m_cyjData_surface.deacc = cyj.deacc;


    if(m_cyjData_surface.automanual==0&&m_cyjData_surface.remoteLocal==0)
    {
        m_controlMode = Local;
    }
    else if(m_cyjData_surface.automanual==0&&m_cyjData_surface.remoteLocal==0)
    {

    }

//    qDebug()<<"===========================================>";
//    qDebug()<<"data from surface forward:"<<m_cyjData_surface.forward;
//    qDebug()<<"data from surface backward:"<<m_cyjData_surface.backward;
//    qDebug()<<"data from surface neutral:"<<m_cyjData_surface.neutral;
//    qDebug()<<"data from surface stop:"<<m_cyjData_surface.stop;
//    qDebug()<<"data from surface scram:"<<m_cyjData_surface.scram;
//    qDebug()<<"data from surface light:"<<m_cyjData_surface.light;
//    qDebug()<<"data from surface horn:"<<m_cyjData_surface.horn;
//    qDebug()<<"data from surface zero:"<<m_cyjData_surface.zero;
//    qDebug()<<"data from surface start:"<<m_cyjData_surface.start;
//    qDebug()<<"data from surface flameout:"<<m_cyjData_surface.flameout;
//    qDebug()<<"data from surface middle:"<<m_cyjData_surface.middle;
//    qDebug()<<"data from surface remoteLocal:"<<m_cyjData_surface.remoteLocal;
//    qDebug()<<"data from surface automanual:"<<m_cyjData_surface.automanual;
//    qDebug()<<"data from surface rise:"<<m_cyjData_surface.rise;
//    qDebug()<<"data from surface fall:"<<m_cyjData_surface.fall;
//    qDebug()<<"data from surface turn:"<<m_cyjData_surface.turn;
//    qDebug()<<"data from surface back:"<<m_cyjData_surface.back;
//    qDebug()<<"data from surface left:"<<m_cyjData_surface.left;
//    qDebug()<<"data from surface right:"<<m_cyjData_surface.right;
//    qDebug()<<"data from surface acc:"<<m_cyjData_surface.acc;
//    qDebug()<<"data from surface deacc:"<<m_cyjData_surface.deacc;
}

//easy to debug:all info shows into the statusBar
void MainWindow::slot_on_updateStatusTable(QString qstr)
{
    //qDebug()<<"slot_on_updateStatusBar:"<<qstr;
    //qDebug()<<"rowcount:"<<ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    QTableWidgetItem *newItem = new QTableWidgetItem(QTime::currentTime().toString());
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,newItem);
    QTableWidgetItem *newItem2 = new QTableWidgetItem(qstr);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,newItem2);
}
