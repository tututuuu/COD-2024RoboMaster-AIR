#ifndef _SHOOT_TASK_H
#define _SHOOT_TASK_H

enum 
{
	Frictl,
	Frictr,
	Trigger,
};

typedef struct
{
	struct
  {
		float frictl_v;
		float frictr_v;
		float trigger_v;
		
	}Target;
	
	int sendvalue[3];
		
}Shoot_Info_t;

extern Shoot_Info_t shoot_info;

#endif
