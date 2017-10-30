#include "utils.h"
#include "user_cfg.h"

static const Uint8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

Utils::Utils()
{
    
}

/*
********************************************************************************
* 名    称：Uint32 TIME_TimeToLong(void)
* 功    能：时间转换函数
* 入口参数：无
* 出口参数：当前时间对应的秒计数
********************************************************************************
*/
Uint32 Utils::timeToLong(TIME_Dat *CurrentTime)
{
    Uint8 i;
    Uint8 Val_year,Val_mon,Val_day,Val_hour,Val_min,Val_sec;
    Uint32 Sec_count = 0;
    Val_year = CurrentTime->Year;
    Val_mon  = CurrentTime->Month;
    Val_day  = CurrentTime->Date;
    Val_hour = CurrentTime->Hour;
    Val_min  = CurrentTime->Minute;
    Val_sec  = CurrentTime->Second;     
    for(i = 0;i < Val_year;i ++){     //把所有年份的秒钟相加
        if(isLeapYear(i)){
            Sec_count += 31622400;    //闰年的秒钟数
        }else{ 
			Sec_count += 31536000;    //平年的秒钟数
        }
	}
	Val_mon -= 1;
	for(i = 0;i < Val_mon;i ++){ //把前面月份的秒钟数相加
        Sec_count += (Uint32)mon_table[i] * 86400;    //月份秒钟数相加
        if(isLeapYear(Val_year) && i == 1){
			Sec_count += 86400;        //闰年2月份增加一天的秒钟数
        }
	}
    Sec_count += (Uint32)(Val_day - 1) * 86400;//把前面日期的秒钟数相加 
    Sec_count += (Uint32)Val_hour * 3600;//小时秒钟数
    Sec_count += (Uint32)Val_min * 60;      //分钟秒钟数	
    Sec_count += Val_sec;//最后的秒钟加上去
    return Sec_count;
}
/*
********************************************************************************
* 名    称：TIME_LongToTime(TIME_Dat * CurrentTime, Uint32 timecount)
* 功    能：秒累计转换成时间
* 入口参数：TIME_Dat * CurrentTime 当前时间
            Uint32 timecount  累计秒数  
* 出口参数：无
********************************************************************************
*/	
void Utils::longToTime(TIME_Dat *CurrentTime, Uint32 timecount)
{
    Uint32 temp = 0;
	Uint8  temp1,temp2; 	   

 	temp = timecount / 86400; //得到天数
	if(temp != 0){            //超过一天了
        temp1 = 0;            //从2000年开始
        while(temp >= 365){
            if(isLeapYear(temp1)){    //是闰年
                if(temp >= 366){
                    temp -= 366;
                }else break;  
            }else {
                temp -= 365;       //平年
            }
            temp1 ++;  
		}   
		CurrentTime->Year = temp1;  	  //得到年份
		temp2 = 0;
		while(temp >= 28){         //超过了一个月的最小值
            if(isLeapYear(temp1) && temp2 == 1){ //当年是闰年并且是2月份
				if(temp >= 29){
                    temp -= 29;
                }else break; 
			}else{
                if(temp >= mon_table[temp2]){
                    temp -= mon_table[temp2];    //平年
                } else break;
			}
            temp2 ++;  
		}
        temp2 = temp2 + 1;			 //得到月份
		CurrentTime->Month = temp2;  
		temp = temp + 1;				//得到日期
		CurrentTime->Date = temp;  
	}
	temp = timecount % 86400;     //得到秒钟数      
	temp2 = temp / 3600;     //小时
	CurrentTime->Hour = temp2;  
	temp2 = (temp % 3600) / 60; //分钟      
	CurrentTime->Minute= temp2;  
	temp2 = (temp % 3600) % 60; //秒钟
    CurrentTime->Second = temp2;
}


QString Utils::numberToWeek(Uint8 Day)
{
    QString week;
    switch (Day) {
    case 1:
        week = "周一";
        break;
    case 2:
        week = "周二";
        break;
    case 3:
        week = "周三";
        break;
    case 4:
        week = "周四";
        break;
    case 5:
        week = "周五";
        break;
    case 6:
        week = "周六";
        break;
    case 7:
        week = "周日";
        break;
    default:
        break;
    }
    return week;
}

//#define USB_ACK_MEAS_RESULT              (0x0011)
//#define USB_ACK_SYS_PAR                  (0x0022)
//#define USB_ACK_RUN_PAR                  (0x0033)
//#define USB_ACK_FILE_NAME                (0x0044)
//#define USB_ACK_FILE                     (0x0055)

//#define USB_ACK_OK                       (0x1000)
//#define USB_ACK_FAIL                     (0xf000)
QString Utils::cmdToCmdString(ushort Cmd)
{
    QString cmdStr;
    
    switch (Cmd) {
    case USB_SET_SYS_PAR:
        cmdStr = "USB_SET_SYS_PAR";
        break;
    case USB_SET_RUN_PAR:
        cmdStr = "USB_SET_RUN_PAR";
        break;
    case USB_SET_HV:
        cmdStr = "USB_SET_HV";
        break;
    case USB_SET_BT:
        cmdStr = "USB_SET_BT";
        break;
    case USB_SET_RTC:
        cmdStr = "USB_SET_RTC";
        break;
    case USB_SET_COFFT:
        cmdStr = "USB_SET_COFFT";
        break;
    case USB_GET_SYS_PAR:
        cmdStr = "USB_GET_SYS_PAR";
        break;
    case USB_GET_RUN_PAR:
        cmdStr = "USB_GET_RUN_PAR";
        break;
    case USB_GET_FILE_NAME:
        cmdStr = "USB_GET_FILE_NAME";
        break;
    case USB_GET_FILE:
        cmdStr = "USB_GET_FILE";
        break;
    case USB_CMD_HALT:
        cmdStr = "USB_CMD_HALT";
        break;
    case USB_CMD_FMKFS:
        cmdStr = "USB_CMD_FMKFS";
        break;
    case USB_CMD_DEBUG_TIME:
        cmdStr = "USB_CMD_DEBUG_TIME";
        break;
    case USB_CMD_DEBUG_RESULT:
        cmdStr = "USB_CMD_DEBUG_RESULT";
        break;
    case USB_CMD_MEAS_START:
        cmdStr = "USB_CMD_MEAS_START";
        break;
    case USB_CMD_MEAS_STOP:
        cmdStr = "USB_CMD_MEAS_STOP";
        break;
    default:
        break;
    }
    
    return cmdStr;
}

//----------------------------------------------------
//例：2016 -> 16
//----------------------------------------------------
Uint8 Utils::cutLongYearToShort(Uint16 LongYear)
{
    QString str = QString::number(LongYear);
    
    /* 取后两位 */
    str = str.mid(str.length() - 2,2);
    
    return (Uint8)str.toUShort();
}
/*
********************************************************************************
* 名    称：u8 Is_Leap_Year(u8 year)
* 功    能：闰年判断函数
* 入口参数：year  
* 出口参数：是闰年返回1  平年返回0
********************************************************************************
*/
Uint8 Utils::isLeapYear(Uint8 year)
{
   Uint16 year_value;
   year_value = year + 2000;
   if(year_value % 4 == 0)   { 	    //必须能被4整除
      if(year_value % 100 == 0) {  
          if(year_value % 400 == 0){	   //如果以00结尾,还要能被400整除 
            return 1;
          } else {
              return 0; 
          }
      } else{ 
        return 1; 
      }
   }else{ 
    return 0; 
   }

}

