#include <QtWidgets>
#include <QtNetwork>
#include <QList>
#include <QDebug>
#include <QHostInfo>

#include "client.h"
#include "treatmentplan.h"


Client::Client(QWidget *parent) : QDialog(parent)
{
// UI setup
    u_connectButton = new QPushButton(tr("Connect"));
    u_connectButton->setAutoDefault(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(u_connectButton);
    buttonLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Client"));

// Build client object and the connections
    QDataStream in(m_tcpClientConnection);
    in.setVersion(QDataStream::Qt_4_6);

    m_totalBytes = 0;
    m_bytesReceived = 0;
    m_dataKeySize = 0;

    m_ipAddress = getLocalIP();
    qDebug() << "IP Address:" << m_ipAddress;

    connect(u_connectButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(&m_tcpClient, SIGNAL(newConnection()) ,
            this, SLOT(acceptConnection()));    // Send newConnection() signal when a new connection is detected
}

Client::~Client()
{
}


// Start to listen
void Client::start()
{
    m_bytesReceived = 0;

    QHostAddress ipAddress(m_ipAddress.toInt());
    if(!m_tcpClient.listen(ipAddress, 6666))
    {
        qDebug() << m_tcpClient.errorString();
        close();
        return;
    }
    qDebug() << "Listen OK";
}


// Accept new connection
void Client::acceptConnection()
{
    m_tcpClientConnection = m_tcpClient.nextPendingConnection();
    connect(m_tcpClientConnection, SIGNAL(readyRead()), this, SLOT(readHead()));
    connect(m_tcpClientConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    qDebug() << "AcceptConnection OK";
    // m_tcpClient.close();
}


// Receive data
void Client::updateClientProgress()
{
    QDataStream in(m_tcpClientConnection);
    in.setVersion(QDataStream::Qt_4_6);
    if(m_bytesReceived <= sizeof(qint64)*2)
    {
        qDebug() << "m_bytesReceived:" << m_bytesReceived;
        qDebug() << "bytesAvailable:" << m_tcpClientConnection->bytesAvailable()
                 << endl << "m_dataKeySize:" << m_dataKeySize;
        // If received data length is less than 16 bytes, then it has just started, save incoming head information
        if((m_tcpClientConnection->bytesAvailable() >= sizeof(qint64)*2) && (m_dataKeySize == 0))
        {
            // Receive the total data length and the length of dataKey
            in >> m_totalBytes >> m_dataKeySize;
            qDebug() << "m_totalBytes:" << m_totalBytes << endl << "m_dataKeySize:" << m_dataKeySize;
            m_bytesReceived += sizeof(qint64) * 2;
        }
        if((m_tcpClientConnection->bytesAvailable() >= m_dataKeySize) && (m_dataKeySize != 0))
        {
            // Receive the filename, and build the file
            in >> m_dataKey;
            qDebug() << "m_dataKey:" << m_dataKey;

            m_bytesReceived += m_dataKeySize;
        }
        else return;
    }

    if(m_bytesReceived < m_totalBytes)
    {
        // If received data length is less than total length, then write the file
        m_bytesReceived += m_tcpClientConnection->bytesAvailable();
        m_inBlock = m_tcpClientConnection->readAll();
        in >> m_dataValue;
        qDebug() << "m_dataValue" << m_dataValue;
        m_inBlock.resize(0);
    }

    if(m_bytesReceived == m_totalBytes)
    {
        // When receiving process is done
        m_tcpClientConnection->close();
        m_dataKeySize = 0;
    }
}


// Get the local IP address
QString Client::getLocalIP()
{
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address() &&
            (ipAddressesList.at(i).toString().indexOf("168") != (-1))) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddress;
}


// Error display
void Client::displayError(QAbstractSocket::SocketError)
{
    qDebug() << m_tcpClientConnection->errorString();
}


void Client::readHead()
{
    QDataStream in(m_tcpClientConnection);
    in.setVersion(QDataStream::Qt_4_6);

    qint64 head;
    qDebug() << "Reading head...";

    in >> head;
    if (head == 1)
    {
        receiveCommand();
    }
    if (head == 2)
    {
        receivePlan();
        //receivePlanHash();
    }
}


void Client::receivePlan()
{
    QDataStream in(m_tcpClientConnection);
    in.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Receiving plan...";
    QString sendInfo;

    in >> m_totalBytes
            >> m_plan.coordinate.spotNum
            >> m_plan.coordinate.spotPosX
            >> m_plan.coordinate.spotPosY
            >> m_plan.coordinate.spotPosZ
            >> m_plan.parameter.coolingTime
            >> m_plan.parameter.dutyCycle
            >> m_plan.parameter.sonicationPeriod
            >> m_plan.parameter.sonicationTime
            >> sendInfo;

    qDebug() << "m_totalBytes:" << m_totalBytes << endl

             << "spotNum:" << m_plan.coordinate.spotNum << endl
             << "spotPosX:" << m_plan.coordinate.spotPosX << endl
             << "spotPosY:" << m_plan.coordinate.spotPosY << endl
             << "spotPosZ:" << m_plan.coordinate.spotPosZ << endl
             << "coolingTime:" << m_plan.parameter.coolingTime << endl
             << "dutyCycle:" << m_plan.parameter.dutyCycle << endl
             << "sonicationPeriod:" << m_plan.parameter.sonicationPeriod << endl
             << "sonicationTime:" << m_plan.parameter.sonicationTime << endl
             << "sendInfo:" << sendInfo;

    m_inBlock.resize(0);
    qDebug() << "Receive plan finished.";

    QDataStream out(&m_outBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Write back...";

    out << sendInfo;

    connect(m_tcpClientConnection, SIGNAL(bytesWritten(qint64)), this, SLOT(bytes(qint64)));
    m_tcpClientConnection->write(m_outBlock);

    qDebug() << sendInfo;

    m_outBlock.resize(0);

    qDebug() << "Send back finished.";

    m_tcpClientConnection->close();
}


void Client::receivePlanHash()
{
    QDataStream in(m_tcpClientConnection);
    in.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Receiving plan...";

    in >> m_totalBytes;
            //>> m_hashPlan;

    m_plan = m_hashPlan[1];

    qDebug() << "m_totalBytes:" << m_totalBytes << endl

             << "spotNum:" << m_plan.coordinate.spotNum << endl
             << "spotPosX:" << m_plan.coordinate.spotPosX << endl
             << "spotPosY:" << m_plan.coordinate.spotPosY << endl
             << "spotPosZ:" << m_plan.coordinate.spotPosZ << endl
             << "coolingTime:" << m_plan.parameter.coolingTime << endl
             << "dutyCycle:" << m_plan.parameter.dutyCycle << endl
             << "sonicationPeriod:" << m_plan.parameter.sonicationPeriod << endl
             << "sonicationTime" << m_plan.parameter.sonicationTime;

    m_inBlock.resize(0);

    m_tcpClientConnection->close();
    qDebug() << "Receive plan finished.";
}


// 1 start, 2 stop, 3 pause, 4 resume
void Client::receiveCommand()
{
    QDataStream in(m_tcpClientConnection);
    in.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Receiving command...";

    in >> m_command;

    qDebug() << m_command;

    switch (m_command) {
    case 1:
        emit commandStart(); qDebug() << "Start";
        break;
    case 2:
        emit commandStop(); qDebug() << "Stop";
        break;
    case 3:
        emit commandPause(); qDebug() << "Pause";
        break;
    case 4:
        emit commandResume(); qDebug() << "Resume";
        break;
    default:
        break;
    }

//    if (m_command == "start"){emit commandStart(); qDebug() << "Start";}
//    if (m_command == "stop") {emit commandStop(); qDebug() << "Stop";}
//    if (m_command == "pause") {emit commandPause(); qDebug() << "Pause";}
//    if (m_command == "resume") {emit commandResume(); qDebug() << "Resume";}

    m_inBlock.resize(0);

    m_tcpClientConnection->close();
    qDebug() << "Receive command finished.";
}


void Client::bytes(qint64 byteswrite)
{
    qDebug() << "Bytes Written:" << byteswrite;
}
