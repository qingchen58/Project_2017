#ifndef UTILS_H
#define UTILS_H
#include"user_type.h"
#include"user_cfg.h"
#include<QString>

class Utils
{
public:
    Utils();
    Uint32 timeToLong(TIME_Dat * CurrentTime);
    void longToTime(TIME_Dat * CurrentTime, Uint32 timecount);
    QString numberToWeek(Uint8 Day);
    QString cmdToCmdString(ushort Cmd);
    Uint8 cutLongYearToShort(Uint16 LongYear);
    QString numberToMode(Uint8 Num);
private:

    Uint8 isLeapYear(Uint8 year);
};


#endif // UTILS_H
