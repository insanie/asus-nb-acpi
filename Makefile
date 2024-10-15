obj-m += asus-nb-acpi.o
module_version := 0.2

compile:
	make -C /lib/modules/$$(uname -r)/build M=$$(pwd) modules

install: asus-nb-acpi.ko
	mkdir -p /lib/modules/$$(uname -r)/updates/
	cp asus-nb-acpi.ko /lib/modules/$$(uname -r)/updates/asus-nb-acpi.ko
	depmod
	modprobe asus_nb_acpi
	echo "asus_nb_acpi" > /etc/modules-load.d/asus-nb-acpi.conf

uninstall:
	rmmod asus_nb_acpi
	rm -f /etc/modules-load.d/asus-nb-acpi.conf
	rm /lib/modules/$$(uname -r)/updates/asus-nb-acpi.ko
	depmod

install-dkms: asus-nb-acpi.ko
	mkdir -p /usr/src/asus-nb-acpi-$(module_version)
	cp -R $$(pwd)/* /usr/src/asus-nb-acpi-$(module_version)/
	dkms add -m asus-nb-acpi -v $(module_version)
	dkms build -m asus-nb-acpi -v $(module_version)
	dkms install -m asus-nb-acpi -v $(module_version)
	echo "asus_nb_acpi" > /etc/modules-load.d/asus-nb-acpi.conf

uninstall-dkms:
	dkms remove -m asus-nb-acpi -v $(module_version)
	rm -rf /usr/src/asus-nb-acpi-$(module_version)
	rm -f /etc/modules-load.d/asus-nb-acpi.conf

install-ctl:
	cp $$(pwd)/ctl/asus-nb-acpi-ctl.conf /etc/asus-nb-acpi-ctl.conf
	cp $$(pwd)/ctl/asus-nb-acpi-ctl.sh /usr/local/sbin/asus-nb-acpi-ctl.sh
	chmod 755 /usr/local/sbin/asus-nb-acpi-ctl.sh
	cp $$(pwd)/ctl/asus-nb-acpi-d.service /etc/systemd/system/asus-nb-acpi-d.service
	systemctl daemon-reload
	systemctl enable asus-nb-acpi-d.service
	cp $$(pwd)/ctl/90-asus-nb-acpi.rules /etc/udev/rules.d/90-asus-nb-acpi.rules
	udevadm control --reload-rules
	cp $$(pwd)/ctl/asus-nb-acpi-d_sleep-hook /lib/systemd/system-sleep/asus-nb-acpi-d_sleep-hook
	chmod 755 /lib/systemd/system-sleep/asus-nb-acpi-d_sleep-hook

uninstall-ctl:
	rm -f /lib/systemd/system-sleep/asus-nb-acpi-d_sleep-hook
	rm -f /etc/udev/rules.d/90-asus-nb-acpi.rules
	udevadm control --reload-rules
	rm -f /etc/systemd/system/asus-nb-acpi-d.service
	systemctl daemon-reload
	rm -f /usr/local/sbin/asus-nb-acpi-ctl.sh
	rm -f /etc/asus-nb-acpi-ctl.conf

clean:
	make -C /lib/modules/$$(uname -r)/build M=$$(pwd) clean
