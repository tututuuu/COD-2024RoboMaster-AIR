#include "Gimbal_Task.h"
#include "motor.h"
#include "bsp_can.h"
#include "pid.h"
#include "cmsis_os.h"
#include "INS_Task.h"
#include "remote_control.h"

PID_Info_TypeDef GIMBAL_PID[2][2];

float GIMBAL_pid_para[2][2][6] = 
{
	[yaw] = 
	{
		[0] = {78,0,0,0,0,16000},
		[1] = {-40,-0.01,0,0,5000,16000},
	},
	[pitch] = 
	{
		[0] = {85,0,0,0,0,16000},
		[1] = {-45,-0.01,0,0,5000,16000},
	}
};
int z = 3;

static void GIMBAL_Init(void);
static void GIMBAL_PID_Cal(Gimbal_Info_t*gimbal_info);

void Gimbal_Task(void const * argument)
{
	GIMBAL_Init();
	osDelay(100);
	
  for(;;)
  {
		
		GIMBAL_PID_Cal(&gimbal_info);
		
    osDelay(1);
  }

}

static void GIMBAL_Init(void)
{
	PID_Init(&GIMBAL_PID[0][0],PID_POSITION,GIMBAL_pid_para[0][0]);
	PID_Init(&GIMBAL_PID[0][1],PID_POSITION,GIMBAL_pid_para[0][1]);
	PID_Init(&GIMBAL_PID[1][0],PID_POSITION,GIMBAL_pid_para[1][0]);
	PID_Init(&GIMBAL_PID[1][1],PID_POSITION,GIMBAL_pid_para[1][1]);
}

static void GIMBAL_PID_Cal(Gimbal_Info_t*gimbal_info)
{
	if(remote_ctrl.rc.s[0] == 3)
	{
		VAL_LIMIT(gimbal_info->target.pit_angle,-27,0);
		gimbal_info->target.yaw_angle += -0.0001*remote_ctrl.rc.ch[0] * z;
		gimbal_info->target.pit_angle += 0.0001*remote_ctrl.rc.ch[1] * z;	
	}
	gimbal_info->target.yaw_v = f_PID_Calculate(&GIMBAL_PID[0][0],gimbal_info->target.yaw_angle,INS_Info.yaw_tolangle);
	gimbal_info->sendvalve[0] = f_PID_Calculate(&GIMBAL_PID[0][1],gimbal_info->target.yaw_v,INS_Info.yaw_gyro);
//	gimbal_info->sendvalve[0] = f_PID_Calculate(&GIMBAL_PID[0][1],gimbal_info->target.yaw_v,gimbal[1].Data.velocity);
	gimbal_info->target.pit_v = f_PID_Calculate(&GIMBAL_PID[1][0],gimbal_info->target.pit_angle,INS_Info.rol_angle);
	gimbal_info->sendvalve[1] = f_PID_Calculate(&GIMBAL_PID[1][1],gimbal_info->target.pit_v,INS_Info.rol_gyro);

}

