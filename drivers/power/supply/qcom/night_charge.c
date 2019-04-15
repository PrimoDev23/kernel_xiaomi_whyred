/*
 * Copyright (C) 2019, PrimoDev23 <lexx.ps2711@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * v1 - First Introduction of night mode
 *
 * v2 - Correct calculation and fix issue that reboot needed to get back normal charging 
 */

#define module_name "night_charge"

#include <linux/moduleparam.h>
#include <linux/time.h>
#include "night_charge.h"

static unsigned bool active = true; //0 = off, 1 = night mode, 2 = fast charge (not there yet)
static unsigned int time_h = 6;
static unsigned int charger_voltage = 5; //Use this to calculate charging time
static int last_check_s = 0;
static int left_time_s = 0;

module_param(active, bool, 0644);
module_param(time_h, uint, 0644);
module_param(charger_voltage, uint, 0644);

//Gets called from outside and selects by mode
extern int calculate_max_current(unsigned int cap_battery_now, unsigned int charge_till_cap)
{
	//Fallback if capacity is >= 100
	if(cap_battery_now >= 100 || charge_till_cap >= 100)
		return 0;

	if(active){
		//Don't let the user set values below 4 and above 8 for time in hours
	        if (time_h > 6)
        	        time_h = 6;
	        else if (time_h < 4)
        	        time_h = 4;
		struct timespec ts;
		getnstimeofday(*ts);
		//Started charging
		if(left_time_s == 0){
			left_time_s = time_h * 3600;
		}
		//Fallback for crossing midnight
		if((last_check_s - 86400 + 7200) > 0 && (last_check_s - 86400 + 7200) < ts.tv_sec){
			left_time_s = left_time_s - (86400 - last_check_s + ts.tv_sec);
		}
		else if(last_check_s + 7200 < ts.tv_sec) //If not crossing midnight just calculate left time
		{
			left_time_ms = left_time_s - (ts.tv_sec - last_check_s);
		}
		else //If not 2h over don't recalculate
			return 0;
		pr_info("%s: calculating with night mode", module_name);
		return night_mode_icl(cap_battery_now, charge_till_cap);
	}

	return 0;
}

//Calculates night mode icl
extern int night_mode_icl(unsigned int cap_battery_now, unsigned int charge_till_cap)
{
	int needed_capacity, icl, left_time_h;
	left_time_h = left_time_s / 3600;
	needed_capacity = 4000 * (charge_till_cap - cap_battery_now) / 100; //Calculate left capacity to 80%
	pr_info("%s: needed_capacity = %i", module_name, needed_capacity);
	pr_info("%s: charger_voltage = %i", module_name, charger_voltage);
	pr_info("%s: left_time_h = %i", module_name, left_time_h);
	//Calculate icl, print and return it
	//Since I am using average values i can leave out voltages completely (Eventually will reintroduce them later)
	icl = ( (needed_capacity * 405 / 100) / (left_time_h * charger_voltage) ) * 1000;
	pr_info("%s: %i", module_name, icl);
	return icl;
}

//Reset values on unplug charger
extern void reset_values(){
	left_time_s = 0;
	last_check_s = 0;
}

static int __init init_night_charge()
{
	//Just print init message for now
	pr_info("%s: Initialized!", module_name);
	return 0;
}
late_initcall(init_night_charge);
