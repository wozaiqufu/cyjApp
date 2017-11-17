#include "trackmemory.h"
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QMap>
#include <QList>
#include <math.h>

/*
 * TrackMemory implementation Class
 * */

typedef struct Beacon
{
    int width_left;
    int width_right;
    int mile;
}BEACON;

typedef struct controlCommand
{
    int acc;
    int left;
    int right;
}CONTROL;

class TrackMemoryImpl
{
public:
    TrackMemoryImpl();
    ~TrackMemoryImpl();
    void init(const int rssiThreshold,const int beaconLengthThr,const int accmax,const int accmin,const int angmax,const int angmin);
    QVector<int>  update(int mile, QVector<int> dist, QVector<int> rssi);
    bool          saveData(const QString fileName,QVector<int> vec);
	bool		  isBeaconLost(QVector<int> dist, QVector<int> rssi);
private:
     bool         loadData(const QString txtName);
     QVector<int> beaconLength(QVector<int> dist,QVector<int> rssi,const int delta);
     QVector<int> Pro_binary(QVector<int> vec) const;//for beaconLength use(111001111 filter 00)
     bool         matchBeacon(const QVector<int> &vec,const double threshold);//m_beaconIndex is updated!
     void         calibMile(const int mile);//using m_beaconIndex get mile in beacon.txt
     QVector<int> matchMile(const int mile);//using m_mile_calib generate acc left and right
     bool         closeAllFile();
private:
     QFile                      m_pathFile;
     QFile                      m_beaconFile;
     QFile                      m_beaconRawFile;
     QTextStream                m_pathTextStream;
     QTextStream                m_beaconTextStream;
     QTextStream                m_beaconRawTextStream;
     QList <int>                m_beacon;//beacon length actual
     QList <Beacon>             m_beaconAndMile;//key:left and right beacon width find:mile(calibrated)
     QMap <int,controlCommand>  m_trackMap;//memory data are loaded here:key:disp find:control command
     int                        m_beaconIndex;
     int                        m_mile;
     int                        m_mile_calib;
     int                        m_mile_deltaCalib;
     int                        m_rssiThreshold;
     double                     m_beaconLengthThreshold;
     int                        m_beaconMatchPre;
     int                        m_beaconMatchPost;
     int                        m_accmin;
     int                        m_accmax;
     int                        m_angmax;
     int                        m_angmin;
//     static const int           m_angleMax = 30;
//     static const double        m_Angle_degree2Radian = 0.0174532925;
//     static const int           m_acceleratorMax = 100;
     const int           m_angleMax = 30;
     const double        m_Angle_degree2Radian = 0.0174532925;
     const int           m_acceleratorMax = 100;
};

TrackMemory::TrackMemory()
{
    m_Impl = new TrackMemoryImpl();
}

void TrackMemory::init(const int rssiThreshold,const double beaconLengthThr,const int accmax,const int accmin,const int angmax,const int angmin)
{
    m_Impl->init(rssiThreshold,beaconLengthThr,accmax,accmin,angmax,angmin);
}

QVector<int> TrackMemory::update(int mile, QVector<int> dist, QVector<int> rssi)
{
    return m_Impl->update(mile,dist,rssi);
}

bool TrackMemory::saveData(const QString fileName, QVector<int> vec)
{
	return m_Impl->saveData(fileName, vec);
}
bool TrackMemory::isBeaconLost(QVector<int> dist, QVector<int> rssi)
{
	return m_Impl->isBeaconLost(dist,rssi);
}
TrackMemory::~TrackMemory()
{
   delete m_Impl;
}
/*******************************************************************************************************
 * *****************************************************************************************************
 * */
TrackMemoryImpl::TrackMemoryImpl()
    :m_beaconIndex(0),
      m_mile(0),
      m_mile_calib(0),
      m_mile_deltaCalib(0),
      m_rssiThreshold(245),
      m_beaconLengthThreshold(0.1),
      m_beaconMatchPre(0),
      m_beaconMatchPost(0),
      m_accmax(128),
      m_accmin(0),
      m_angmax(80),
      m_angmin(0)
{
	//file names
    m_pathFile.setFileName("path.txt");
    m_beaconFile.setFileName("beacon.txt");
    m_beaconRawFile.setFileName("beaconRaw.txt");
}

TrackMemoryImpl::~TrackMemoryImpl()
{

}

void TrackMemoryImpl::init(const int rssiThreshold,const int beaconLengthThr,const int accmax,const int accmin,const int angmax,const int angmin)
{
    if(rssiThreshold<0)
        return;
    else
        m_rssiThreshold = rssiThreshold;
    if(beaconLengthThr<0)
        return;
    else
        m_beaconLengthThreshold = beaconLengthThr;
    if(accmax<0)
        return;
    else
        m_accmax = accmax;
    if(accmin<0)
        return;
    else
        m_accmin = accmin;
    if(angmax<0)
        return;
    else
        m_angmax = angmax;
    if(angmin<0)
        return;
    else
        m_angmin = angmin;
	loadData();
}

