#ifndef TREATMENTPLAN
#define TREATMENTPLAN

#include <QList>
using namespace std;

struct Parameter
{
    float volt;
    short sonicationTime;
    short sonicationPeriod;
    float dutyCycle;
    short coolingTime;
};


struct Coordinate3D
{
    float spotPosX;
    float spotPosY;
    float spotPosZ;
};


struct TreatmentPlan
{
    Parameter parameter;
    Coordinate coordinate;
};

#endif // TREATMENTPLAN
