/*
 * Malta Platform-specific hooks for SMP operation
 */
#include <linux/init.h>

#include <asm/mips-boards/maltasmp.h>

/*
 * Cause the specified action to be performed on a targeted "CPU"
 */

void core_send_ipi(int cpu, unsigned int action)
{
	if (malta_smtc)
		smtc_send_ipi(cpu, LINUX_SMP_IPI, action);
	else if (malta_cmp)
		cmp_send_ipi(cpu, action);
	else if (malta_smvp)
		smvp_send_ipi(cpu, action);
}

/*
 * Platform "CPU" startup hook
 */

void prom_boot_secondary(int cpu, struct task_struct *idle)
{
	if (malta_smtc)
		smtc_boot_secondary(cpu, idle);
	else if (malta_cmp)
		cmp_boot_secondary(cpu, idle);
	else if (malta_smvp)
		smvp_boot_secondary(cpu, idle);
}

/*
 * Post-config but pre-boot cleanup entry point
 */
void prom_init_secondary(void)
{
	if (malta_smtc) {
		void smtc_init_secondary(void);
		int myvpe;

		/* Don't enable Malta I/O interrupts (IP2) for secondary VPEs */
		myvpe = read_c0_tcbind() & TCBIND_CURVPE;
		if (myvpe != 0) {
			/* Ideally, this should be done only once per VPE, but... */
			clear_c0_status(STATUSF_IP2);
			set_c0_status(STATUSF_IP0 | STATUSF_IP1 | STATUSF_IP3
				      | STATUSF_IP4 | STATUSF_IP5 | STATUSF_IP6
				      | STATUSF_IP7);
		}
		smtc_init_secondary();
	}
	else if (malta_cmp) {
#if defined(USE_GIC)
		write_c0_status((read_c0_status() & ~ST0_IM ) |
				(STATUSF_IP3 | STATUSF_IP4 | STATUSF_IP6 | STATUSF_IP7));
#else
		write_c0_status((read_c0_status() & ~ST0_IM ) |
				(STATUSF_IP0 | STATUSF_IP1 | STATUSF_IP6 | STATUSF_IP7));
#endif
		cmp_init_secondary();
	}
	else if (malta_smvp) {
#if defined(USE_GIC)
		write_c0_status((read_c0_status() & ~ST0_IM ) |
				(STATUSF_IP3 | STATUSF_IP4 | STATUSF_IP6 | STATUSF_IP7));
#else
		write_c0_status((read_c0_status() & ~ST0_IM ) |
				(STATUSF_IP0 | STATUSF_IP1 | STATUSF_IP6 | STATUSF_IP7));
#endif
		smvp_init_secondary();
	}
}

/*
 * Platform SMP pre-initialization
 *
 * As noted above, we can assume a single CPU for now
 * but it may be multithreaded.
 */

void __init plat_smp_setup(void)
{
	if (malta_smtc) {
		if (read_c0_config3() & (1<<2))
			mipsmt_build_cpu_map(0);
	}
	else if (malta_cmp)
		cmp_smp_setup();
	else if (malta_smvp)
		smvp_smp_setup();
}

void __init plat_prepare_cpus(unsigned int max_cpus)
{
	if (malta_smtc) {
		if (read_c0_config3() & (1<<2))
			mipsmt_prepare_cpus();
	}
	else if (malta_cmp) {
		cmp_prepare_cpus(max_cpus);
	}
	else if (malta_smvp)
		smvp_prepare_cpus(max_cpus);
}

/*
 * SMP initialization finalization entry point
 */

void prom_smp_finish(void)
{
	if (malta_smtc)
		smtc_smp_finish();
	else if (malta_cmp)
		cmp_smp_finish();
	else if (malta_smvp)
		smvp_smp_finish();
}

/*
 * Hook for after all CPUs are online
 */

void prom_cpus_done(void)
{
	if (malta_smtc)
		smtc_cpus_done();
	else if (malta_cmp)
		cmp_cpus_done();
	else if (malta_smvp)
		smvp_cpus_done();
}
