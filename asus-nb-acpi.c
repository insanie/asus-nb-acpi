#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/acpi.h>
#include <linux/kobject.h>
#include <linux/platform_data/x86/asus-wmi.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Shiga <insanie@protonmail.com>");
MODULE_DESCRIPTION("Extended ACPI module for ASUS GA402");
MODULE_VERSION("0.2");

#define ASUS_WMI_MGMT_GUID "97845ED0-4E6D-11DE-8A39-0800200C9A66"

static struct kobject *asus_nb_acpi_kobj;

typedef struct {
    u32 device_id;
    unsigned char device_value[16];
} __packed acpi_payload_t;

static void set_asus_wmi_device_value(u32 device_id, unsigned char *value, int value_length)
{
    acpi_payload_t pre_buffer = { .device_id = device_id, .device_value = {0} };
    memcpy(pre_buffer.device_value, value, value_length);

    struct acpi_buffer in_buffer = { (acpi_size) sizeof(pre_buffer), &pre_buffer };
    struct acpi_buffer out_buffer = { ACPI_ALLOCATE_BUFFER, NULL };

    acpi_status status = wmi_evaluate_method(ASUS_WMI_MGMT_GUID, 0, ASUS_WMI_METHODID_DEVS, &in_buffer, &out_buffer);
    printk(KERN_INFO "[asus_nb_acpi] Writing to %x: %s\n", device_id, status == 0 ? "OK" : "ERR");
}

static ssize_t dummy_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "Not readable\n");
}

static ssize_t write_to_parameter(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    u32 device_id = 0x00000000;
    if (strcmp(attr->attr.name, "perfmode") == 0) { device_id = ASUS_WMI_DEVID_THROTTLE_THERMAL_POLICY; }
    else if (strcmp(attr->attr.name, "cpu_fan_curve") == 0) { device_id = ASUS_WMI_DEVID_CPU_FAN_CURVE; }
    else if (strcmp(attr->attr.name, "gpu_fan_curve") == 0) { device_id = ASUS_WMI_DEVID_GPU_FAN_CURVE; }
    else if (strcmp(attr->attr.name, "fast") == 0) { device_id = ASUS_WMI_DEVID_PPT_FPPT; }
    else if (strcmp(attr->attr.name, "slow") == 0) { device_id = ASUS_WMI_DEVID_PPT_PL2_SPPT; }
    else if (strcmp(attr->attr.name, "apu") == 0) { device_id = ASUS_WMI_DEVID_PPT_APU_SPPT; }

    int size = 0;
    for (int i = 0; i < count - 1; ++i) {
        if (buf[i] == ',') {
            size++;
        }
    }
    unsigned char value[16] = {0};
    int i = 0;
    for (int j = 0; j < count - 1; ++j) {
        if (buf[j] != ',') {
            value[i] = (int)value[i] * 10 + (buf[j] - '0');
        } else {
            i++;
        }
    }

    set_asus_wmi_device_value(device_id, value, size + 1);
    return count;
}

static struct kobj_attribute perfmode_attr = __ATTR(perfmode, 0664, dummy_show, write_to_parameter);
static struct kobj_attribute cpu_fan_curve_attr = __ATTR(cpu_fan_curve, 0664, dummy_show, write_to_parameter);
static struct kobj_attribute gpu_fan_curve_attr = __ATTR(gpu_fan_curve, 0664, dummy_show, write_to_parameter);
static struct kobj_attribute fast_attr = __ATTR(fast, 0664, dummy_show, write_to_parameter);
static struct kobj_attribute slow_attr = __ATTR(slow, 0664, dummy_show, write_to_parameter);
static struct kobj_attribute apu_attr = __ATTR(apu, 0664, dummy_show, write_to_parameter);

static int __init asus_nb_acpi_init(void) {
    printk(KERN_INFO "[asus_nb_acpi] Module started\n");

    asus_nb_acpi_kobj = kobject_create_and_add("parameters", &THIS_MODULE->mkobj.kobj);
    int err;
    err = sysfs_create_file(asus_nb_acpi_kobj, &perfmode_attr.attr);
    err = sysfs_create_file(asus_nb_acpi_kobj, &cpu_fan_curve_attr.attr);
    err = sysfs_create_file(asus_nb_acpi_kobj, &gpu_fan_curve_attr.attr);
    err = sysfs_create_file(asus_nb_acpi_kobj, &fast_attr.attr);
    err = sysfs_create_file(asus_nb_acpi_kobj, &slow_attr.attr);
    err = sysfs_create_file(asus_nb_acpi_kobj, &apu_attr.attr);

    return 0;
}

static void __exit asus_nb_acpi_exit(void) {
    sysfs_remove_file(asus_nb_acpi_kobj, &perfmode_attr.attr);
    sysfs_remove_file(asus_nb_acpi_kobj, &cpu_fan_curve_attr.attr);
    sysfs_remove_file(asus_nb_acpi_kobj, &gpu_fan_curve_attr.attr);
    sysfs_remove_file(asus_nb_acpi_kobj, &fast_attr.attr);
    sysfs_remove_file(asus_nb_acpi_kobj, &slow_attr.attr);
    sysfs_remove_file(asus_nb_acpi_kobj, &apu_attr.attr);
    kobject_put(asus_nb_acpi_kobj);

    printk(KERN_INFO "[asus_nb_acpi] Module exited\n");
}

module_init(asus_nb_acpi_init);
module_exit(asus_nb_acpi_exit);
