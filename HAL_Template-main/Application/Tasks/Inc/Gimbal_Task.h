#ifndef _GIMBAL_TASK_H
#define _GIMBAL_TASK_H


typedef struct
{
	struct
	{
		float yaw_v;
		float yaw_angle;
		float pit_v;
		float pit_angle;
	}target;
	int sendvalve[2];

}Gimbal_Info_t;

extern Gimbal_Info_t gimbal_info;

#endif
