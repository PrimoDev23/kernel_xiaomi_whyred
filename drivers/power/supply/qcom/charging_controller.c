#define module_name "charging_controller"

#include <linux/moduleparam.h>
#include <linux/state_notifier.h>
#include "charging_controller.h"

static unsigned int input_current_limit = 0; //0 = disabled module
static unsigned int mode = 0; //0 = off, 1 = night mode, 2 = fast charge (just increased maximum current for now)

module_param(input_current_limit, uint, 0644);
module_param(mode, uint, 0644);

extern int calculate_max_current(unsigned int cap_battery_now)
{
	if(mode == 0)
		return 0;

	if(mode == 1){
		return night_mode_icl(cap_battery_now);
	}

	return 0;
}

extern int night_mode_icl(unsigned int cap_battery_now)
{
	//init voltages for battery capacities and calc average
	unsigned int thirty_voltage = 3700; //mv
	unsigned int eighty_voltage = 4000; //mv
	float average = (thirty_voltage + eighty_voltage) / 2; //average will be used to calculate needed charging
	
	unsigned int capacity = 2000 - (4000 * (cap_battery_now - 30) / 100); //Calculate left capacity to 80%
	float power_batt_valued = capacity * average;
	unsigned int time_h = 6; //expose to userspace later

	unsigned int icl = (power_batt_valued / time_h * average) * 1000;

	pr_info("%s: %i", module_name, icl);
	return icl;
}

static int __init init_charging_controller()
{
	//Just print init message for now
	pr_info("%s: Initialized!", module_name);
	return 0;
}
late_initcall(init_charging_controller);
