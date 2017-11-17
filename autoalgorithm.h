#ifndef AUTOALGORITHM_H
#define AUTOALGORITHM_H
#include <QVector>
#include <QObject>
#include <QStringList>
#include <QByteArray>
class TrackMemory;
class PID;

class autoAlgorithm : public QObject
{
    Q_OBJECT
public:
    enum StageType{Teach,Auto};//Teach for saving beacon.txt
    enum AlgorithmType{PIDType,TrackMemoryType,MixedType};//0,1,2
    explicit autoAlgorithm(QObject *parent = 0);
    /* *
     *public methods called by mainwindow
     * */
    void    setStageType(const int type);
    void    setAlgorithmType(const int type);
    void    setBeaconRSSIThreshold(const int threshold);
    int     left() const;//returns control left
    int     right() const;//returns control right
    int     accelerator() const;//returns control acc
    int     deaccelerator() const;
    //public methods ends
signals:
    void sig_statusTable(QString);
public slots:
    //if control mode is Auto,receive SICK data
    void slot_on_updateControlMode(bool isAuto);
    void slot_on_updateSICKDIS(QVector<int> vec);
    void slot_on_updateSICKRSSI(QVector<int> vec);
    void slot_on_updateMile(int mile);
private:
    bool            m_isAuto;
    QVector<int>    m_SICKdata;
    QVector<int>    m_SICKRSSI;
    StageType       m_stage;//teach mode or auto mode
    AlgorithmType   m_type;//PID or TrackMemory
    TrackMemory     *p_track;
    PID             *p_pid;
};

#endif // AUTOALGORITHM_H
