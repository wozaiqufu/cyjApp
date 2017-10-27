#include "autoalgorithm.h"
#include <QtCore>
#include <QDebug>

autoAlgorithm::autoAlgorithm(QObject *parent) : QObject(parent),
  m_isAuto(true),
  m_stage(Auto),
  m_type(PID),
  m_beaconRSSIThreshold(200),
  m_left(0),
  m_right(0),
  m_accelerator(0),
  m_deaccelerator(0),
  m_mile(0)
{
    m_pathFile.setFileName("path.txt");
    m_beaconFile.setFileName("beacon.txt");
}

int autoAlgorithm::getBeaconIndex() const
{
    return 0;
}

bool autoAlgorithm::initWriting(const QString fileName)
{
    if(fileName=="beacon.txt")
    {
        if(!m_beaconFile.open(QIODevice::Text|QIODevice::WriteOnly))
        {
            emit sig_statusTable("can not open Beacon file for writing!");
            return false;
        }
        m_beaconTextStream.setDevice(&m_beaconFile);
        return true;
    }
    else if(fileName=="path.txt")
    {
        if (!m_pathFile.open(QIODevice::Text|QIODevice::WriteOnly))
        {
            emit sig_statusTable("can not open path file for writing!");
            return false;
        }
        m_pathTextStream.setDevice(&m_pathFile);
        return true;
    }
    else
        return false;
}

