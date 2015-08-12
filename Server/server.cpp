#include <QtWidgets>
#include <QtNetwork>
#include <QDebug>
#include <QHostInfo>
#include <QDate>
#include <QTime>
#include <stdlib.h>
#include "server.h"
#include "treatmentplan.h"


Server::Server(QWidget *parent) : QDialog(parent)
{
// Initialize the structure variable which stores the treatment plan
    QList<double> x, y, z;
    x << 11 << 12 << 13;
    y << 21 << 22 << 23;
    z << 31 << 32 << 33;
    m_plan.coordinate = {3, x, y, z};
    m_plan.parameter = {5, 10 , 0.5, 20};

    m_hashPlan[1] = m_plan;

// UI setup
    u_sendPlanButton = new QPushButton(tr("Send plan"));
    u_sendPlanButton->setAutoDefault(false);
    u_sendCommandButton = new QPushButton(tr("Send command"));
    u_sendCommandButton->setAutoDefault(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(u_sendPlanButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(u_sendCommandButton);
    buttonLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Server"));

// Variables initialization and build connections
    m_sendTimeNum = 1;
    m_totalBytes = 0;
    m_tcpServer = new QTcpSocket(this);

    connect(m_tcpServer, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    connect(u_sendPlanButton, SIGNAL(clicked()), this, SLOT(sendPlan()));
    //connect(u_sendPlanButton, SIGNAL(clicked()), this, SLOT(sendPlanHash()));
    connect(u_sendCommandButton, SIGNAL(clicked()), this, SLOT(sendCommandStart()));
}


Server::~Server()
{
}


// Connect to client and start session
void Server::connectServer()
{
    // Initialize the sent data as 0
    QHostAddress ipAddress("172.168.0.116");
    m_tcpServer->connectToHost(ipAddress, 6666);    // Connect
}


// Useless, delete later
void Server::startTransfer()
{
//    m_totalBytes = m_dataValue.size();    // Total bytes of the file
//    qDebug() << "m_dataValue.size():" << m_totalBytes;

//    QDataStream m_sendOut(&m_outBlock, QIODevice::WriteOnly);
//    m_sendOut.setVersion(QDataStream::Qt_4_6);
//    m_sendOut << qint64(0) << qint64(0) << m_dataKey;
//    qDebug() << "m_dataKey:" << m_dataKey;
//// Write total bytes space, dataKey space, dataKey in order
//    m_totalBytes += m_outBlock.size();
//// m_totalBytes is the bytes information of dataKey space and the actual bytes of the data
//    m_sendOut.device()->seek(0);
//    m_sendOut << m_totalBytes << qint64((m_outBlock.size() - sizeof(qint64)*2));
//    qDebug() << "m_totalBytes:" << m_totalBytes;
//// Return the start of m_outBlock, replace two qint64 spaces by actual length information

//    m_sendOut << m_dataValue;
//    qDebug() << "m_dataValue:" << m_dataValue;

//    m_tcpServer->write(m_outBlock);
//    m_outBlock.resize(0);

//    m_tcpServer->close();
//    qDebug() << "Send finished";
}


// Display error report
void Server::displayError(QAbstractSocket::SocketError)
{
    qDebug() << m_tcpServer->errorString();
    m_tcpServer->close();
}


// Send treatment plan
void Server::sendPlan()
{
    QHostAddress ipAddress("172.168.0.116");
    m_tcpServer->connectToHost(ipAddress, 6666);

    QDataStream m_sendOut(&m_outBlock, QIODevice::WriteOnly);
    m_sendOut.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Sending plan...";
    QString sendInfo = writeSendInfo();

    // Write data
    connect(m_tcpServer, SIGNAL(bytesWritten(qint64)), this, SLOT(writtenBytes(qint64)));    // Check if the data has been all well written

    m_sendOut << qint64(0)
              << qint64(0)
              << m_plan.coordinate.spotNum
              << m_plan.coordinate.spotPosX
              << m_plan.coordinate.spotPosY
              << m_plan.coordinate.spotPosZ
              << m_plan.parameter.coolingTime
              << m_plan.parameter.dutyCycle
              << m_plan.parameter.sonicationPeriod
              << m_plan.parameter.sonicationTime
              << sendInfo;

    qDebug() << "sendInfo:" << sendInfo;

    m_totalBytes = m_outBlock.size();
    qDebug() << "m_totalBytes:" << m_totalBytes;
    m_sendOut.device()->seek(0);
    m_sendOut << qint64(2) << m_totalBytes;    // Find the head of array and write the haed information

    m_tcpServer->write(m_outBlock);

    m_sendTimeNum += 1;
    m_writtenBytes = 0;
    m_outBlock.resize(0);

    qDebug() << "Send finished";

    // Read send-back data
    connect(m_tcpServer, SIGNAL(readyRead()), this, SLOT(readSendBack()));

    m_tcpServer->waitForReadyRead(3000);

    qDebug() << m_receivedInfo;

    // Check the consistency of the send-back data
    if(m_receivedInfo == sendInfo)
    {
        qDebug() << "Send-back checked.";
        m_receivedInfo = "";
    }
    else
    {
        emit error_sendBackCheck();
        qDebug() << "Check failed ! emit error signal...";
    }
}


// 1 means command, 2 means treatment plan, 3 means connection check
void Server::sendHead(qint64 head)
{
    qDebug() << "Sending head...";

    // m_sendOut << head;
    qDebug() << "Head:" << head << "writing finished...";

}


// Send command start
void Server::sendCommandStart()
{
    QHostAddress ipAddress("172.168.0.116");
    m_tcpServer->connectToHost(ipAddress, 6666);

    QDataStream m_sendOut(&m_outBlock, QIODevice::WriteOnly);
    m_sendOut.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Sending command start...";

    m_sendOut << qint64(1);
    m_sendOut << qint64(1);

    m_tcpServer->write(m_outBlock);
    m_outBlock.resize(0);

    m_tcpServer->close();
    qDebug() << "Send finished";
}


// Send command stop
void Server::sendCommandStop()
{
    QHostAddress ipAddress("172.168.0.116");
    m_tcpServer->connectToHost(ipAddress, 6666);

    QDataStream m_sendOut(&m_outBlock, QIODevice::WriteOnly);
    m_sendOut.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Sending command start...";

    m_sendOut << qint64(1);
    m_sendOut << qint64(2);

    m_tcpServer->write(m_outBlock);
    m_outBlock.resize(0);

    m_tcpServer->close();
    qDebug() << "Send finished";
}


// Send command pause
void Server::sendCommandPause()
{
    QHostAddress ipAddress("172.168.0.116");
    m_tcpServer->connectToHost(ipAddress, 6666);

    QDataStream m_sendOut(&m_outBlock, QIODevice::WriteOnly);
    m_sendOut.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Sending command start...";

    m_sendOut << qint64(1);
    m_sendOut << qint64(3);

    m_tcpServer->write(m_outBlock);
    m_outBlock.resize(0);

    m_tcpServer->close();
    qDebug() << "Send finished";
}


// Send command resume
void Server::sendCommandResume()
{
    QHostAddress ipAddress("172.168.0.116");
    m_tcpServer->connectToHost(ipAddress, 6666);

    QDataStream m_sendOut(&m_outBlock, QIODevice::WriteOnly);
    m_sendOut.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Sending command start...";

    m_sendOut << qint64(1);
    m_sendOut << qint64(4);

    m_tcpServer->write(m_outBlock);
    m_outBlock.resize(0);

    m_tcpServer->close();
    qDebug() << "Send finished";
}


// Useless, delete later
void Server::sendPlanHash()
{
    QHostAddress ipAddress("172.168.0.116");
    m_tcpServer->connectToHost(ipAddress, 6666);

    QDataStream m_sendOut(&m_outBlock, QIODevice::WriteOnly);
    m_sendOut.setVersion(QDataStream::Qt_4_6);

    qDebug() << "Sending plan...";

    m_sendOut << qint64(0)
              << qint64(0);
              //<< m_hashPlan;
    m_totalBytes = sizeof(m_hashPlan);
    qDebug() << "m_totalBytes:" << m_totalBytes;
    m_sendOut.device()->seek(0);
    m_sendOut << qint64(2) << m_totalBytes;

    m_tcpServer->write(m_outBlock);
    m_outBlock.resize(0);

    m_tcpServer->close();
    qDebug() << "Send finished";
}


// Generate the log information of treatment plan sending
QString Server::writeSendInfo()
{
    QString sendInfo;
    QString date = m_date.currentDate().toString();
    QString time = m_time.currentTime().toString();
    QString server = "ServerName";
    QString client = "ClientName";
    QString timeNum = "Time: " + QString::number(m_sendTimeNum, 10);
    sendInfo = "From: " + server + ", " + "To: " + client + ", " + timeNum + ", " + date + ", " + time;
    return sendInfo;
}


// Read send-back information
void Server::readSendBack()
{
    qDebug() << "readyRead...";
    QDataStream in(m_tcpServer);
    in.setVersion(QDataStream::Qt_4_6);

    qDebug() << "bytesAvail:" << m_tcpServer->bytesAvailable();
    in >> m_receivedInfo;

    m_inBlock.resize(0);

    qDebug() << "m_receivedInfo:" << m_receivedInfo;

    m_tcpServer->close();
}


// Slot function to capture written bytes of socket
void Server::writtenBytes(qint64 bytesWrite)
{
    qDebug() << "Bytes Written:" << bytesWrite;

    m_writtenBytes = bytesWrite;

    if (m_writtenBytes != m_totalBytes) {emit error_sendCheck(); qDebug() << "Send error !";}
    else {qDebug()<< "Send checked !";}
}
