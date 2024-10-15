# asus-nb-acpi

A custom kernel module to extend ACPI control over ASUS ROG/TUF laptops on Linux. Allows for precise control over power limits,
fan curves, and performance mode via sysfs. Shipped with optional automation to interact with exposed char devices in sysfs.

## Compatibility

Initially only GA402 is supported but this approach may be applied to other ROG/TUF devices using AMD APU+dGPU.
You can play around with different device codes from linux kernel in Credits section. While fans and perfmode devices remain the same
for all ROG/TUF laptops, power limits devices can vary across AMD/Intel/Nvidia hardware in these laptops.

## Disclaimer

While the module doesn't do anything harmful itself, it exposes parameters that can be used to hurt you device. If you try to pass some
ridiculous value to any of those char devices, there's absolutely no guarantee on what will happen next. Please, be mindful with
selecting values for them. There are some thresholds for these parameters within laptop's embedded controller, so you won't brick
your device. But you may reach emergency overheating restart that can break you OS if you're not careful.

## Requirements
* Probably any linux kernel, but let's say 5.0+ to be on the safe side + its headers
* `gcc` (may be `clang` will work too)
* ROG/TUF (may be other ASUS?) laptop for basic fan curve and performance mode functionality
* AMD APU for power limits functionality (again, see linux kernel for device codes for other platforms)
* No other external software that overrides the same parameters, e.g. `asusctl`

## Installation

### Option 1: manual

In this case you'll have to recompile the module *each time* you install new kernel.
1. Clone the repo.
2. `make` or `make compile` to compile the module for your current kernel.
3. (Optional) Sign the module (`asus-nb-acpi.ko`) before installation if SecureBoot is used.
4. `sudo make install` to install the module. You're free to interact with values in `/sys/module/asus_nb_acpi/parameters/` now.
Use `sudo make uninstall` to uninstall the module if needed.
5. (Optional) `sudo make install-ctl` to install a set of systemd service, udev rule, and shell script that aim to automate parameters
application. The script will be triggered on boot, on every power status change (battery->AC and back), and on waking up from sleep. The
config for it is stored in `/etc/asus-nb-acpi-ctl.conf`. Similarly to the module installation, use `sudo make uninstall-ctl` to remove
the solution.


### Option 2: DKMS

You can leverage DKMS to automate module recompiling every time you install new kernel.
1. Clone the repo.
2. Make sure DKMS is installed in your system.
3. `sudo make install-dkms` to add the sources to DKMS, build and install the module. You're free to interact with values in
`/sys/module/asus_nb_acpi/parameters/` now. Use `sudo make uninstall-dkms` to uninstall the module if needed.
4. (Optional) `sudo make install-ctl` to install a set of systemd service, udev rule, and shell script that aim to automate parameters
application. The script will be triggered on boot, on every power status change (battery->AC and back), and on waking up from sleep. The
config for it is stored in `/etc/asus-nb-acpi-ctl.conf`. Similarly to the DKMS installation, use `sudo make uninstall-ctl` to remove the
solution.


## Credits

* Linux kernel, device codes specifically -
https://github.com/torvalds/linux/blob/047371968ffc470769f541d6933e262dc7085456/include/linux/platform_data/x86/asus-wmi.h#L94
