#ifndef _CHARGING_CONTROLLER_H_
#define _CHARGING_CONTROLLER_H_

#include <linux/types.h>

extern int calculate_max_current(unsigned int cap_battery_now);
extern int night_mode_icl(unsigned int cap_battery_now);


#endif /* _CHARGING_CONTROLLER_H_ */
