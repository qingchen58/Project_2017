/*
********************************************************************************
**文件说明：数据类型定义文件
**文件名称：user_type.h 
**创建日期：2009.07.21  
**创 建 人：王玮
**------------------------------------------------------------------------------
**修改日期：2013.xx.xx
**修改说明：
********************************************************************************
*/


#ifndef USER_TYPE_H
#define USER_TYPE_H

/*
--------------------------------------------------------------------------------
说明：数据类型，位长度。
--------------------------------------------------------------------------------
*/
typedef unsigned       char  Bool;
typedef unsigned       char  Uint8;
typedef unsigned short int   Uint16;
typedef unsigned       int   Uint32;
typedef signed         char  Int8;
typedef signed short   int   Int16;
typedef signed         int   Int32;
typedef float                Float32;

/*
--------------------------------------------------------------------------------
说明: 真假值
--------------------------------------------------------------------------------
*/
//typedef enum {TRUE = 1, FALSE = !TRUE} BoolStatus;


/*
--------------------------------------------------------------------------------
说明: 8bit位掩码 
--------------------------------------------------------------------------------
*/
typedef enum{
    MASK_BIT0 = 0x01,
    MASK_BIT1 = 0x02,
    MASK_BIT2 = 0x04,
    MASK_BIT3 = 0x08,
    MASK_BIT4 = 0x10,
    MASK_BIT5 = 0x20,
    MASK_BIT6 = 0x40,
    MASK_BIT7 = 0x80,
}BIT_MASK;

//------------------------------------------------------------------------------
//说明：  Set or clear all bits in the mask
//------------------------------------------------------------------------------

#define ClearMask(addr,mask) addr = (addr & ~(mask))
#define SetMask(addr,mask) addr = (addr | (mask))
#define SeeMask(addr,mask) addr = (addr & mask) 

#define ReadMask(addr,mask) (addr & (mask))
#define WriteMask(addr,data,mask) addr = (addr & ~(mask)) | (data)
#define AntiMask(addr,mask) addr=(addr | mask) & ~(addr & mask) 

/***************************************************
说明：RTC数据格式
****************************************************/
typedef struct
{
    Uint8 Second;  //秒    
    Uint8 Minute;  //分    
    Uint8 Hour;    //时
    Uint8 Day;     //星期
    Uint8 Date;    //日
    Uint8 Month;   //月
    Uint8 Year;    //年
}TIME_Dat;

typedef struct 
{
    Uint8  Index;
    Uint8  CharBuf[10];    //字符串缓存 
    Uint32 Value;          //字符串转换成16进制值
}CHAR_Dat;


/**************************************************
说明：测量控制参数数据类型
**************************************************/
typedef struct
{
    Uint8  MeasMode;      //测量模式
    Uint16 MeasNo;        //测量次数
    Uint16 MeasGroup;     //测量组数
    Uint16 tSecond;       //秒倒计数
}MEAS_CONDat;

/**************************************************
说明：系统运行控制数据类型
**************************************************/
typedef struct
{
    Uint8  State   ;        //系统当前状态
    Uint8  OldState;        //系统曾经状态
    Uint8  MeasInfo;        //测量状态信息--开启/关闭
    Uint8  ContInfo;        //连接状态 --串口连接/USB连接
    Uint8  BtInfo;          //蓝牙信息 --电源开-连接状态
    Uint8  GpsInfo;         //GPS信息  --电源开-卫星个数
    Uint8  DiskInfo;        //磁盘信息 --剩余容量 百分比 0-99
    Uint8 FileName[10];     //文件名缓存
    TIME_Dat  CtTime;       //当前时间
    Uint32 CtSecond;        //当前时间--转换成秒累计数
    Uint16 POnSec;          //开机秒计时，用于自动关机
    MEAS_CONDat MEASConDat; //测量控制数据
}SYS_Dat;

#pragma pack(push) //保存对齐状态
#pragma pack(1)    //设定为1字节对齐

/**************************************************
说明：系统运行参数数据类型
**************************************************/
typedef struct
{
    Uint8  AtuoOffTime;     //自动关机时间
    Uint8  AutoPrint;       //自动打印 开/关
    Uint16 MeasTime;        //测量时间
    Uint16 MeasNo;          //测量次数
    Uint16 MeasGroup;       //测量组数
}RUNPAR_Dat;

/**************************************************
说明：系统设置参数数据类型
**************************************************/
typedef struct
{
    Uint16  HVSDat;         //高压控制
    Float32 Cofft;          //校正系数
    Float32 DTCofft;        //死时间修正系数
    Float32 Bkgd;           //本底
    Uint32  SN;             //仪器编号
}SYSPAR_Dat;

