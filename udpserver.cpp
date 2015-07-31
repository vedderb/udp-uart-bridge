/*
    Copyright 2014 Benjamin Vedder	benjamin@vedder.se

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

#include "udpserver.h"
#include <QDebug>

UdpServer::UdpServer(QObject *parent) :
    QObject(parent)
{
    mUdpSocket = new QUdpSocket(this);
    mSerialPort = new SerialPort(this);
    mPacketInterface = new PacketInterface(this);
    mTimer = new QTimer(this);
    mTimer->setInterval(10);
    mTimer->start();

    mPortPath = "/dev/ttyO2";
    mUdpPort = 27800;
    mBaudrate = 115200;
    mReconnectTimer = 0;

    connect(mUdpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
    connect(mSerialPort, SIGNAL(serial_port_error(int)),
            this, SLOT(serialPortError(int)));
    connect(mSerialPort, SIGNAL(serial_data_available()),
            this, SLOT(serialDataAvilable()));
    connect(mPacketInterface, SIGNAL(packetReceived(QByteArray)),
            this, SLOT(processPacket(QByteArray)));
    connect(mPacketInterface, SIGNAL(dataToSend(QByteArray&)),
            this, SLOT(packetDataToSend(QByteArray&)));
    connect(mTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));
}

bool UdpServer::startServer(const QString &serialPort, int baudrate, quint16 udpPort)
{
    mPortPath = serialPort;
    mUdpPort = udpPort;
    mBaudrate = baudrate;

    if (mSerialPort->openPort(mPortPath, mBaudrate) < 0) {
        qWarning() << "Unable to open serial port.";
        return false;
    }

    return mUdpSocket->bind(QHostAddress::Any, mUdpPort);
}

void UdpServer::readPendingDatagrams()
{
    while (mUdpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(mUdpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        mUdpSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        mPacketInterface->sendPacket(datagram);

        mLastHostAddress = sender;
    }
}

void UdpServer::serialDataAvilable()
{
    while (mSerialPort->bytesAvailable() > 0) {
        QByteArray data = mSerialPort->readAll();
        mPacketInterface->processData(data);
    }
}

void UdpServer::serialPortError(int e)
{
    qWarning() << "Serial port error: " << e;
    mReconnectTimer = 50;
}

void UdpServer::timerSlot()
{
    mPacketInterface->timerSlot();

    if (mReconnectTimer) {
        mReconnectTimer--;
        if (!mReconnectTimer) {
            if (mSerialPort->openPort(mPortPath, mBaudrate) < 0) {
                qWarning() << "Unable to open serial port. Retrying...";
                mReconnectTimer = 50;
            }
        }
    }
}

void UdpServer::processPacket(QByteArray data)
{
    if (QString::compare(mLastHostAddress.toString(), "0.0.0.0") == 0) {
        qDebug() << "No send destination...";
        return;
    }

    mUdpSocket->writeDatagram(data, mLastHostAddress, mUdpPort + 1);
}

void UdpServer::packetDataToSend(QByteArray &data)
{
    if (mSerialPort->isOpen()) {
        mSerialPort->writeData(data.data(), data.size());
    }
}

QString UdpServer::getSerialPortPath()
{
    return mPortPath;
}

quint16 UdpServer::getUdpPort()
{
    return mUdpPort;
}
