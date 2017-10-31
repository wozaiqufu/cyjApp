#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMutex>
#include <QTableWidget>
#include <QThread>
#include "NetAccess.h"
#include "canobj.h"
#include "surfacecommunication.h"
#include "autoalgorithm.h"

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
     void initTable();
signals:
    void sig_CAN(ulong id, uchar length, uchar *data);
    void sig_stopPermanentReq();
    void sig_informDirection(int);
    void sig_informInfo2surface(QVector<int> vec);
    void sig_autoInfo2Algorithm(bool);
    void finished();
    void sig_statusTable(QString);
private slots:
    void slot_on_connectSICK();
    void slot_on_requestSICK_Permanent();
    void slot_on_requestSICK_PermanentStop();
    //void slot_on_initCAN();
    void slot_on_readFrame();
    void slot_on_sendFrame();
    void slot_on_sendFrame2();
    void slot_on_sendFrame3();
    void slot_on_initSurface();
    void slot_on_mainTimer_timeout();
    void slot_on_setAlgorithm_PID();
    void slot_on_setAlgorithm_TrackMemory();
    void slot_on_savedata();
    void slot_on_openFile();
    void slot_on_loadData();
    void slot_on_closeFile();
public slots:
    void slot_on_updateStatusTable(QString qstr);
    void slot_on_updateCourseAngle(int angle);
    void slot_on_updateLateralOffset(int offset);
    void slot_on_updateCAN306(QVector<int> vec);
    void slot_on_updateCAN307(QVector<int> vec);
    void slot_on_surfaceUpdate(QVector<int> vec);
private:
    Ui::MainWindow *ui;
    NetAccess_SICK m_sickObj;
    CANobj m_can;
    SurfaceCommunication m_surfaceComm;
    autoAlgorithm m_algorithm;
    QTimer m_timer_SICK;
    QThread m_thread_SICK;
    QTimer m_timer_CAN;
    QThread m_thread_CAN;
    QTimer m_timer_main;
    QTimer m_timer_surface;
    /*
     * vehicle states to surface
*****************************/
    Direction m_direction;
    bool m_isNeutralGear;
    bool m_isBraking;
    bool m_isEmergencyStop;
    bool m_isMainLight;
    bool m_isHorning;
    ControlMode m_controlMode;
    bool m_isEngineStarted;
    bool m_isEngineswitchMedium;
    bool m_isEarthFault;
    bool m_isIntegratedFault;
    bool m_isOverTemperutureFault;
    int m_bucketUp;
    int m_bucketDown;
    int m_tipingBucket;
    int m_backBucket;
    int m_turnLeft;
    int m_turnRight;
    int m_accelerator;
    int m_deaccelerator;
    int m_velocity;
    int m_engineSpeed;
    int m_spliceAngle;
    int m_waterTemperature;
    int m_alarm;
    /*
     *for control algorithm use
*****************************/
    int m_courseAngle;
    int m_lateralOffset;
    int m_mileMeterPulse;
    int m_calibratedMile;
    /*
     *test only
*****************************/
    int _light;
    bool _CANReady;

    /**receive surface control
*****************************/
    QVector<int> m_surface_control_vec;

};

#endif // MAINWINDOW_H
