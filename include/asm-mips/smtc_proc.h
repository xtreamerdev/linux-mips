/*
 * Definitions for SMTC /proc entries
 * Copyright(C) 2005 MIPS Technologies Inc.
 */

/*
 * per-"CPU" statistics
 */

struct smtc_cpu_proc {
	unsigned long timerints;
	unsigned long selfipis;
};

extern struct smtc_cpu_proc smtc_cpu_stats[NR_CPUS];

/* Count of number of recoveries of "stolen" FPU access rights on 34K */

extern atomic_t smtc_fpu_recoveries;
