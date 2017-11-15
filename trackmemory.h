#ifndef TRACKMEMORY_H
#define TRACKMEMORY_H
#include <QVector>

class TrackMemoryImpl;

class TrackMemory
{
public:
    TrackMemory();
    ~TrackMemory();
    /*
     * init important params in the algorithm
     * @input param rssiThreshold  :RSSI value higher than it is valid
     * @input param beaconLengthThr:
     * actualLength*(1-beaconLengthThr)~actualLength*(1-beaconLengthThr)
     * */
    void init(const int rssiThreshold,const double beaconLengthThr,const int accmax,const int accmin,const int angmax,const int angmin);
    /*
     * update control according to mile
     * @input param mile:original mile from mile meter
     * @input param dist:SICK distance data
     * @input param rssi:SICK RSSI     data
     * */
    QVector<int>  update(int mile, QVector<int> dist, QVector<int> rssi);
    /*
     * data are saved into path.txt and beacon.txt
     * @intput param fileName:file name of saving
     * @intput param vec:vec[0]:data length;vec[1]...data
     * */
    bool saveData(const QString fileName,QVector<int> vec);

private:
    TrackMemoryImpl *m_Impl;
};

#endif // TRACKMEMORY_H