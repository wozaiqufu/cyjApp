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
    bool loadData();
    void setAlgorithmType(const int type);
    /***************core*****************************************************/
    void setBeaconRSSIThreshold(const int threshold);
    bool matchBeacon();
    void update();
    int left() const;
    int right() const;
    int accelerator() const;
    int deaccelerator() const;
//  double *beaconLength(QVector<int> vec1, QVector<int> vec2);

signals:
    void sig_statusTable(QString);
private:
    int getBeaconIndex() const;
//    QByteArray m_beaconLength;
    QVector<int> beaconLength(const int delta);
    QVector<int> Pro_binary(QVector<int> vec) const;
public slots:
    //if control mode is Auto,receive SICK data
    void slot_on_updateControlMode(bool isAuto);
    void slot_on_updateSICKDIS(QVector<int> vec);
    void slot_on_updateSICKRSSI(QVector<int> vec);
private:
    QFile m_pathFile;
    QFile m_beaconFile;
    bool m_isAuto;
    QVector<int> m_SICKdata;
    QVector<int> m_SICKRSSI;
    QVector<int> m_beaconLength;
    QMap <int,controlCommand> m_trackMap;//memory data are loaded here:key:disp find:control command
    QList <Beacon> m_beaconAndMile;//key:left and right beacon width find:mile(calibrated)
    StageType m_stage;
    AlgorithmType m_type;
    QTextStream m_beaconTextStream;
    QTextStream m_pathTextStream;
    int m_beaconRSSIThreshold;
    int m_left;
    int m_right;
    int m_accelerator;
    int m_deaccelerator;
    int m_mile;
    static const int m_angleMax = 30;
    static const int m_acceleratorMax = 100;
    const double m_Angle_degree2Radian = 0.0174532925;
};

#endif // AUTOALGORITHM_H