QVector<int> TrackMemoryImpl::update(int mile, QVector<int> dist, QVector<int> rssi)
{
    if(matchBeacon(beaconLength(dist,rssi,m_rssiThreshold),m_beaconLengthThreshold))
    {
        m_beaconMatchPre = m_beaconMatchPost;
        m_beaconMatchPost = 1;
    }
    else
    {
        m_beaconMatchPre = m_beaconMatchPost;
        m_beaconMatchPost = 0;
    }
    switch(m_beaconMatchPost-m_beaconMatchPre)
    {
    case -1:
    {
        calibMile(mile);
        return matchMile(mile);
        break;
    }
    case 0:
    {
        return matchMile(mile);
        break;
    }
    case 1:
    {
        return matchMile(mile);
        break;
    }
	default:
	{
		return QVector<int> (0);
		break;
	}
    }
}

bool TrackMemoryImpl::isBeaconLost(QVector<int> dist, QVector<int> rssi)
{
	if (matchBeacon(beaconLength(dist, rssi, m_rssiThreshold), m_beaconLengthThreshold))
	{
		m_beaconMatchPre = m_beaconMatchPost;
		m_beaconMatchPost = 1;
	}
	else
	{
		m_beaconMatchPre = m_beaconMatchPost;
		m_beaconMatchPost = 0;
	}
	if ((m_beaconMatchPost - m_beaconMatchPre) == -1)
		return true;
	else return false;
}
bool TrackMemoryImpl::saveData(const QString fileName, QVector<int> vec)
{
    if(fileName==m_pathFile.fileName())
    {
        if(!m_pathFile.isOpen())
        {
            m_pathFile.open(QIODevice::Text|QIODevice::WriteOnly);
            m_pathTextStream.setDevice(&m_pathFile);
        }
        int numEachLine = vec.at(0);
        if(numEachLine!=(vec.size()-1))
        {
            qDebug()<<"saveData to " + fileName + ".txt:vector Error!";
            return false;
        }
        for(int i=0;i<vec.size();i++)
        {
            if(i==0)
            {
                 m_pathTextStream<<vec.at(i)<<"#";
            }
            else
            {
                 m_pathTextStream<<vec.at(i)<<",";
            }
        }
         m_pathTextStream<<endl;
         return true;
    }
    else if(fileName==m_beaconFile.fileName())
    {
        if(!m_beaconFile.isOpen())
        {
            m_beaconFile.open(QIODevice::Text|QIODevice::WriteOnly);
            m_beaconTextStream.setDevice(&m_beaconFile);
        }
        int numEachLine = vec.at(0);
        if(numEachLine!=(vec.size()-1))
        {
            qDebug()<<"saveData to " + fileName + ".txt:vector Error!";
            return false;
        }
        for(int i=0;i<vec.size();i++)
        {
            if(i==0)
            {
                 m_beaconTextStream<<vec.at(i)<<"#";
            }
            else
            {
                 m_beaconTextStream<<vec.at(i)<<",";
            }
        }
         m_beaconTextStream<<endl;
         return true;
    }
    else
        return false;
}

