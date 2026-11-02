// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sevil Medgitova");
MODULE_DESCRIPTION("Hello World kernel module with timestamps");

static unsigned int count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of Hello, world! messages");

struct hello_event {
	struct list_head list;
	ktime_t timestamp;
};

static LIST_HEAD(hello_list);

static void clear_hello_list(void)
{
	struct hello_event *event, *tmp;

	list_for_each_entry_safe(event, tmp, &hello_list, list) {
		pr_info("Hello timestamp: %lld ns\n",
			(long long)ktime_to_ns(event->timestamp));
		list_del(&event->list);
		kfree(event);
	}
}

static int __init hello_init(void)
{
	struct hello_event *event;
	unsigned int i;

	if (count > 10) {
		pr_err("Count must not exceed 10\n");
		return -EINVAL;
	}

	if (count == 0 || (count >= 5 && count <= 10))
		pr_warn("Unusual count value: %u\n", count);

	for (i = 0; i < count; i++) {
		event = kmalloc(sizeof(*event), GFP_KERNEL);
		if (!event) {
			clear_hello_list();
			return -ENOMEM;
		}

		event->timestamp = ktime_get();
		list_add_tail(&event->list, &hello_list);
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	clear_hello_list();
	pr_info("Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);


