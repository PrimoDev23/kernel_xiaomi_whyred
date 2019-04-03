#define module_name "charging_controller"

#include <linux/moduleparam.h>
#include <linux/state_notifier.h>
#include "charging_controller.h"

static unsigned int mode = 0; //0 = off, 1 = night mode, 2 = fast charge (not there yet)
static unsigned int time_h = 6;

module_param(mode, uint, 0644);
module_param(time_h, uint, 0644);

//Gets called from outside and selects by mode
extern int calculate_max_current(unsigned int cap_battery_now, unsigned int charge_till_cap)
{

	pr_info("%s: %i", module_name, mode);

	//Fallback if capacity is above 100 (seems not to happen often but can)
	if(cap_battery_now > 100 || charge_till_cap > 100)
		return 0;

	//Mode 1 = Night mode
	if(mode == 1){
		pr_info("%s: calculating with night mode", module_name);
		return night_mode_icl(cap_battery_now, charge_till_cap);
	}

	return 0;
}

//Calculates night mode icl
extern int night_mode_icl(unsigned int cap_battery_now, unsigned int charge_till_cap)
{
	//Don't let the user set values below 4 and above 8 for time in hours
	if (time_h > 6)
		time_h = 6;
	else if (time_h < 4)
		time_h = 4;
	int needed_capacity = 4000 * (charge_till_cap - cap_battery_now) / 100; //Calculate left capacity to 80%
	pr_info("%s: needed_capacity = %i", module_name, needed_capacity);
	//Calculate icl, print and return it
	//Since I am using average values i can leave out voltages completely (Eventually will reintroduce them later)
	int icl = (needed_capacity / time_h) * 1000;

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
