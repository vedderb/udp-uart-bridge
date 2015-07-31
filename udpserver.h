/*
    Copyright 2012 Benjamin Vedder	benjamin@vedder.se

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include "serialport.h"
#include "packetinterface.h"

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = 0);
    bool startServer(const QString &serialPort, int baudrate, quint16 udpPort);
    QString getSerialPortPath();
    quint16 getUdpPort();

signals:
    
public slots:
    void readPendingDatagrams();
    void serialDataAvilable();
    void serialPortError(int e);
    void timerSlot();
    void processPacket(QByteArray data);
    void packetDataToSend(QByteArray &data);

private:
    QUdpSocket *mUdpSocket;
    SerialPort *mSerialPort;
    PacketInterface *mPacketInterface;
    QTimer *mTimer;
    QHostAddress mLastHostAddress;
    QString mPortPath;
    int mBaudrate;
    quint16 mUdpPort;
    int mReconnectTimer;

};

#endif // UDPSERVER_H
