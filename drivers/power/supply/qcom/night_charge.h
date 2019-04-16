#ifndef _NIGHT_CHARGE_H_
#define _NIGHT_CHARGE_H_

#include <linux/types.h>

extern void calculate_max_current(unsigned int cap_battery_now, unsigned int charge_till_cap);
extern void updateBatteryStats(unsigned int cap_battery_now);
extern void reset_values(void);

extern int custom_icl;

#endif /* _NIGHT_CHARGE_H_ */
