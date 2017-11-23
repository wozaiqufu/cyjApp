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
	/*
	* tells tech mode or auto mode
	* @input param type :0 for teach mode,1 for auto mode
	* */
    void    setStageType(const int type);
	/*
	* tells algorithm type is auto mode
	* @input param type:0 for PID, 1 for trackmemory, 2 for Mixed
	* */
    void    setAlgorithmType(const int type);
	/*
	* RSSI threshold for beacon recognization 
	* @input param threshold:RSSI value higher than it is valid
	* */
    void    setBeaconRSSIThreshold(const int threshold);
	/*
	* update:
	* if in TEACH MODE,save path.txt and beacon.txt
	* if in AUTO MODE, update acc, angle according to algorithm type
	* */
	void	update();
	/*
	* returns final left 
	* @return turn left command(opposite when backward)
	* */
    int     left() const;
	/*
	* returns final right
	* @return turn right command(opposite when backward)
	* */
    int     right() const;
	/*
	* returns final acc
	* @return acc command
	* */
    int     accelerator() const;
	/*
	* returns final deacc
	* @return deacc command
	* */
    int     deaccelerator() const;
private:
	/*
	* increase to certain value specified with inc,return true if succeed
	* add a noise scale if not increase to (origin+inc) EXACTLY
	* @return true if succeed
	* */
	bool isCertainMileIncrement(const int mile,const int inc);
signals:
    void sig_statusTable(QString);
public slots:
    //if control mode is Auto,receive SICK data
    void slot_on_updateControlMode(bool isAuto);
    void slot_on_updateSICKDIS(QVector<int> vec);
    void slot_on_updateSICKRSSI(QVector<int> vec);
	void slot_on_updateControlInfo(QVector<int> vec);
private:
    bool            m_isAuto;
    QVector<int>    m_SICKdata;
    QVector<int>    m_SICKRSSI;
	QVector<int>	m_mile_acc_deacc_left_right;//signal from mainwindow
	int				m_mile_saved;//saved data into path.txt
	int				m_mile_current;//current mile 
    StageType       m_stage;//teach mode or auto mode
    AlgorithmType   m_type;//PID or TrackMemory
    TrackMemory     *p_track;
    PID             *p_pid;
	const int		m_mileDelta = 20;//path.txt mile increment is 20cm
};

#endif // AUTOALGORITHM_H
