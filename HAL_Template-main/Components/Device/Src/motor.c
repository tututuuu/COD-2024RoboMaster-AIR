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

/* Includes ------------------------------------------------------------------*/
#include "motor.h"

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  transform the encoder(0-8192) to anglesum(3.4E38)
  */
static float encoder_to_anglesum(Motor_GeneralInfo_Typedef *,float ,uint16_t );
/**
  * @brief  transform the encoder(0-8192) to angle(-180-180)
  */
float encoder_to_angle(Motor_GeneralInfo_Typedef *,float ,uint16_t );
/** 
  * @brief  Judge the DJI Motor state
  */
static void DJI_Motor_ErrorHandler(DJI_Motor_Info_Typedef *);



DJI_Motor_Info_Typedef gimbal[motor_num] = 
{
	[yaw] = 
	{
	.Type = DJI_GM6020,
	.Data.StdId = 0x205,
	},
	[pitch] = 
  {
		.Type = DJI_GM6020,
		.Data.StdId = 0x206,
	},
	
};

DJI_Motor_Info_Typedef shoot[motor_num] = 
{
	[frictl] =
	{
		.Type = DJI_M3508,
		.Data.StdId = 0x201,
	},
	[frictr] = 
	{
		.Type = DJI_M3508,
		.Data.StdId = 0x202,
	},
	[trigger] =
  {
		.Type = DJI_M2006,
		.Data.StdId = 0x203,
	},	

};
/**
  * @brief  Update the DJI motor Information
  * @param  StdId  pointer to the specifies the standard identifier.
  * @param  rxBuf  pointer to the can receive data
  * @param  DJI_Motor pointer to a DJI_Motor_Info_t structure 
  *         that contains the information of DJI motor
  * @retval None
  */
void get_Motor_Data(uint32_t *canId, uint8_t *rxBuf,DJI_Motor_Info_Typedef *DJI_Motor)
{
	/* check the StdId */
	if(*canId != DJI_Motor->Data.StdId) return;
	
	/* transforms the  general motor data */
	DJI_Motor->Data.temperature = rxBuf[6];
	DJI_Motor->Data.encoder  = ((int16_t)rxBuf[0] << 8 | (int16_t)rxBuf[1]);
	DJI_Motor->Data.velocity = ((int16_t)rxBuf[2] << 8 | (int16_t)rxBuf[3]);
	DJI_Motor->Data.current  = ((int16_t)rxBuf[4] << 8 | (int16_t)rxBuf[5]);
	
	/* Judge the motor error	*/
	DJI_Motor_ErrorHandler(DJI_Motor);


	/* transform the encoder to anglesum */
	switch(DJI_Motor->Type)
	{
		case DJI_GM6020:
			DJI_Motor->Data.angle = encoder_to_anglesum(&DJI_Motor->Data,1.f,8192);
		break;
	
		case DJI_M3508:
			DJI_Motor->Data.angle = encoder_to_anglesum(&DJI_Motor->Data,3591.f/187.f,8192);
		break;
		
		case DJI_M2006:
			DJI_Motor->Data.angle = encoder_to_anglesum(&DJI_Motor->Data,36.f,8192);
		break;
		
		default:break;
	}
}

//------------------------------------------------------------------------------

/**
  * @brief  transform the encoder(0-8192) to anglesum(3.4E38)
  * @param  *Info        pointer to a Motor_GeneralInfo_Typedef structure that 
	*					             contains the infomation for the specified motor
  * @param  torque_ratio the specified motor torque ratio
  * @param  MAXencoder   the specified motor max encoder number
  * @retval anglesum
  */
