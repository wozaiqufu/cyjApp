#ifndef SURFACECOMMUNICATION_H
#define SURFACECOMMUNICATION_H

#include <QObject>
#include <QUdpSocket>

class SurfaceCommunication : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceCommunication(QObject *parent = 0);
    void init();
private:
signals:

public slots:
    void slot_doWork();
    void slot_on_SICKdataUpdate(QVector<int> vec);
    void slot_on_mainwindowUpdate(QVector<int> vec);
    void readPendingDatagrams();
private:
    QUdpSocket m_UdpSocket_sender;
    QUdpSocket m_UdpSocket_receiver;
    QHostAddress m_hostAddr;
    QVector<int> m_SICKdata;
    QVector<int> m_mainwindowData;
};

#endif // SURFACECOMMUNICATION_H
