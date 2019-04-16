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
 *
 * v3 - Completely rework the module and use a timer to recalculate again after some time (this will give more accurate results)
 */

#define module_name "night_charge"

#include <linux/moduleparam.h>
#include <linux/timer.h>
#include "night_charge.h"

static unsigned int active = 0; //0 = off, 1 = on
static unsigned int time_h = 6;
static unsigned int charger_voltage = 5; //Use this to calculate charging time

module_param(active, uint, 0644);
module_param(time_h, uint, 0644);
module_param(charger_voltage, uint, 0644);

static struct timer_list recalc_timer;
static unsigned int cap_batt_now;
static unsigned int charge_till;
static unsigned int counter = 0;

//Calculates night mode icl
void calc_icl(unsigned long data)
{
	int left_time = time_h - counter;
	if(left_time <= 0){
		return;
	}
	counter = counter + 1;
	int needed_capacity, icl;
	needed_capacity = 4000 * (charge_till - cap_batt_now) / 100; //Calculate left capacity to 80%
	pr_info("%s: needed_capacity = %i", module_name, needed_capacity);
	pr_info("%s: charger_voltage = %i", module_name, charger_voltage);
	pr_info("%s: left_time_h = %i", module_name, left_time);
	//Calculate icl, print and return it
	//Since I am using average values i can leave out voltages completely (Eventually will reintroduce them later)
	icl = ( (needed_capacity * 405 / 100) / (left_time * charger_voltage) ) * 1000;
	pr_info("%s: %i", module_name, icl);
	custom_icl = icl;
	mod_timer(&recalc_timer, jiffies + msecs_to_jiffies(3600000));
}

void load_timer(void){
	//Setup and start timer to refresh charging rate every hour
        setup_timer(&recalc_timer, calc_icl, 0);
        mod_timer(&recalc_timer, jiffies + msecs_to_jiffies(3600000));
        calc_icl(0);
}

//Reset values on unplug charger
void reset_values(void){
	counter = 0;
	custom_icl = 0;
	//delete timer so it starts new cycle on replug
	del_timer(&recalc_timer);
}

extern void updateBatteryStats(unsigned int cap_battery_now){
	cap_batt_now = cap_battery_now;
}

//Gets called from outside and selects by mode
void calculate_max_current(unsigned int cap_battery_now, unsigned int charge_till_cap)
{
        //Fallback if capacity is >= 100
        if(cap_battery_now >= 100){
                return;
        }

	if(time_h < 3)
		time_h = 3;
	else if(time_h > 8)
		time_h = 8;

	if(charge_till_cap >= 101)
		charge_till_cap = 100;

        if(active == 1 && !timer_pending(&recalc_timer)){
                cap_batt_now = cap_battery_now;
		charge_till = charge_till_cap;
		load_timer();
                return;
        }

        custom_icl = 0;
}

static int __init init_night_charge()
{
	//Just print init message for now
	pr_info("%s: Initialized!", module_name);
	return 0;
}
late_initcall(init_night_charge);
