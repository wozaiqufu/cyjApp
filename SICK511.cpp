#include <QtCore>
#include "SICK511.h"
#include <QDebug>


SICK511::SICK511(QObject* parent)
: QObject(parent),
m_isOn(false),
m_numberDataOneSocket(0),
m_courseAngle(0),
m_lateralOffset(0)
{
	connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(slot_on_readMessage()));
	connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_on_error(QAbstractSocket::SocketError)));
}

SICK511::~SICK511()
{
	if (m_isOn)
	{
		continuousStop();
	}
	m_tcpSocket.close();
}

bool SICK511::init(const QString name, const QString ip, const int port)
{
	m_name = name;
	m_ip = ip;
	m_port = port;
	m_tcpSocket.abort();
	m_tcpSocket.connectToHost(m_ip, m_port);
	if (m_tcpSocket.waitForConnected(m_milSecondsWait))
	{
		emit sig_statusTable(m_name + "init succeeded");
		m_isOn = true;
		return true;
	}
	else return false;
}

bool SICK511::isOn()
{
	return m_isOn == true;
}

void SICK511::continuousStart()
{
	QString qstr("\x2sEN LMDscandata 1\x3");
	qDebug() << "request at:" << QTime::currentTime();
	m_dataRecieved.clear();
	m_tcpSocket.write(qstr.toLocal8Bit());
}

void SICK511::continuousStop()
{
	QString qstr("\x2sEN LMDscandata 0\x3");
	qDebug() << "request at:" << QTime::currentTime();
	m_dataRecieved.clear();
	m_tcpSocket.write(qstr.toLocal8Bit());
}

void SICK511::slot_on_readMessage(){
	//QDataStream in(m_tcpSocket);
	//qDebug()<<"slot_on_readMessage is triigerred!";
	m_dataRecieved.clear();
	m_dataRecieved = m_tcpSocket.readAll();
	//one data has 2 bytes
	//qDebug()<<"m_dataRecieved:"<<m_dataRecieved<<"at"<<QTime::currentTime();
	extractDISTData();
	extractRSSIData();
	emit sigUpdateCourseAngle(courseAngle());
	emit sigUpdateLateralOffset(lateralOffset());
}
bool SICK511::isDataLoaded()
{
	return (m_DISTdata.size() > 179 && m_RSSIdata.size() > 179);
}


void SICK511::extractDISTData()
{
    if(m_dataRecieved.isEmpty()){
        return;
    }
    //if using the permanent mode,the first diagram is the confirmation,not the DATA
	if (!m_dataRecieved.contains("00000000")){
        return;
    }
    QString eightZeros("00000000");
    int zerosStartPos = m_dataRecieved.indexOf(eightZeros);
    zerosStartPos = zerosStartPos + 9;//index of Starting Angle
    //crop Starting Angle
    int startAngle_End = m_dataRecieved.indexOf(" ",zerosStartPos);
    QByteArray startAngle = m_dataRecieved.mid(zerosStartPos,startAngle_End-zerosStartPos);
    //qDebug()<<"startAngle:"<<startAngle;
    //crop Angular step width
    int AngularStep_Begin = startAngle_End + 1;
    QByteArray angularStep = m_dataRecieved.mid(AngularStep_Begin,m_dataRecieved.indexOf(" ",AngularStep_Begin)-AngularStep_Begin);
	//qDebug()<<"angular step:"<<angularStep;
    //crop Number Data
    int NumberData_Begin = m_dataRecieved.indexOf(" ",AngularStep_Begin) + 1;
    QByteArray numberData = m_dataRecieved.mid(NumberData_Begin,m_dataRecieved.indexOf(" ",NumberData_Begin)-NumberData_Begin);
    //qDebug()<<"number data in Hex:"<<numberData;
    //crop Data:in order to completely crop data,we need to transform Hex to Decimal
    bool ok;
    m_numberDataOneSocket = numberData.toInt(&ok,16);
	m_angleResolution = 180.0 / (m_numberDataOneSocket - 1);
	//qDebug() << "m_angleResolution is:" << m_angleResolution;
    int data_index = m_dataRecieved.indexOf(" ",NumberData_Begin) +1;
     //qDebug()<<"number data in decimal :"<<m_numberDataOneSocket;
    //crop data begins
     m_DISTdata.clear();//CAUTION!m_data_forward vector must be cleaned before new data is pushed!
     for(int i=0;i<m_numberDataOneSocket;i++)
     {
         QByteArray iData = m_dataRecieved.mid(data_index,m_dataRecieved.indexOf(" ",data_index)-data_index);
         bool ok;
         int iData_Dec = iData.toInt(&ok,16);
         //if 40000000,returned data*2 is the real value in decimal
         iData_Dec = 2*iData_Dec;
         if(ok){
             m_DISTdata.push_back(iData_Dec);
             //qDebug()<<"the "<<i<<" th data is: "<<iData_Dec;
         }
         data_index = m_dataRecieved.indexOf(" ",data_index) + 1;
     }
     //signal to surface obj
     emit sigUpdateDIST(m_DISTdata);
	 //qDebug() << "dist of " + m_name + ":" << m_DISTdata;
     //qDebug()<<"size of m_data_forward:"<<m_data_forward.size();
}