bool TrackMemoryImpl::loadData(const QString txtName)
{
    //load beacon data:beaconRaw.txt
    QFile file;
    file.setFileName("beaconRaw.txt");
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        while(!textStream.atEnd())
        {
            bool ok;
            QByteArray line = file.readLine();
            int data =  line.toInt(&ok,10);
            if(!ok)
            {
                qDebug()<<"loadBeaconData:data Error!";
                break;
            }
            m_beacon.append(data);
        }
    }
    //file.close();
    //load beacon and mile data:beacon.txt
    file.setFileName("beacon.txt");
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        while(!textStream.atEnd())
        {
            {
                Beacon beacon;
                int index = 0;
                bool ok;
                QByteArray line = file.readLine();
                qDebug()<<"beacon line:"<<line;
                QByteArray dataNum = line.mid(index,line.indexOf("#",index)-index);
                //emit sig_statusTable("data num each line is:"+QString(dataNum));
                index = line.indexOf("#",index) + 1;
                int iDataNum = dataNum.toInt(&ok,10);
                if(ok!=true)
                {
                    qDebug()<<"loadData:dataNum Error!";
                    return false;
                }
                for(int i=0;i<iDataNum;i++)
                {
                    QByteArray bData = line.mid(index,line.indexOf(",",index)-index);
                    int iData = bData.toInt(&ok,10);
                    if(ok!=true)
                    {
                        qDebug()<<"loadData:iDataNum";
                        return false;
                    }
                    switch(i)
                    {
                    case 0:
                        beacon.width_left = iData;
                        break;
                    case 1:
                        beacon.width_right = iData;
                        break;
                    case 2:
                        beacon.mile = iData;
                        break;
                    default:
                        break;
                    }
                    index = line.indexOf(",",index) + 1;
            }
                m_beaconAndMile.append(beacon);
            }
        }
        for(int i=0;i<m_beaconAndMile.size();i++)
        {
             qDebug()<<"m_beaconAndMile left:"<<m_beaconAndMile.at(i).width_left;
             qDebug()<<"m_beaconAndMile right:"<<m_beaconAndMile.at(i).width_right;
             qDebug()<<"m_beaconAndMile mile:"<<m_beaconAndMile.at(i).mile;
        }
    }
    //file.close();
    //load mile and control command data:path.txt
     file.setFileName("path.txt");
     if(file.open(QIODevice::ReadOnly))
     {
         QTextStream textStream(&file);
         while(!textStream.atEnd())
         {
             controlCommand contr;
             int disp = 0;
             int index = 0;
             bool ok;
             QByteArray line = file.readLine();
             qDebug()<<"line:"<<line;
             QByteArray dataNum = line.mid(index,line.indexOf("#",index)-index);
             index = line.indexOf("#",index) + 1;
             int iDataNum = dataNum.toInt(&ok,10);
             if(ok!=true)
             {
                 qDebug()<<"loadData:dataNum Error!";
                 return false;
             }
             for(int i=0;i<iDataNum;i++)
             {
                 QByteArray bData = line.mid(index,line.indexOf(",",index)-index);
                 int iData = bData.toInt(&ok,10);
                 if(ok!=true)
                 {
                     qDebug()<<"loadData:iDataNum";
                     return false;
                 }
                 switch(i)
                 {
                 case 0:
                     disp = iData;
                     break;
                 case 1:
                     contr.acc = iData;
                     break;
                 case 2:
                     contr.left = iData;
                 case 3:
                     contr.right = iData;
                     break;
                 default:
                     break;
                 }
                 m_trackMap.insert(disp,contr);
                 index = line.indexOf(",",index) + 1;
             }
         }
     }
     //file.close();
     return true;
}

QVector<int> TrackMemoryImpl::beaconLength(QVector<int> dist,QVector<int> rssi,const int delta)
{
    QVector<int> binary_vec;
    QVector<int> beaconLength;
    for(int ix = 0;ix < rssi.size(); ++ix)
    {
        if(rssi[ix] > delta)
            binary_vec.push_back(1);
        else
            binary_vec.push_back(0);
    }
    //qDebug()<<"before Pro binary_vec is :"<<binary_vec;
    binary_vec = Pro_binary(binary_vec);
    //qDebug()<<"after Pro binary_vec is :"<<binary_vec;
    //qDebug()<<"the dist is :"<<m_SICKdata;
    QVector<int> pos01_binary_vec ;//index of "01"
    QVector<int> pos10_binary_vec ;//index of "10"
    if(binary_vec.at(0) == 1)
    {
        pos01_binary_vec.push_back(0);
    }
    for(int ix = 0;ix < binary_vec.size() -1; ++ix)
    {
        int RSSIX = binary_vec.at(ix+1)- binary_vec.at(ix);
        if(RSSIX == 1)
        {
            pos01_binary_vec.push_back(ix + 1) ;
        }
        else if(RSSIX == -1)
        {
            pos10_binary_vec.push_back(ix) ;
        }
    }

    if(binary_vec.last() == 1)
    {
        pos10_binary_vec.push_back(binary_vec.size()-1) ;
    }
    qDebug()<<"the pos of 01 in RSSI is :"<<pos01_binary_vec;
    qDebug()<<"the pos of 10 in RSSI is :"<<pos10_binary_vec;
    //angle = pos10_binary_vec - pos10_binary_vec ;
    beaconLength.clear();
    if(pos01_binary_vec.size() == pos10_binary_vec.size())
    {
        int dist1_beacon = 0;
        int dist2_beacon = 0;
        for(int ix = 0;ix < pos10_binary_vec.size(); ++ix)
        {
            double angle_num = pos10_binary_vec.at(ix) - pos01_binary_vec.at(ix);
            if(angle_num > 2)
            //qDebug()<<"start Length";
            dist1_beacon = dist.at(pos01_binary_vec.at(ix));
            dist2_beacon = dist.at(pos10_binary_vec.at(ix));
            double angle_beacon = (pos10_binary_vec.at(ix) - pos01_binary_vec.at(ix)) * m_Angle_degree2Radian;
            qDebug()<<"dist1_beacon "<<dist1_beacon;
            qDebug()<<"dist2_beacon "<<dist2_beacon;
            qDebug()<<"angle_beacon "<<angle_beacon;
            //qDebug()<<"cos of angle_beacon "<<cos(angle_beacon);
            int temp3 = sqrt(pow(dist1_beacon,2) + pow(dist2_beacon,2)- 2*dist1_beacon*dist2_beacon*cos(angle_beacon));
            if(temp3 >0)
            {
                double angle_beacon = angle_num * m_Angle_degree2Radian * 0.5;
                int temp1 = pos01_binary_vec.at(ix);
                int dist1_beacon = dist[temp1];
                int temp2 = pos10_binary_vec.at(ix);
                int dist2_beacon = dist[temp2];
                int temp3 = sqrt(pow(dist1_beacon,2) + pow(dist2_beacon,2)- 2*dist1_beacon*dist2_beacon*cos(angle_beacon));
                qDebug()<<"dist1_beacon "<<dist1_beacon;
                qDebug()<<"dist2_beacon "<<dist2_beacon;
                qDebug()<<"angle_beacon "<<angle_beacon;
                beaconLength.push_back(temp3);
            }
            //qDebug()<<"cos of angle_beacon "<<cos(angle_beacon);
        }
    }
    return beaconLength;
}

