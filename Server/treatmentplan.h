#ifndef TREATMENTPLAN
#define TREATMENTPLAN

#include <QList>
using namespace std;

struct Parameter
{
    short sonicationTime;
    short sonicationPeriod;
    float dutyCycle;
    short coolingTime;
};


struct Coordinate
{
    int spotNum;
    QList<double> spotPosX;
    QList<double> spotPosY;
    QList<double> spotPosZ;
};


struct TreatmentPlan
{
    Parameter parameter;
    Coordinate coordinate;
};


#endif // TREATMENTPLAN