void SICK511::extractRSSIData()
{
    /***********************forward************************************/
    if(m_dataRecieved.isEmpty()){
        return;
    }
    //if using the permanent mode,the first diagram is the confirmation,not the DATA
    if(!m_dataRecieved.contains("00000000")){
        return;
    }
    QString eightZeros("00000000");
    int zerosStartPos = m_dataRecieved.lastIndexOf(eightZeros);
    zerosStartPos = zerosStartPos + 9;//index of Starting Angle
    //crop Starting Angle
    int startAngle_End = m_dataRecieved.indexOf(" ",zerosStartPos);
    QByteArray startAngle = m_dataRecieved.mid(zerosStartPos,startAngle_End-zerosStartPos);
    //qDebug()<<"startAngle:"<<startAngle;
    //crop Angular step width
    int AngularStep_Begin = startAngle_End + 1;
    QByteArray angularStep = m_dataRecieved.mid(AngularStep_Begin,m_dataRecieved.indexOf(" ",AngularStep_Begin)-AngularStep_Begin);
    //qDebug()<<"angular step:"<<angularStep;
    //crop Number Data
    int NumberData_Begin = m_dataRecieved.indexOf(" ",AngularStep_Begin) + 1;
    QByteArray numberData = m_dataRecieved.mid(NumberData_Begin,m_dataRecieved.indexOf(" ",NumberData_Begin)-NumberData_Begin);
    //qDebug()<<"number data in Hex:"<<numberData;
    //crop Data:in order to completely crop data,we need to transform Hex to Decimal
    bool ok;
    m_numberDataOneSocket = numberData.toInt(&ok,16);
    int data_index = m_dataRecieved.indexOf(" ",NumberData_Begin) +1;
     //qDebug()<<"number data in decimal :"<<m_numberDataOneSocket;
    //crop data begins
     m_RSSIdata.clear();//CAUTION!m_data_forward vector must be cleaned before new data is pushed!
     for(int i=0;i<m_numberDataOneSocket;i++)
     {
         QByteArray iData = m_dataRecieved.mid(data_index,m_dataRecieved.indexOf(" ",data_index)-data_index);
         bool ok;
         int iData_Dec = iData.toInt(&ok,16);
         //if 40000000,returned data*2 is the real value in decimal
        // iData_Dec = 2*iData_Dec;
         if(ok){
             m_RSSIdata.push_back(iData_Dec);
             //qDebug()<<"the "<<i<<" th data is: "<<iData_Dec;
         }
         data_index = m_dataRecieved.indexOf(" ",data_index) + 1;
     }
     //signal to surface obj
     emit sigUpdataRSSI(m_RSSIdata);
     //qDebug()<<"\n"<<"m_RSSIdata_forward in decimal:"<<m_RSSIdata_forward;
     //qDebug()<<"size of m_RSSIdata_forward:"<<m_RSSIdata_forward.size();
}

int SICK511::angle2index(const int angle)
{
	if (angle > 180 || angle < 0)
		return -1;
	return angle / m_angleResolution;
}

