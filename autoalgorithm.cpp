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
        if (isCertainMileIncrement(m_mile_current, MILEDELTA))
		{
			p_track->saveData("path.txt", m_mile_acc_deacc_left_right);
			m_mile_current = m_mile_acc_deacc_left_right.at(0);
		}
        if (p_track->matchBeacon(m_beaconLength))
		{
			QVector<int> vec;
			vec.push_back(p_track->currentBeacon());
			vec.push_back(p_track->currentBeacon());
			vec.push_back(m_mile_acc_deacc_left_right.at(0));
			p_track->saveData("beacon.txt", vec);
		}
		break;
	case autoAlgorithm::Auto:
        p_track->loadData("path.txt");
        p_track->loadData("beacon.txt");
        //double elapsedTime = m_time.restart()/1000.0;
        //QVector<int> track = p_track->update(m_mile_current,m_beaconLength);
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

void autoAlgorithm::slot_on_updateBeaconLength(QVector<int> vec)
{
    m_beaconLength = vec;
}

void autoAlgorithm::slot_on_updateCourseAngle(int angle)
{
    m_courseAngle = angle;
}

void autoAlgorithm::slot_on_updateLateralOffset(int of)
{
    m_courseAngle = of;
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
    return (abs(mile - m_mile_saved - MILEDELTA) < 0.1*MILEDELTA);
}