QVector<int> TrackMemoryImpl::Pro_binary(QVector<int> vec) const
{
    QVector<int> Pro_vec = vec;
        QVector<int> expansion_vec ;
        QVector<int> corrosion_vec ;
        //expansion
        if(Pro_vec.at(0)+Pro_vec.at(1))
        {
            expansion_vec.push_back(1);
        }
        else
        {
            expansion_vec.push_back(0);
        }
        for(int ix = 1; ix < Pro_vec.size()-1; ++ix)
        {
            if(Pro_vec.at(ix-1) + Pro_vec.at(ix) + Pro_vec.at(ix+1))
            {
                expansion_vec.push_back(1);
            }
            else
            {
                expansion_vec.push_back(0);
            }
        }
        if(Pro_vec.last() + Pro_vec.at(Pro_vec.size()-2))
            {
                expansion_vec.push_back(1);
            }
            else
            {
                expansion_vec.push_back(0);
            }
            //corrosion
            corrosion_vec.push_back(expansion_vec.at(0) * expansion_vec.at(1));
            for(int ix = 1; ix < expansion_vec.size()-1; ++ix)
            {
                corrosion_vec.push_back(expansion_vec.at(ix-1) * expansion_vec.at(ix) * expansion_vec.at(ix+1));
            }
            corrosion_vec.push_back(expansion_vec.last() * expansion_vec.at(expansion_vec.size()-2));

            return corrosion_vec;
}

bool TrackMemoryImpl::matchBeacon(const QVector<int> &vec, const double threshold)
{
    int count_Length = 0;
    double relative_error = 0.0;
    for(int i = 0; i < m_beaconAndMile.size(); ++i)
    {
        for(int j = 0; j < vec.size(); ++j)
        {
            relative_error = static_cast<double>(abs((vec.at(j) - m_beaconAndMile.at(i).width_left))/m_beaconAndMile.at(i).width_left);
            if(relative_error <= threshold)
            {
                ++count_Length;
            }
        }
        if(count_Length == 2)
        {
            m_beaconIndex = i;
            return true;
        }
    }
    return false;
}

void TrackMemoryImpl::calibMile(const int mile)
{
    m_mile_deltaCalib = m_beaconAndMile.at(m_beaconIndex).mile - mile;
}

QVector<int> TrackMemoryImpl::matchMile(const int mile)
{
    QVector<int> vec;
    int key = 0;
    int calibMile = mile + m_mile_deltaCalib;
    key = m_trackMap.lowerBound(calibMile).key();
    if(m_trackMap.contains(key))
    {
        if(m_trackMap[key].acc>m_accmax)
            vec.push_back(m_accmax);
        else if(m_trackMap[key].acc<m_accmin)
            vec.push_back(m_accmin);
            else vec.push_back(m_trackMap[key].acc);

        if(m_trackMap[key].left>m_angmax)
            vec.push_back(m_angmax);
        else if(m_trackMap[key].left<m_angmin)
            vec.push_back(m_angmin);
            else vec.push_back(m_trackMap[key].left);

        if(m_trackMap[key].right>m_angmax)
            vec.push_back(m_angmax);
        else if(m_trackMap[key].right<m_angmin)
            vec.push_back(m_angmin);
            else vec.push_back(m_trackMap[key].right);
    }
    return vec;
}

bool TrackMemoryImpl::closeAllFile()
{
    if(m_pathFile.isOpen()) m_pathFile.close();
    if(m_beaconFile.isOpen()) m_beaconFile.close();
    if(m_beaconRawFile.isOpen()) m_beaconRawFile.close();
    return true;
}
