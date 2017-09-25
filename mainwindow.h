#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMutex>
#include <QThread>
#include "NetAccess.h"
#include "canobj.h"
#include "surfacecommunication.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum Direction{Forward = 0,Backward};//default Forward==0
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void sig_stopPermanentReq();
    void sig_informDirection(int);
    void sig_informInfo2surface(QVector<int> vec);
    void finished();
private slots:
    void slot_on_connectSICK();
    void slot_on_requestSICK_Permanent();
    void slot_on_requestSICK_PermanentStop();
    void slot_on_initCAN();
    void slot_on_readFrame();
    void slot_on_sendFrame();
    void slot_on_initSurface();
    void slot_on_mainTimer_timeout();
public slots:
    void slot_on_updateCourseAngle(int angle);
    void slot_on_updateLateralOffset(int offset);
    void slot_on_updateCAN304(QVector<int> vec);
    void slot_on_updateCAN305(QVector<int> vec);
private:
    Ui::MainWindow *ui;
    NetAccess_SICK m_sickObj;
    CANobj m_can;
    SurfaceCommunication m_surfaceComm;
    QTimer m_timer_SICK;
    QThread m_thread_SICK;
    QTimer m_timer_CAN;
    QThread m_thread_CAN;
    QTimer m_timer_main;
    QThread m_thread_Surface;
    QTimer m_timer_surface;
    QVector<int> m_vector_CAN304;
    QVector<int> m_vector_CAN305;
    int m_velocity;
    int m_engineSpeed;
    int m_gear;
    int m_courseAngle;
    int m_spliceAngle;
    int m_lateralOffset;
    int m_vehicleControlMode;
    int m_command_accelerator;
    int m_command_angle;
    Direction m_direction;


    //test
    int m_aa;
};

#endif // MAINWINDOW_H
