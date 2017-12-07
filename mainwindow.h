#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTableWidget>
#include <QThread>
#include "SICK511.h"
#include "sick400.h"
#include "CAN.h"
#include "surfacecommunication.h"
#include "autoalgorithm.h"
#include "cyjdata.h"
//test only
#include "trackmemory.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum Direction{Forward = 0,Backward};//default Forward==0
    enum ControlMode{Local,Visible,Remote,Auto};//default Local==0
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
     void initStatusTable();
     void checkControlMode();
signals:
    void sig_CAN(ulong id, uchar length, uchar *data);
    void sig_stopPermanentReq();
    void sig_informDirection(int);
	void sig_2AlgorithmRSSI(QVector<int>);
	void sig_2AlgorithmDIST(QVector<int>);
    void sig_informInfo2surface(CYJData);
    void sig_autoInfo2Algorithm(bool);
    void sig_informAlgrithmMile(int);
    void finished();
    void sig_statusTable(QString);
private slots:
    void slot_on_initSICK511();
    void slot_on_stopSICK511();
    void slot_on_initCAN();
    void slot_on_initSICK400();
    void slot_on_stopSICK400();
    void slot_on_readFrame();
    void slot_on_sendFrame();
    void slot_on_sendFrame2();
    void slot_on_sendFrame3();
    void slot_on_initSurface();
    void slot_on_mainTimer_timeout();
    void slot_on_setAlgorithm();
	void slot_on_setMode();
    void slot_on_savedata();
    void slot_on_openFile();
    void slot_on_loadData();
    void slot_on_closeFile();
public slots:
    void slot_on_updateStatusTable(QString qstr);
	void slot_on_updateForwardDIST(QVector<int> vec);
	void slot_on_updateForwardRSSI(QVector<int> vec);
	void slot_on_updateBackwardDIST(QVector<int> vec);
	void slot_on_updateBackwardRSSI(QVector<int> vec);
    void slot_on_updateForwardCourseAngle(int angle);
    void slot_on_updateForwardLateralOffset(int offset);
	void slot_on_updateBackwardCourseAngle(int angle);
	void slot_on_updateBackwardLateralOffset(int offset);
    void slot_on_updateCAN304(QVector<int> vec);
    void slot_on_updateCAN305(QVector<int> vec);
    void slot_on_surfaceUpdate(CYJData cyj);
private:
    Ui::MainWindow *ui;
    CYJData m_cyjData_surface;
    CYJData m_cyjData_actual;
	SICK511 m_sick511_f;//Forward SICK511
	SICK511 m_sick511_b;//Backward SICK511
    SICK400 m_sick400;
    CAN m_can;
    SurfaceCommunication	m_surfaceComm;
    autoAlgorithm			m_algorithm;
    QTimer					m_timer_SICK;
    QTimer					m_timer_CAN;
    QThread					m_thread_CAN;
    QTimer					m_timer_main;
    QTimer					m_timer_surface;
    /*************************************************************************
     * vehicle states to surface
	 *
	/*************************************************************************/
    Direction m_direction;
	ControlMode m_controlMode;
    int m_mileMeter;//in cm
	/*************************************************************************
	* for control algorithm use
	*
	/*************************************************************************/
    int m_courseAngle;
    int m_lateralOffset;
    int m_mileMeterPulse;
    int m_calibratedMile;
    /*
     *test only
*****************************/
    int _light;
    bool _CANReady;
	TrackMemory m_track;
    /**receive surface control
*****************************/
    QVector<int> m_surface_control_vec;

};

#endif // MAINWINDOW_H
