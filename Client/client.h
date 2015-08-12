#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtWidgets>
#include <QtNetwork>
#include <QHash>
#include "treatmentplan.h"

class Client : public QDialog
{
    Q_OBJECT

private:
    QTcpServer m_tcpClient;
    QTcpSocket *m_tcpClientConnection;
    QString m_ipAddress;

    qint64 m_totalBytes;    // Total bytes of data
    qint64 m_bytesReceived;    // Length of received data
    qint64 m_dataKeySize;    // Information of filename length
    QString m_dataKey;    // Save the key of data
    QString m_dataValue;

    QByteArray m_inBlock;    // Data buffer
    QByteArray m_outBlock;    // Data buffer
    QDataStream out;

    TreatmentPlan m_plan;
    QHash<int, TreatmentPlan> m_hashPlan;
    qint64 m_command;

    QPushButton *u_connectButton;

public:
    Client(QWidget *parent = 0);
    ~Client();

signals:
    commandStart();
    commandStop();
    commandPause();
    commandResume();

private slots:
    void start();    // Start to listen port
    void acceptConnection();    // Build connection
    void updateClientProgress();    //Update progress bar, receive the data
    void displayError(QAbstractSocket::SocketError socketError);    //Display the error
    QString getLocalIP();

    void readHead();
    void receivePlan();
    void receivePlanHash();
    void receiveCommand();
    void bytes(qint64 byteswrite);
};

#endif // CLIENT_H