//calculate course angle
int SICK511::courseAngle()
{
	if (!isDataLoaded())
	{
		return m_courseAngle;
	}
    //qDebug()<<"courseAngle is triggered!";
    //qDebug()<<"m_currentDirection:"<<m_currentDirection;
    int courseAngle_right = 0;
    int courseAngle_left = 0;
	int courseAngle = 0;
	if ((m_DISTdata.size() == m_numberDataOneSocket)&&(m_angleResolution>0.20))
	{
		//right side
		double sumCourse = 0;
		int validCount = 0;
		qDebug() << "total index is:" << m_angleScale / m_angleResolution;
		for (int i = 0; i<m_angleScale/m_angleResolution; i++)
		{
			//l1!=0&&l2!=0
			if ((m_DISTdata.at(angle2index(m_angleStart) + i) == 0) || (m_DISTdata.at(angle2index(m_angleStart + m_anglel1l2) + i) == 0))
			{
				continue;
			}
			//qDebug()<<"Valid count======:"<<validCount;
			//beta
			double l1 = m_DISTdata.at(angle2index(m_angleStart) + i);
			//qDebug() << "right l1 data index:" << angle2index(m_angleStart) + i;
			double l2 = m_DISTdata.at(angle2index(m_angleStart + m_anglel1l2) + i);
			//qDebug() << "right l2 data index:" << angle2index(m_angleStart + m_anglel1l2) + i;
			double l3 = sqrt(pow(l1, 2)
				+ pow(l2, 2)
				- 2 * l1*l2*cos(m_anglel1l2*m_Angle_degree2Radian));
			//qDebug()<<"l3:"<<l3;
			double cos_beta = (pow(l1, 2) + pow(l3, 2) - pow(l2, 2)) / (2 * l1*l3);
			//qDebug()<<"cos_beta"<<cos_beta;
			double icourse = 90 + (m_angleStart + i) - acos(cos_beta) / m_Angle_degree2Radian;
			//qDebug()<<"icourse:"<<icourse;
			//sum of course
			sumCourse += icourse;
			validCount++;
		}
		if (validCount == 0)
		{
			qDebug() << "course angle all l1 l2 are invalid!";
		}
		else
		{
			courseAngle_right = sumCourse / validCount;
		}

		//left side
		sumCourse = 0;
		validCount = 0;
		for (int i = 0; i<m_angleScale/m_angleResolution; i++)
		{
			//l1!=0&&l2!=0
			if ((m_DISTdata.at(angle2index(180 - m_angleStart) - i) == 0) || (m_DISTdata.at(angle2index(180 - m_angleStart - m_anglel1l2) - i) == 0))
			{
				continue;
			}
			//qDebug()<<"Valid count======:"<<validCount;
			//beta
			double l1 = m_DISTdata.at(angle2index(180 - m_angleStart - i));
			//qDebug() << "left l1 data index:" << angle2index(180 - m_angleStart - i);
			double l2 = m_DISTdata.at(angle2index(180 - m_angleStart - m_anglel1l2 - i));
			//qDebug() << "left l2 data index:" << angle2index(180 - m_angleStart - m_anglel1l2 - i);
			double l3 = sqrt(pow(l1, 2) + pow(l2, 2) - 2 * l1*l2*cos(m_anglel1l2*m_Angle_degree2Radian));
			//qDebug()<<"l1:"<<l1;
			//qDebug()<<"l2:"<<l2;
			//qDebug()<<"l3:"<<l3;
			double cos_beta = (pow(l1, 2) + pow(l3, 2) - pow(l2, 2)) / (2 * l1*l3);
			//qDebug()<<"beta:"<<acos(cos_beta)/m_Angle_degree2Radian;
			//sum of beta
			double icourse = acos(cos_beta) / m_Angle_degree2Radian - 90 - i;
			//qDebug()<<"icourse"<<icourse;
			sumCourse += icourse;
			validCount++;
		}
		if (validCount == 0)
		{
			qDebug() << "course angle all l1 l2 are invalid!";
		}
		else
		{
			courseAngle_left = sumCourse / validCount;
		}

		courseAngle = (courseAngle_left + courseAngle_right) / 2;
		m_courseAngle = courseAngle;
	}
	//return (courseAngle_left+courseAngle_right)/2;
	//qDebug()<<"=====================================================>";
	//return courseAngle_right;//is ok
	qDebug() << "course angle is:" << m_courseAngle;
	return m_courseAngle;
}

//calculate lateral offset use both Caution:if return 2000,no available data!
int SICK511::lateralOffset()
{
	if (!isDataLoaded())
	{
		return m_lateralOffset;
	}
    qDebug()<<"courseAngle is triggered!";
     //qDebug()<<"m_currentDirection:"<<m_currentDirection;
    //qDebug()<<"m_data_forward"<<m_data_forward;
    int lateral = 1;
	if ((m_DISTdata.size() == m_numberDataOneSocket) && (m_angleResolution>0.20))
	{
		double sumH1 = 0;
		double sumH2 = 0;
		double H1 = 0;
		double H2 = 0;
		int validCount = 0;
		for (int i = 0; i < m_angleScale / m_angleResolution; i++)
		{
			//right side h1
			double l = m_DISTdata.at(i + angle2index(m_angleStart));
			if (l == 0)
			{
				continue;
			}
			double h1 = l*cos(m_Angle_degree2Radian*(m_courseAngle - m_angleStart - angle2index(i)));
			//                qDebug()<<"l"<<l;
			//                qDebug()<<"valid:"<<validCount;
			//                qDebug()<<"h1:"<<h1;
			sumH1 += h1;
			validCount++;
		}
		if (validCount == 0)
		{
			qDebug() << "no data for H1";
			return 2000;
		}
		H1 = sumH1 / validCount;
		//            qDebug()<<"H1"<<H1;
		validCount = 0;
		for (int i = 0; i < m_angleScale/m_angleResolution; i++)
		{
			//left side h2
			double l = m_DISTdata.at(angle2index(180 - m_angleStart) - i);
			if (l == 0)
			{
				continue;
			}
			double h2 = l*cos(m_Angle_degree2Radian*(m_courseAngle + m_angleStart + angle2index(i)));
			sumH2 += h2;
			validCount++;
		}
		if (validCount == 0)
		{
			qDebug() << "no data for H1";
			return 2000;
		}
		H2 = sumH2 / validCount;

		lateral = (H2 - H1) / 2;
		m_lateralOffset = lateral;
		return m_lateralOffset;
	}
	else return m_lateralOffset;
}

void SICK511::slot_on_error(QAbstractSocket::SocketError)
{
     emit sig_statusTable("forward error:");
     qDebug()<<"backward error:"<<m_tcpSocket.errorString();
	 m_isOn = false;
}

QString SICK511::name() const
{
	return m_name;
}
QString SICK511::ip() const
{
	return m_ip;
}
int SICK511::port() const
{
	return m_port;
}
