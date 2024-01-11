#include "CANTx_Task.h"
#include "cmsis_os.h"
#include "Gimbal_Task.h"
#include "Shoot_Task.h"
#include "bsp_can.h"
#include "remote_control.h"
#include "INS_Task.h"

Gimbal_Info_t gimbal_info;
//Shoot_Info_t shoot_info;

void CANTx_Task(void const * argument)
{
   
  for(;;)
  {
		if((remote_ctrl.rc.s[0] == 2 && remote_ctrl.rc.s[1] == 2) || (remote_ctrl.rc.s[1] == 0) || (remote_ctrl.rc.s[1] == 0))
		{
			gimbal_info.sendvalve[0] = 0;
			gimbal_info.sendvalve[1] = 0;
			shoot_info.sendvalue[0] = 0;
			shoot_info.sendvalue[1] = 0;
			shoot_info.sendvalue[2] = 0;
			gimbal_info.target.yaw_angle = INS_Info.yaw_tolangle;
			gimbal_info.target.pit_angle = INS_Info.rol_angle;
      			
		}
		
		Gimbal_TxFrame.data[0] = (uint8_t)(gimbal_info.sendvalve[0]>>8);
		Gimbal_TxFrame.data[1] = (uint8_t)(gimbal_info.sendvalve[0]);
		Gimbal_TxFrame.data[2] = (uint8_t)(gimbal_info.sendvalve[1]>>8);
		Gimbal_TxFrame.data[3] = (uint8_t)(gimbal_info.sendvalve[1]);
		
		USER_CAN_TxMessage(&Gimbal_TxFrame);
		
		Shoot_TxFrame.data[0] = (uint8_t)(shoot_info.sendvalue[0]>>8);
		Shoot_TxFrame.data[1] = (uint8_t)(shoot_info.sendvalue[0]);
		Shoot_TxFrame.data[2] = (uint8_t)(shoot_info.sendvalue[1]>>8);
		Shoot_TxFrame.data[3] = (uint8_t)(shoot_info.sendvalue[1]);
		Shoot_TxFrame.data[4] = (uint8_t)(shoot_info.sendvalue[2]>>8);
		Shoot_TxFrame.data[5] = (uint8_t)(shoot_info.sendvalue[2]);
		
		USER_CAN_TxMessage(&Shoot_TxFrame);
		

		osDelay(1);
	}

}