static float encoder_to_anglesum(Motor_GeneralInfo_Typedef *Info,float torque_ratio,uint16_t MAXencoder)
{
  float res1 = 0,res2 =0;
  
  if(Info == NULL) return 0;
  
  /* Judge the motor Initlized */
  if(Info->Initlized != true)
  {
    /* update the last encoder */
    Info->last_encoder = Info->encoder;

    /* reset the angle */
    Info->angle = 0;

    /* Set the init flag */
    Info->Initlized = true;
  }
  
  /* get the possiable min encoder err */
  if(Info->encoder < Info->last_encoder)
  {
      res1 = Info->encoder - Info->last_encoder + MAXencoder;
  }
  else if(Info->encoder > Info->last_encoder)
  {
      res1 = Info->encoder - Info->last_encoder - MAXencoder;
  }
  res2 = Info->encoder - Info->last_encoder;
  
  /* update the last encoder */
  Info->last_encoder = Info->encoder;
  
  /* transforms the encoder data to tolangle */
	if(fabsf(res1) > fabsf(res2))
	{
		Info->angle += (float)res2/(MAXencoder*torque_ratio)*360.f;
	}
	else
	{
		Info->angle += (float)res1/(MAXencoder*torque_ratio)*360.f;
	}
  
  return Info->angle;
}
//------------------------------------------------------------------------------

/**
  * @brief  float loop constrain
  * @param  Input    the specified variables
  * @param  minValue minimum number of the specified variables
  * @param  maxValue maximum number of the specified variables
  * @retval variables
  */
static float f_loop_constrain(float Input, float minValue, float maxValue)
{
  if (maxValue < minValue)
  {
    return Input;
  }
  
  float len = maxValue - minValue;    

  if (Input > maxValue)
  {
      do{
          Input -= len;
      }while (Input > maxValue);
  }
  else if (Input < minValue)
  {
      do{
          Input += len;
      }while (Input < minValue);
  }
  return Input;
}
//------------------------------------------------------------------------------

/**
  * @brief  transform the encoder(0-8192) to angle(-180-180)
  * @param  *Info        pointer to a Motor_GeneralInfo_Typedef structure that 
	*					             contains the infomation for the specified motor
  * @param  torque_ratio the specified motor torque ratio
  * @param  MAXencoder   the specified motor max encoder number
  * @retval angle
  */
float encoder_to_angle(Motor_GeneralInfo_Typedef *Info,float torque_ratio,uint16_t MAXencoder)
{	
  float encoder_err = 0.f;
  
  /* check the motor init */
  if(Info->Initlized != true)
  {
    /* update the last encoder */
    Info->last_encoder = Info->encoder;

    /* reset the angle */
    Info->angle = 0;

    /* config the init flag */
    Info->Initlized = true;
  }
  
  encoder_err = Info->encoder - Info->last_encoder;
  
  /* 0 -> MAXencoder */		
  if(encoder_err > MAXencoder*0.5f)
  {
    Info->angle += (float)(encoder_err - MAXencoder)/(MAXencoder*torque_ratio)*360.f;
  }
  /* MAXencoder-> 0 */		
  else if(encoder_err < -MAXencoder*0.5f)
  {
    Info->angle += (float)(encoder_err + MAXencoder)/(MAXencoder*torque_ratio)*360.f;
  }
  else
  {
    Info->angle += (float)(encoder_err)/(MAXencoder*torque_ratio)*360.f;
  }
  
  /* update the last encoder */
  Info->last_encoder = Info->encoder;
  
  /* loop constrain */
  f_loop_constrain(Info->angle,-180.f,180.f);

  return Info->angle;
}
//------------------------------------------------------------------------------

/** 
  * @brief  Judge the DJI Motor state
  * @param  *DJI_Motor pointer to a DJI_Motor_Info_Typedef structure that contains
  *                    the configuration information for the specified motor.  
  * @retval None
  */
static void DJI_Motor_ErrorHandler(DJI_Motor_Info_Typedef *DJI_Motor)
{
	/* Judge the DJI motor temperature */
	if(DJI_Motor->Data.temperature > 80)
	{
    DJI_Motor->ERRORHandler.ErrorCount++;

    if(DJI_Motor->ERRORHandler.ErrorCount > 200)
    {
      DJI_Motor->ERRORHandler.Status = MOTOR_OVER_TEMPERATURE;
      DJI_Motor->ERRORHandler.ErrorCount = 0;
    }
	}
  else
	{
    DJI_Motor->ERRORHandler.ErrorCount = 0;	
	}
}
//------------------------------------------------------------------------------



