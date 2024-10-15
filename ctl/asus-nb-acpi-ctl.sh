#!/bin/bash

source /etc/asus-nb-acpi-ctl.conf

if [[ $(cat /sys/class/power_supply/AC0/online) == "1" ]]
then
  PERFMODE=$PERFMODE_AC
  CURVE=$CURVE_AC
  FAST=$FAST_AC
  SLOW=$SLOW_AC
  APU=$APU_AC
else
  PERFMODE=$PERFMODE_DC
  CURVE=$CURVE_DC
  FAST=$FAST_DC
  SLOW=$SLOW_DC
  APU=$APU_DC
fi

{
  sleep 1
  echo $PERFMODE > /sys/module/asus_nb_acpi/parameters/perfmode
  echo $CURVE > /sys/module/asus_nb_acpi/parameters/cpu_fan_curve
  echo $CURVE > /sys/module/asus_nb_acpi/parameters/gpu_fan_curve
  echo $FAST > /sys/module/asus_nb_acpi/parameters/fast
  echo $SLOW > /sys/module/asus_nb_acpi/parameters/slow
  echo $APU > /sys/module/asus_nb_acpi/parameters/apu
} &
