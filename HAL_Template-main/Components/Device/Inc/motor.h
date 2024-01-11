/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : motor.c
  * @brief          : motor interfaces functions 
  * @author         : Yan Yuanbin
  * @date           : 2023/04/27
  * @version        : v1.0
  ******************************************************************************
  * @attention      : to be tested
  ******************************************************************************
  */
/* USER CODE END Header */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DEVICE_MOTOR_H
#define DEVICE_MOTOR_H


/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "stm32f4xx.h"
#include "pid.h"

/* Exported types ------------------------------------------------------------*/

/**
 * @brief typedef enum that contains the Error status for Motor Device.
 */
typedef enum
{
  MOTOR_ERROR_NONE = 0x00U,   /*!< no error */
  MOTOR_CAN_OFFLINE = 0x01U,    /*!< CAN transfer failed */
  MOTOR_OVER_TEMPERATURE = 0x02U,   /*!< abnormal motor temperature */
}Motor_Status_e;



/**
 * @brief typedef enum that contains the type of DJI Motor Device.
 */
typedef enum{
    DJI_GM6020,
    DJI_M3508,
    DJI_M2006,
    DJI_MOTOR_TYPE_NUM,
}DJI_Motor_Type_e;


typedef enum
{
	yaw,
	pitch,
	frictl,
	frictr,
	trigger,
	motor_num,

}MOTOR_NUM_t;

/**
 * @brief typedef structure that contains the information for the Motor Error handler.
 */
typedef struct 
{
  uint16_t ErrorCount;    /*!< Error status judgment count */
  Motor_Status_e Status;   /*!< Error status */
}Motor_ErrorrHandler_Typedef;


/**
 * @brief typedef structure that contains the General information for the Motor Device.
 */
typedef struct 
{
  bool Initlized;   /*!< init flag */
  uint32_t StdId;
  int16_t  current;   /*!< Motor electric current */
  int16_t  velocity;    /*!< Motor rotate velocity */
  int16_t  encoder;   /*!< Motor encoder angle */
  int16_t  last_encoder;   /*!< previous Motor encoder angle */
  float    angle;   /*!< Motor angle in degree */
  uint8_t  temperature;   /*!< Motor Temperature */
}Motor_GeneralInfo_Typedef;

/**
 * @brief typedef structure that contains the information for the DJI Motor Device.
 */
typedef struct
{
	DJI_Motor_Type_e Type;   /*!< Type of Motor */
	Motor_GeneralInfo_Typedef Data;   /*!< information for the Motor Device */
	Motor_ErrorrHandler_Typedef ERRORHandler;   /*!< information for the Motor Error */
}DJI_Motor_Info_Typedef;


/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Update the DJI motor Information
  */
extern void get_Motor_Data(uint32_t *StdId, uint8_t *rxBuf,DJI_Motor_Info_Typedef *DJI_Motor);


extern DJI_Motor_Info_Typedef gimbal[motor_num],shoot[motor_num];

#endif //DEVICE_MOTOR_H
