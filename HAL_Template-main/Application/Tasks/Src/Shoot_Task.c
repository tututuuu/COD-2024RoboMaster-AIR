#include "Shoot_Task.h"
#include "motor.h"
#include "bsp_can.h"
#include "pid.h"
#include "remote_control.h"
#include "cmsis_os.h"


PID_Info_TypeDef SHOOT_PID[3];
Shoot_Info_t shoot_info;

float SHOOT_pid_para[3][6] = 
{
	[Frictl] = {10,0.001,0,0,3000,16300},
	[Frictr] = {10,0.001,0,0,3000,16300},
  [Trigger] = {10,0.001,0,0,3000,16300},
};
int frict_target_v = 0;
int frict_v = 0;
float trigger_kv = 8.1;
int stuck_flag = 1;
uint16_t cnt = 0;
int fztime = 100; 
int flag = 0;

static void SHOOT_Init(void);
static void SHOOT_PID_Cal(Shoot_Info_t*shoot_info);
static void Trigger_handle(void);

void Shoot_Task(void const * argument)
{
	SHOOT_Init();
	osDelay(100);
	
  for(;;)
  {
		Trigger_handle();
		SHOOT_PID_Cal(&shoot_info);
    osDelay(1);
  }

}

bool Judge_IF_block(void)
{
	bool res = false;
	if((ABS(shoot_info.Target.trigger_v - shoot[trigger].Data.velocity)>300) && (shoot[trigger].Data.velocity<300))
	{
		res = true;
		flag = 1;
	}
	else flag = 0;
	return res;
}

static void Trigger_handle(void)
{
	static uint16_t time;
	if(Judge_IF_block() == true)
	{
		cnt++;
		if(cnt>300)
		{
			stuck_flag = -1;
		  cnt = 0;
		}
	}
	else 
  {
		cnt = 0;
	}
	
	if(stuck_flag == -1)	time++;
	if(stuck_flag == -1 && time>fztime)
	{
		stuck_flag = 1;
		time = 0;		
	}			
}


static void SHOOT_Init(void)
{
	PID_Init(&SHOOT_PID[0],PID_POSITION,SHOOT_pid_para[0]);
	PID_Init(&SHOOT_PID[1],PID_POSITION,SHOOT_pid_para[1]);
	PID_Init(&SHOOT_PID[2],PID_POSITION,SHOOT_pid_para[2]);
}

static void SHOOT_PID_Cal(Shoot_Info_t*shoot_info)
{
	if(remote_ctrl.rc.s[0] == 3)
	{
	  shoot_info->Target.trigger_v = remote_ctrl.rc.ch[2] * trigger_kv * stuck_flag;
		frict_target_v = frict_v;
		if(remote_ctrl.rc.s[1] == 3)
		{
			frict_target_v = 0;		
		}
	}
	if((remote_ctrl.rc.s[0] == 2 && remote_ctrl.rc.s[1] == 2) || (remote_ctrl.rc.s[1] == 0) || (remote_ctrl.rc.s[1] == 0))
	{
		frict_target_v = 0;
	}
  shoot_info->sendvalue[0] = f_PID_Calculate(&SHOOT_PID[0],frict_target_v,shoot[frictl].Data.velocity);
	shoot_info->sendvalue[1] = f_PID_Calculate(&SHOOT_PID[1],-frict_target_v,shoot[frictr].Data.velocity);
	shoot_info->sendvalue[2] = f_PID_Calculate(&SHOOT_PID[2],shoot_info->Target.trigger_v,shoot[trigger].Data.velocity);
	
}