/**************************************************
说明：系统初始化数据类型
**************************************************/
typedef struct
{
    Uint32 RamInitFlg;   //Ram初始化参数
}INIT_Dat;

typedef struct
{
    INIT_Dat   InitDat;     //系统初始化及授权信息
    RUNPAR_Dat RunPar;      //运行参数
    SYSPAR_Dat SysPar;      //系统参数
}PAR_Dat;

/**************************************************
说明：串口命令数据类型
**************************************************/
typedef struct
{
    Uint32 Start;
    Uint16 Cmd;
    Uint8  Buf[22];
    Uint32 CRCDat;
}USART_CmdDat;

typedef struct
{
    Uint16 RcvCnt;
    Uint16 SendCnt;
    USART_CmdDat RcvDat;
    USART_CmdDat SendDat;
}BTUSART_Dat;

/**************************************************
说明：USB命令数据类型
**************************************************/
typedef struct
{
    Uint16 CmdCode;    //命令码
    Uint8  Buf[62];    //数据缓存
}USB_CmdDat;

typedef struct
{
    Uint8 Buf[64];    //USB数据缓存
}USB_RcvBufDat;
typedef struct
{
    Uint8 Buf[64];    //USB数据缓存
}USB_SendBufDat;

typedef struct
{
    USB_RcvBufDat RcvDat;
    USB_SendBufDat SendDat;
}USB_BufDat;

#pragma pack(pop)

/**************************************************
说明：GPS 信息串口接收缓存结构体
**************************************************/
typedef struct
{
    Uint8 EntCnt;    //回车个数
    Uint16 RcvCnt;   //接收计数器
    Uint8 RcvBuf[300];  //接收缓存
}GPSRCV_Dat;

/**************************************************
说明：GPS 信息存储数据结构体
**************************************************/
typedef struct {
    Uint8 LatDr;     //纬度度数
    Uint8 LatFr;     //纬度分度
    Uint8 LatSec;    //纬度秒数
    Uint8 SN;        //纬度半球N（北半球）或S（南半球）
    Uint8 LonDr;     //纬度度数
    Uint8 LonFr;     //纬度分度
    Uint8 LonSec;    //经度秒数
    Uint8 EW;        //经度半球E（东经）或W（西经）
    Uint8 GPM;       //定位状态 A = 有效定位，V =无效定位
    Uint8 AltSyb;    //海拔高度符号（-9999.99~99999.99）
    Uint16 AltInt;   //海拔高度整数部分
    Uint32 SecCnt;   //当前时间
}GPSSAVE_Dat;

/**************************************************
说明：GPS 数据结构体
**************************************************/
typedef struct {
    Uint8  Lat[10];    //纬度ddmm.mmmm （度分）格式
    Uint8  SN;        //纬度半球N（北半球）或S（南半球）
    Uint8  Lon[11];   //经度dddmm.mmmm （度分）格式
    Uint8  EW;        //经度半球E（东经）或W（西经）
    Uint8  GPM;       //定位状态 A = 有效定位，V =无效定位
    Uint8  SNum;      //正在使用解算位置的卫星数量（00~12）
    Uint8  Alt[8];    //海拔高度（-9999.99~99999.99）
    Uint8  SPM;       //定位模式,1=未定位，2=2D定位，3=3D定位
    Uint8  Speed[5];  //地面速率（000.0~999.9 节）
    Uint8  Cour[5];   //地面航向（000.0~359.9 度，以真北为参考基准）
    Uint32 SecCnt;    //当前年月日时分秒计数
}GPS_Dat;


/**************************************************
说明: 存储数据结构
**************************************************/
typedef struct
{
    Float32  TempDat;   //温度
    Float32  RHDat;     //湿度
    Float32  PresDat;   //气压
}MEMS_INFO_Dat;

typedef struct
{
    Uint8  MeasMode;      //测量模式
    Uint8  MeasTime ;     //测量时间
    Uint8  BottleType;    //瓶子类型
    Uint16 SealTime;      //密封时间
    Uint32 ResultCnt;     //测量计数值
    Uint16 BkgdCnt;       //本底计数值
}MEAS_RESDat;

typedef struct
{
    Uint16 PointNum;         //当前点号
    Uint32 SecondCnt;        //当前时间
    MEAS_RESDat MeasResDat;  //测量结果
    MEMS_INFO_Dat MEMSDat;   //传感器值 温度/湿度/气压
}SAVE_Dat;

#endif
