#ifndef _CHARGING_CONTROLLER_H_
#define _CHARGING_CONTROLLER_H_

#include <linux/types.h>

extern int calculate_max_current(unsigned int cap_battery_now, unsigned int charge_till_cap);
extern int night_mode_icl(unsigned int cap_battery_now, unsigned int charge_till_cap);


#endif /* _CHARGING_CONTROLLER_H_ */
