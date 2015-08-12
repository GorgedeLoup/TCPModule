#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QList>
#include <QDialog>
#include <QtNetwork>
#include <QHash>
#include <QDate>
#include <QTime>
#include <QDate>
#include "treatmentplan.h"

QT_BEGIN_NAMESPACE
class QPushButton;
class QByteArray;
class QTcpSocket;

class QPushButton;
QT_END_NAMESPACE


class Server : public QDialog
{
    Q_OBJECT

private:

    QTcpSocket *m_tcpServer;
    QByteArray m_outBlock;    // Data buffer
    QByteArray m_inBlock;    // Data buffer

    qint64 m_totalBytes;    // Total bytes to send for this send progress
    qint64 m_writtenBytes;

    QString m_ipAddress;    // The IPV4 address of current computer

    TreatmentPlan m_plan;

    QPushButton *u_sendPlanButton;
    QPushButton *u_sendCommandButton;

    QDate m_date;
    QTime m_time;
    int m_sendTimeNum;
    QString m_receivedInfo;

    QHash<int, TreatmentPlan> m_hashPlan;

public:
    Server(QWidget *parent = 0);
    ~Server();

signals:
    error_sendBackCheck();
    error_sendCheck();

private slots:
    void connectServer();    // Connect to client
    void startTransfer();    // Start session
    void displayError(QAbstractSocket::SocketError);    // Display error

    void sendPlan();
    void sendPlanHash();
    void sendHead(qint64 head);
    QString writeSendInfo();
    void sendCommandStart();
    void sendCommandStop();
    void sendCommandPause();
    void sendCommandResume();
    void readSendBack();
    void writtenBytes(qint64);

public slots:
};

#endif // SERVER_H