bool autoAlgorithm::saveData(QVector<int> vec,const QString fileName)
{
    if(fileName=="path.txt")
    {
        int numEachLine = vec.at(0);
        if(numEachLine!=(vec.size()-1))
        {
            emit sig_statusTable("saveData to path.txt:vector Error!");
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
        //m_textStream<<"\n";
         m_pathTextStream<<endl;
        return true;
    }
    else if(fileName=="beacon.txt")
    {
        int numEachLine = vec.at(0);
        if(numEachLine!=(vec.size()-1))
        {
            emit sig_statusTable("saveData to beacon.txt:vector Error!");
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
        //m_textStream<<"\n";
         m_beaconTextStream<<endl;
        return true;
    }
    else
        return false;
}

bool autoAlgorithm::closeFile(const QString fileName)
{
    if(fileName=="path.txt")
    {
        m_pathFile.flush();
        m_pathFile.close();
        return true;
    }
    else if(fileName=="beacon.txt")
    {
        m_beaconFile.flush();
        m_beaconFile.close();
        return true;
    }
    else
        return false;
}

bool autoAlgorithm::initReading(const QString fileName)
{

    if(fileName=="path.txt")
    {
        if(!m_pathFile.open(QIODevice::ReadOnly))
        {
            emit sig_statusTable("can not open file for reading!");
            return false;
        }
        m_pathTextStream.setDevice(&m_pathFile);
        return true;
    }
    else if(fileName=="beacon.txt")
    {
        if(!m_beaconFile.open(QIODevice::ReadOnly))
        {
            emit sig_statusTable("can not open file for reading!");
            return false;
        }
        m_beaconTextStream.setDevice(&m_beaconFile);
        return true;
    }
    else
        return false;
}

//loadData:beacon.txt and path.txt into
bool autoAlgorithm::loadData()
{
    initReading("path.txt");
    initReading("beacon.txt");
    while(!m_pathTextStream.atEnd())
    {
        controlCommand contr;
        int disp = 0;
        int index = 0;
        bool ok;
        QByteArray line = m_pathFile.readLine();
        qDebug()<<"line:"<<line;
        QByteArray dataNum = line.mid(index,line.indexOf("#",index)-index);
        emit sig_statusTable("data num each line is:"+QString(dataNum));
        index = line.indexOf("#",index) + 1;
        int iDataNum = dataNum.toInt(&ok,10);
        if(ok!=true)
        {
            emit sig_statusTable("loadData:dataNum Error!");
            return false;
        }
        for(int i=0;i<iDataNum;i++)
        {
            QByteArray bData = line.mid(index,line.indexOf(",",index)-index);
            emit sig_statusTable(QString::number(i) +"th data is :"+QString(bData));
            int iData = bData.toInt(&ok,10);
            if(ok!=true)
            {
                emit sig_statusTable("loadData:iDataNum");
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
    m_pathFile.close();

    while(!m_beaconTextStream.atEnd())
    {
        Beacon beacon;
        int index = 0;
        bool ok;
        QByteArray line = m_beaconFile.readLine();
        qDebug()<<"beacon line:"<<line;
        QByteArray dataNum = line.mid(index,line.indexOf("#",index)-index);
        emit sig_statusTable("data num each line is:"+QString(dataNum));
        index = line.indexOf("#",index) + 1;
        int iDataNum = dataNum.toInt(&ok,10);
        if(ok!=true)
        {
            emit sig_statusTable("loadData:dataNum Error!");
            return false;
        }
        for(int i=0;i<iDataNum;i++)
        {
            QByteArray bData = line.mid(index,line.indexOf(",",index)-index);
            emit sig_statusTable(QString::number(i) +"th data is :"+QString(bData));
            int iData = bData.toInt(&ok,10);
            if(ok!=true)
            {
                emit sig_statusTable("loadData:iDataNum");
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
        for(int i=0;i<m_beaconAndMile.size();i++)
        {
             qDebug()<<"m_beaconAndMile left:"<<m_beaconAndMile.at(i).width_left;
             qDebug()<<"m_beaconAndMile right:"<<m_beaconAndMile.at(i).width_right;
             qDebug()<<"m_beaconAndMile mile:"<<m_beaconAndMile.at(i).mile;
        }
    }
    return false;
}

void autoAlgorithm::setAlgorithmType(const int type)
{
    switch(type)
    {
    case 0:
        m_type = PID;
        break;
    case 1:
        m_type = TrackMemory;
        break;
    default:
        break;
    }
}

void autoAlgorithm::update()
{

}

int autoAlgorithm::left() const
{
    if(m_left<=m_angleMax)
    {
        return m_left;
    }
    else
    {
        return m_angleMax;
    }
}

int autoAlgorithm::right() const
{
    if(m_right<=m_angleMax)
    {
        return m_right;
    }
    else
    {
        return m_angleMax;
    }
}

int autoAlgorithm::accelerator() const
{
    if(m_accelerator<=m_acceleratorMax)
    {
        return m_accelerator;
    }
    else
    {
        return m_acceleratorMax;
    }
}

int autoAlgorithm::deaccelerator() const
{
    if(m_deaccelerator<=m_acceleratorMax)
    {
        return m_deaccelerator;
    }
    else
    {
        return m_acceleratorMax;
    }
}

void autoAlgorithm::slot_on_updateControlMode(bool isAuto)
{
    m_isAuto = isAuto;
}

void autoAlgorithm::slot_on_updateSICKDIS(QVector<int> vec)
{
//    if(m_isAuto)
//    {
//        m_SICKdata = vec;
//    }
//    else
//    {
//        return;
//    }
/*************************************************************
 * ******************for test only*****************************
 * ***********************************************************/
   if(vec.size()!=181)
   {
       return;
   }
   else
   {
       m_SICKdata = vec;
   }
}

void autoAlgorithm::slot_on_updateSICKRSSI(QVector<int> vec)
{
    //qDebug()<<"slot_on_updateSICKRSSI";
    if(vec.size()!=181)
    {
        return;
    }
    m_SICKRSSI = vec;
    qDebug()<<"the value of RSSI is:"<<m_SICKRSSI;
    qDebug()<<"the size of RSSI is:"<<m_SICKRSSI.size();
    qDebug()<<"beacon length are:"<<beaconLength(m_beaconRSSIThreshold);
}

bool autoAlgorithm::matchBeacon()
{
    //data base is in m_beaconAndMile

    return true;
}

QVector<int> autoAlgorithm::beaconLength(const int delta)
{
    QVector<int> binary_vec;
    for(int ix = 0;ix < m_SICKRSSI.size(); ++ix)
    {
        if(m_SICKRSSI[ix] > delta)
            binary_vec.push_back(1);
        else
            binary_vec.push_back(0);
    }
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
        //else if(RSSIX == 0 && binary_vec[ix] == 1 )
        //{
        //   pos01_binary_vec.push_back(ix - 1) ;
        //}
        else if(RSSIX == -1)
        {
            pos10_binary_vec.push_back(ix) ;
        }
        //qDebug()<<"pos01_binary_vec is :"<<pos01_binary_vec;
        //qDebug()<<"pos10_binary_vec is :"<<pos10_binary_vec;
    }

    //qDebug()<<"pos01_binary_vec is :"<<pos01_binary_vec;
    //qDebug()<<"the binary_vec.last() is :"<<binary_vec.last();

    if(binary_vec.last() == 1)
    {
        pos10_binary_vec.push_back(binary_vec.size()-1) ;
    }
    qDebug()<<"the pos of 01 in RSSI is :"<<pos01_binary_vec;
    qDebug()<<"the pos of 10 in RSSI is :"<<pos10_binary_vec;
    //angle = pos10_binary_vec - pos10_binary_vec ;
    m_beaconLength.clear();
    if(pos01_binary_vec.size() == pos10_binary_vec.size())
    {
        for(int ix = 0;ix < pos10_binary_vec.size(); ++ix)
        {
            //qDebug()<<"start Length";
            int temp1 = pos01_binary_vec.at(ix);
           // qDebug()<<"var"<<temp1;
            int dist1_beacon = m_SICKdata[temp1];
            //qDebug()<<"dist_beacon1 "<<dist_beacon1;
            int temp2 = pos10_binary_vec.at(ix);
            int dist2_beacon = m_SICKdata[temp2];
            double angle_beacon = (pos10_binary_vec.at(ix) - pos01_binary_vec.at(ix)) * m_Angle_degree2Radian;
            //qDebug()<<"dist1_beacon "<<dist1_beacon;
            //qDebug()<<"dist2_beacon "<<dist2_beacon;
            //qDebug()<<"angle_beacon "<<angle_beacon;
            //qDebug()<<"cos of angle_beacon "<<cos(angle_beacon);
            int temp3 = sqrt(pow(dist1_beacon,2) + pow(dist2_beacon,2)- 2*dist1_beacon*dist2_beacon*cos(angle_beacon));
            if(temp3 >0)
            {
                 m_beaconLength.push_back(temp3);
            }
        }
    }
    return m_beaconLength ;

}
