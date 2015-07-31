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

#include <QCoreApplication>
#include "udpserver.h"

void showHelp()
{
    qDebug() << "TODO: Write help text";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    UdpServer server;
    QString ttyPort = "/dev/ttyACM0";
    int baudrate = 115200;
    int udpPort = 27800;

    QStringList args = QCoreApplication::arguments();

    for (int i = 0;i < args.size();i++) {
        // Skip the program argument
        if (i == 0) {
            continue;
        }

        QString str = args.at(i).toLower();

        // Skip path argument
        if (i >= args.size() && args.size() >= 3) {
            break;
        }

        bool dash = str.startsWith("-") && !str.startsWith("--");
        bool found = false;

        if ((dash && str.contains('h')) || str == "--help") {
            showHelp();
            found = true;
            return 0;
        }

        if ((dash && str.contains('t')) || str == "--ttyport") {
            if ((i - 1) < args.size()) {
                i++;
                ttyPort = args.at(i);
                found = true;
            }
        }

        if ((dash && str.contains('b')) || str == "--baudrate") {
            if ((i - 1) < args.size()) {
                i++;
                bool ok;
                baudrate = args.at(i).toInt(&ok);
                found = ok;
            }
        }

        if ((dash && str.contains('u')) || str == "--udpport") {
            if ((i - 1) < args.size()) {
                i++;
                bool ok;
                udpPort = args.at(i).toInt(&ok);
                found = ok;
            }
        }

        if (!found) {
            if (dash) {
                qCritical() << "At least one of the flags is invalid:" << str;
            } else {
                qCritical() << "Invalid option:" << str;
            }

            showHelp();
            return 1;
        }
    }

    qDebug() << "Serial Port:" << ttyPort;
    qDebug() << "Baudrate:" << baudrate;
    qDebug() << "UDP Port:" << udpPort;

    if (server.startServer(ttyPort, baudrate, udpPort)) {
        qDebug() << "The server was successfully started.";
        qDebug() << "UDP Port:" << server.getUdpPort();
        qDebug() << "Serial Port:" << server.getSerialPortPath();
    } else {
        qCritical() << "Errors occured. Exiting...";
        QTimer::singleShot(500, &a, SLOT(quit()));
    }

    return a.exec();
}
