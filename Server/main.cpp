#include <QApplication>
#include <QHash>
#include <QDataStream>
#include "server.h"
#include "treatmentplan.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Test data for debug
//    QHash<float, QList<Coordinate3D> > spot;

//    Coordinate3D spot1, spot2, spot3, spot4, spot5;
//    spot1.spotPosX = 11; spot1.spotPosY = 12; spot1.spotPosZ = 13;
//    spot2.spotPosX = 21; spot2.spotPosY = 22; spot2.spotPosZ = 23;
//    spot3.spotPosX = 31; spot3.spotPosY = 32; spot3.spotPosZ = 33;
//    spot4.spotPosX = 41; spot4.spotPosY = 42; spot4.spotPosZ = 43;
//    spot5.spotPosX = 51; spot5.spotPosY = 52; spot5.spotPosZ = 53;
//    QList<Coordinate3D> spotList45;
//    spot[45.0] = spotList45;

//    Coordinate3D spot6, spot7, spot8, spot9, spot10;
//    spot6.spotPosX = 1.1; spot6.spotPosY = 1.2; spot6.spotPosZ = 1.3;
//    spot7.spotPosX = 2.1; spot7.spotPosY = 2.2; spot7.spotPosZ = 2.3;
//    spot8.spotPosX = 3.1; spot8.spotPosY = 3.2; spot8.spotPosZ = 3.3;
//    spot9.spotPosX = 4.1; spot9.spotPosY = 4.2; spot9.spotPosZ = 4.3;
//    spot10.spotPosX = 5.1; spot10.spotPosY = 5.2; spot10.spotPosZ = 5.3;
//    QList<Coordinate3D> spotList90;
//    spotList90 << spot6 << spot7 << spot8 << spot9 << spot10;
//    spot[90.0] = spotList90;


    QHash<float, QList<QList> > spot;

    QList<float> spot1, spot2, spot3, spot4, spot5;
    spot1 << 11 << 12 << 13;
    spot2 << 21 << 22 << 23;
    spot3 << 31 << 32 << 33;
    spot4 << 41 << 42 << 43;
    spot5 << 51 << 52 << 53;
    QList<QList> spotList45;
    spotList45 << spot1 << spot2 << spot3 << spot4 << spot5;
    spot[45.0] = spotList45;

    QList<float> spot6, spot7, spot8, spot9, spot10;
    spot6 << 1.1 << 1.2 << 1.3;
    spot7 << 2.1 << 2.2 << 2.3;
    spot8 << 3.1 << 3.2 << 3.3;
    spot9 << 4.1 << 4.2 << 4.3;
    spot10 << 5.1 << 5.2 << 5.3;
    QList<QList> spotList90;
    spotList90 << spot6 << spot7 << spot8 << spot9 << spot10;
    spot[90.0] = spotList90;


    QHash<float, QList<int> > spotOrder;

    QList<int>  spotOrderList1, spotOrderList2;
    spotOrderList1 << 1 << 2 << 3 << 4 << 5;
    spotOrderList2 << 10 << 9 << 8 << 7 << 6;
    spotOrder[45.0] = spotOrderList1;
    spotOrder[90.0] = spotOrderList2;


    QHash<QString, float> parameter;
    parameter["volt"] = 5;
    parameter["sonicationTime"] = 10;
    parameter["sonicationPeriod"] = 50;
    parameter["dutyCycle"] = 0.5;
    parameter["coolingTime"] = 30;


    Server server;

    server.getCoordinate(spot);
    server.getSpotOrder(spotOrder);
    server.getParameter(parameter);
    server.show();

    return a.exec();
}
