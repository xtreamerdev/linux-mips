#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/init.h>
#include <linux/node.h>
#include <linux/nodemask.h>
#include <linux/percpu.h>

#ifdef CONFIG_NUMA
static struct node *sysfs_nodes;
#endif
static DEFINE_PER_CPU(struct cpu, cpu_devices);

static int __init topology_init(void)
{
	int i, ret;

#ifdef CONFIG_NUMA
	sysfs_nodes = kzalloc(sizeof(struct node) * MAX_NUMNODES, GFP_KERNEL);
	if (!sysfs_nodes)
		return -ENOMEM;

	for_each_online_node(i)
		register_node(sysfs_nodes + i, i, 0);
#endif /* CONFIG_NUMA */

	for_each_present_cpu(i) {
		ret = register_cpu(&per_cpu(cpu_devices, i), i, NULL);
		if (ret)
			printk(KERN_WARNING "topology_init: register_cpu %d "
			       "failed (%d)\n", i, ret);
	}

	return 0;
}

subsys_initcall(topology_init);
