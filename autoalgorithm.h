#ifndef AUTOALGORITHM_H
#define AUTOALGORITHM_H
#include <QVector>
#include <QObject>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QByteArray>

typedef struct controlCommand
{
    int acc;
    int left;
    int right;
}CONTROL;

typedef struct Beacon
{
    int width_left;
    int width_right;
    int mile;
}BEACON;

class autoAlgorithm : public QObject
{
    Q_OBJECT
public:
    enum StageType{Teach,Auto};//Teach for saving beacon.txt
    enum AlgorithmType{PID,TrackMemory};
    explicit autoAlgorithm(QObject *parent = 0);
    bool initWriting(const QString fileName);
    bool saveData(QVector<int> vec,const QString fileName);
    bool closeFile(const QString fileName);
    bool initReading(const QString fileName);
    bool loadBeaconData();
    bool loadData();
    void setStageType(StageType type);
    void setAlgorithmType(const int type);
    void setBeaconRSSIThreshold(const int threshold);
    void update();//mainwindow slot_on_maintimer_timeout triggers when time out
    int left() const;//returns control left
    int right() const;//returns control right
    int accelerator() const;//returns control acc
    int deaccelerator() const;
signals:
    void sig_statusTable(QString);
private:
    /***************core*****************************************************/
    int getBeaconIndex() const;
    bool matchBeacon(const QVector<int> &vec,const double threshold);
    void matchMile();//using m_mile_calib generate acc left and right
    int calibMile();//using m_beaconIndex get mile in beacon.txt
    QVector<int> beaconLength(const int delta);
    QVector<int> Pro_binary(QVector<int> vec) const;
public slots:
    //if control mode is Auto,receive SICK data
    void slot_on_updateControlMode(bool isAuto);
    void slot_on_updateSICKDIS(QVector<int> vec);
    void slot_on_updateSICKRSSI(QVector<int> vec);
    void slot_on_updateMile(int mile);
private:
    QFile m_pathFile;
    QFile m_beaconFile;
    QFile m_beaconInfoFile;
    bool m_isAuto;
    QVector<int> m_SICKdata;
    QVector<int> m_SICKRSSI;
    QVector<int> m_beaconLength;
    QList <int> m_beacon;//beacon length actual
    QMap <int,controlCommand> m_trackMap;//memory data are loaded here:key:disp find:control command
    QList <Beacon> m_beaconAndMile;//key:left and right beacon width find:mile(calibrated)
    StageType m_stage;//teach mode or auto mode
    AlgorithmType m_type;//PID or TrackMemory
    int m_mileDeltaCalib;
    QTextStream m_beaconInfoTextStream;
    QTextStream m_beaconTextStream;
    QTextStream m_pathTextStream;
    int m_beaconRSSIThreshold;
    double m_beaconMatchThreshold;
    int m_left;
    int m_right;
    int m_accelerator;
    int m_deaccelerator;
    int m_mile;
    int m_mile_calib;
    int m_beaconMatchPre;
    int m_beaconMatchPost;
    int m_beaconIndex;
    static const int m_angleMax = 30;
    static const double m_Angle_degree2Radian = 0.0174532925;
    static const int m_acceleratorMax = 100;
};

#endif // AUTOALGORITHM_H
