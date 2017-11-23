#include "autoalgorithm.h"
#include "pid.h"
#include "trackmemory.h"
#include <QtCore>
#include <QVector>
#include <QDebug>

autoAlgorithm::autoAlgorithm(QObject *parent) : QObject(parent),
  m_isAuto(true),
  m_stage(Auto),
  m_type(PIDType),
  m_mile_saved(0),
  m_mile_current(0)
{
}

void autoAlgorithm::update()
{
	switch (m_stage)
	{
	case autoAlgorithm::Teach:
		p_track->loadData("beaconRaw.txt");
		if (isCertainMileIncrement(m_mile_current, m_mileDelta))
		{
			p_track->saveData("path.txt", m_mile_acc_deacc_left_right);
			m_mile_current = m_mile_acc_deacc_left_right.at(0);
		}
		if (p_track->isBeaconLost(m_SICKdata, m_SICKRSSI))
		{
			QVector<int> vec;
			vec.push_back(p_track->currentBeacon());
			vec.push_back(p_track->currentBeacon());
			vec.push_back(m_mile_acc_deacc_left_right.at(0));
			p_track->saveData("beacon.txt", vec);
		}
		break;
	case autoAlgorithm::Auto:
		break;
	default:
		break;
	}
}
void autoAlgorithm::setStageType(const int type)
{
	switch (type)
	{
	case 0:
		m_stage = Teach;
		break;
	case 1:
		m_stage = Auto;
		break;
	default:
		break;
	}
}

void autoAlgorithm::setAlgorithmType(const int type)
{
    switch(type)
    {
    case 0:
        m_type = PIDType;
        break;
    case 1:
        m_type = TrackMemoryType;
        break;
    default:
        break;
    }
}

int autoAlgorithm::left() const
{
    return -1;
}

int autoAlgorithm::right() const
{
     return -1;
}

int autoAlgorithm::accelerator() const
{
     return -1;
}

int autoAlgorithm::deaccelerator() const
{
     return -1;
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
   if(vec.size()!=361)
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
    if(vec.size()!=361)
    {
        return;
    }
    m_SICKRSSI = vec;
    //qDebug()<<"the value of RSSI is:"<<m_SICKRSSI;
   // qDebug()<<"the size of RSSI is:"<<m_SICKRSSI.size();
    //qDebug()<<"beacon length are:"<<beaconLength(m_beaconRSSIThreshold);
}

void autoAlgorithm::slot_on_updateControlInfo(QVector<int> vec)
{
	m_mile_acc_deacc_left_right = vec;
	m_mile_current = vec.at(0);
}

bool autoAlgorithm::isCertainMileIncrement(const int mile, const int inc)
{
	//10% of m_mileDelta
	if ((mile - m_mile_saved) <= 0) return false;
	return (abs(mile - m_mile_saved - m_mileDelta) < 0.1*m_mileDelta);
}
