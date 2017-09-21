#ifndef SURFACECOMMUNICATION_H
#define SURFACECOMMUNICATION_H

#include <QObject>
#include <QUdpSocket>

class SurfaceCommunication : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceCommunication(QObject *parent = 0);

signals:

public slots:
private:
    QUdpSocket m_udpSocket;
};

#endif // SURFACECOMMUNICATION_H
