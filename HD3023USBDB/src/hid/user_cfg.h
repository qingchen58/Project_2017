/*
********************************************************************************
**文件说明：配置文件
**文件名称： user_cfg.h 
**创建日期：2009.07.21  
**创 建 人：王玮
**------------------------------------------------------------------------------
**修改日期：2013.xx.xx
**修改说明：
********************************************************************************
*/
#ifndef USER_CFG_H
#define USER_CFG_H


/************************************************
**说明: 底层驱动文件
************************************************/
//#include "stm32l1xx.h"

/************************************************
**说明: 配置文件
************************************************/
//#include "adc_cfg.h"
//#include "isr_cfg.h"
//#include "spi_cfg.h"
//#include "i2c_cfg.h"
//#include "dma_cfg.h"
//#include "usart_cfg.h"



/************************************************
**说明: OS启用
************************************************/
#define USER_OS                             (1)




/************************************************
**说明: 版本号
************************************************/
#define VERSION                             (1.1)

/************************************************
**说明: 调试版本 
0为运行版本 
1为软件调试版本 
************************************************/
#define DEBUG                                 (1)

/************************************************
**说明: 系统标志事件定义
************************************************/
#define SYS_EVENT_SLAVE_BHT_RCV      (0x00000010)
#define SYS_EVENT_MEASURE_STOP       (0x00000100)
#define SYS_EVENT_SIM_MEASURE_STOP   (0x00000200)
#define SYS_EVENT_STOP_SAVE          (0x00001000)
#define SYS_EVENT_SYS_INIT_OVER      (0x80000000)

/************************************************
**说明: 测量模式定义
************************************************/
#define MEAS_MODE_MASK_L                   (0x0f)
#define MEAS_MODE_MASK_H                   (0xf0)
#define MEAS_MODE_SCANNING                 (0x01)
#define MEAS_MODE_MEASURE                  (0x02)

/************************************************
**说明: 系统蓝牙状态信息
主机开 2 从机开 4 ；主机联机 3 从机联机 5
************************************************/
#define BT_FLAG_MASK                       (0x07)
#define BT_FLAG_OFF                        (0x00)
#define BT_FLAG_CN                         (0x01)
#define BT_FLAG_MASTER_ON                  (0x02)
#define BT_FLAG_SLAVE_ON                   (0x04)

/************************************************
**说明: 有线联机状态信息
串口联机 1；USB联机 2
************************************************/
#define CONNECT_FLAG_ALL                   (0x03)
#define CONNECT_FLAG_NULL                  (0x00)
#define CONNECT_FLAG_USB                   (0x01)
#define CONNECT_FLAG_COMM                  (0x02)

/************************************************
**说明: USB通信数据帧含义
************************************************/
#define USB_SET_SYS_PAR                  (0x0001)
#define USB_SET_RUN_PAR                  (0x0002)
#define USB_SET_HV                       (0x0003)
#define USB_SET_BT                       (0x0004)
#define USB_SET_RTC                      (0x0005)
#define USB_SET_COFFT                    (0x0006)

#define USB_GET_SYS_PAR                  (0x0010)
#define USB_GET_RUN_PAR                  (0x0020)
#define USB_GET_HV                       (0x0030)
#define USB_GET_FILE_NAME                (0x0040)
#define USB_GET_FILE                     (0x0050)

#define USB_CMD_ACCESS_RIGHT             (0x0f10)
#define USB_CMD_HALT                     (0x0f01)
#define USB_CMD_FMKFS                    (0x0f02)
#define USB_CMD_DEBUG_TIME               (0x0f03)
#define USB_CMD_DEBUG_RESULT             (0x0f04)
#define USB_CMD_MEAS_START               (0x0f05)
#define USB_CMD_MEAS_STOP                (0x0f06)

#define USB_ACK_MEAS_RESULT              (0x0011)
#define USB_ACK_SYS_PAR                  (0x0022)
#define USB_ACK_RUN_PAR                  (0x0033)
#define USB_ACK_FILE_NAME                (0x0044)
#define USB_ACK_FILE                     (0x0055)

#define USB_ACK_OK                       (0x1000)
#define USB_ACK_FAIL                     (0xf000)


/************************************************
**说明: 有线串口通信数据帧含义
************************************************/
#define COM_START_CODE               (0xcc55bbaa)
#define COM_END_CODE                 (0xabbaeffe)
#define COM_SET_SYS_PAR                  (0x0001)
#define COM_SET_RUN_PAR                  (0x0002)
#define COM_SET_HV                       (0x0003)
#define COM_SET_RTC                      (0x0004)
#define COM_SET_COFFT                    (0x0005)

#define COM_GET_MEAS_RESULT              (0x0010)
#define COM_GET_SYS_PAR                  (0x0020)
#define COM_GET_RUN_PAR                  (0x0030)
#define COM_GET_FILE_NAME                (0x0040)
#define COM_GET_FILE                     (0x0050)

#define COM_CMD_ACCESS_RIGHT             (0x0f10)
#define COM_CMD_HALT                     (0x0f01)
#define COM_CMD_FMKFS                    (0x0f02)
#define COM_CMD_START_MEAS               (0x0f03)
#define COM_CMD_STOP_MEAS                (0x0f04)

#define COM_ACK_MEAS_RESULT              (0x0011)
#define COM_ACK_SYS_PAR                  (0x0022)
#define COM_ACK_RUN_PAR                  (0x0033)
#define COM_ACK_FILE_NAME                (0x0044)
#define COM_ACK_FILE                     (0x0055)

#define COM_ACK_OK                       (0x1000)
#define COM_ACK_FAIL                     (0xf000)


#define HV_MAX                           (1000)     //V
#define TH_MAX                           (2500)     //mv
#define MEASTIME_MAX                    (65535)      //S
/* 高压模拟量和数字量之间转换 */

/* 阈值模拟量和数字量之间转换 */
#define TH_BASE_VALUE                    (4096)
#endif



