/*
 * offset.c: Calculate pt_regs and task_struct offsets.
 *
 * Copyright (C) 1996 David S. Miller
 * Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003 Ralf Baechle
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 *
 * Kevin Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000 MIPS Technologies, Inc.
 */
#include <linux/compat.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/kbuild.h>
#include <asm/gdb-stub.h>
#include <asm/ptrace.h>
#include <asm/processor.h>

void output_ptreg_defines(void)
{
	COMMENT("MIPS pt_regs offsets.");
	OFFSET(PT_R0, pt_regs, regs[0]);
	OFFSET(PT_R1, pt_regs, regs[1]);
	OFFSET(PT_R2, pt_regs, regs[2]);
	OFFSET(PT_R3, pt_regs, regs[3]);
	OFFSET(PT_R4, pt_regs, regs[4]);
	OFFSET(PT_R5, pt_regs, regs[5]);
	OFFSET(PT_R6, pt_regs, regs[6]);
	OFFSET(PT_R7, pt_regs, regs[7]);
	OFFSET(PT_R8, pt_regs, regs[8]);
	OFFSET(PT_R9, pt_regs, regs[9]);
	OFFSET(PT_R10, pt_regs, regs[10]);
	OFFSET(PT_R11, pt_regs, regs[11]);
	OFFSET(PT_R12, pt_regs, regs[12]);
	OFFSET(PT_R13, pt_regs, regs[13]);
	OFFSET(PT_R14, pt_regs, regs[14]);
	OFFSET(PT_R15, pt_regs, regs[15]);
	OFFSET(PT_R16, pt_regs, regs[16]);
	OFFSET(PT_R17, pt_regs, regs[17]);
	OFFSET(PT_R18, pt_regs, regs[18]);
	OFFSET(PT_R19, pt_regs, regs[19]);
	OFFSET(PT_R20, pt_regs, regs[20]);
	OFFSET(PT_R21, pt_regs, regs[21]);
	OFFSET(PT_R22, pt_regs, regs[22]);
	OFFSET(PT_R23, pt_regs, regs[23]);
	OFFSET(PT_R24, pt_regs, regs[24]);
	OFFSET(PT_R25, pt_regs, regs[25]);
	OFFSET(PT_R26, pt_regs, regs[26]);
	OFFSET(PT_R27, pt_regs, regs[27]);
	OFFSET(PT_R28, pt_regs, regs[28]);
	OFFSET(PT_R29, pt_regs, regs[29]);
	OFFSET(PT_R30, pt_regs, regs[30]);
	OFFSET(PT_R31, pt_regs, regs[31]);
	OFFSET(PT_LO, pt_regs, lo);
	OFFSET(PT_HI, pt_regs, hi);
#ifdef CONFIG_CPU_HAS_SMARTMIPS
	OFFSET(PT_ACX, pt_regs, acx);
#endif
	OFFSET(PT_EPC, pt_regs, cp0_epc);
	OFFSET(PT_BVADDR, pt_regs, cp0_badvaddr);
	OFFSET(PT_STATUS, pt_regs, cp0_status);
	OFFSET(PT_CAUSE, pt_regs, cp0_cause);
#ifdef CONFIG_MIPS_MT_SMTC
	OFFSET(PT_TCSTATUS, pt_regs, cp0_tcstatus);
#endif /* CONFIG_MIPS_MT_SMTC */
	DEFINE(PT_SIZE, sizeof(struct pt_regs));
	BLANK();
}

void output_task_defines(void)
{
	COMMENT("MIPS task_struct offsets.");
	OFFSET(TASK_STATE, task_struct, state);
	OFFSET(TASK_THREAD_INFO, task_struct, stack);
	OFFSET(TASK_FLAGS, task_struct, flags);
	OFFSET(TASK_MM, task_struct, mm);
	OFFSET(TASK_PID, task_struct, pid);
	DEFINE(TASK_STRUCT_SIZE, sizeof(struct task_struct));
	BLANK();
}

void output_thread_info_defines(void)
{
	COMMENT("MIPS thread_info offsets.");
	OFFSET(TI_TASK, thread_info, task);
	OFFSET(TI_EXEC_DOMAIN, thread_info, exec_domain);
	OFFSET(TI_FLAGS, thread_info, flags);
	OFFSET(TI_TP_VALUE, thread_info, tp_value);
	OFFSET(TI_CPU, thread_info, cpu);
	OFFSET(TI_PRE_COUNT, thread_info, preempt_count);
	OFFSET(TI_ADDR_LIMIT, thread_info, addr_limit);
	OFFSET(TI_RESTART_BLOCK, thread_info, restart_block);
	OFFSET(TI_REGS, thread_info, regs);
	DEFINE(_THREAD_SIZE, THREAD_SIZE);
	DEFINE(_THREAD_MASK, THREAD_MASK);
	BLANK();
}

void output_thread_defines(void)
{
	COMMENT("MIPS specific thread_struct offsets.");
	OFFSET(THREAD_REG16, task_struct, thread.reg16);
	OFFSET(THREAD_REG17, task_struct, thread.reg17);
	OFFSET(THREAD_REG18, task_struct, thread.reg18);
	OFFSET(THREAD_REG19, task_struct, thread.reg19);
	OFFSET(THREAD_REG20, task_struct, thread.reg20);
	OFFSET(THREAD_REG21, task_struct, thread.reg21);
	OFFSET(THREAD_REG22, task_struct, thread.reg22);
	OFFSET(THREAD_REG23, task_struct, thread.reg23);
	OFFSET(THREAD_REG29, task_struct, thread.reg29);
	OFFSET(THREAD_REG30, task_struct, thread.reg30);
	OFFSET(THREAD_REG31, task_struct, thread.reg31);
	OFFSET(THREAD_STATUS, task_struct,
	       thread.cp0_status);
	OFFSET(THREAD_FPU, task_struct, thread.fpu);

	OFFSET(THREAD_BVADDR, task_struct, \
	       thread.cp0_badvaddr);
	OFFSET(THREAD_BUADDR, task_struct, \
	       thread.cp0_baduaddr);
	OFFSET(THREAD_ECODE, task_struct, \
	       thread.error_code);
	OFFSET(THREAD_TRAPNO, task_struct, thread.trap_no);
	OFFSET(THREAD_TRAMP, task_struct, \
	       thread.irix_trampoline);
	OFFSET(THREAD_OLDCTX, task_struct, \
	       thread.irix_oldctx);
	BLANK();
}

void output_thread_fpu_defines(void)
{
	OFFSET(THREAD_FPR0, task_struct, thread.fpu.fpr[0]);
	OFFSET(THREAD_FPR1, task_struct, thread.fpu.fpr[1]);
	OFFSET(THREAD_FPR2, task_struct, thread.fpu.fpr[2]);
	OFFSET(THREAD_FPR3, task_struct, thread.fpu.fpr[3]);
	OFFSET(THREAD_FPR4, task_struct, thread.fpu.fpr[4]);
	OFFSET(THREAD_FPR5, task_struct, thread.fpu.fpr[5]);
	OFFSET(THREAD_FPR6, task_struct, thread.fpu.fpr[6]);
	OFFSET(THREAD_FPR7, task_struct, thread.fpu.fpr[7]);
	OFFSET(THREAD_FPR8, task_struct, thread.fpu.fpr[8]);
	OFFSET(THREAD_FPR9, task_struct, thread.fpu.fpr[9]);
	OFFSET(THREAD_FPR10, task_struct, thread.fpu.fpr[10]);
	OFFSET(THREAD_FPR11, task_struct, thread.fpu.fpr[11]);
	OFFSET(THREAD_FPR12, task_struct, thread.fpu.fpr[12]);
	OFFSET(THREAD_FPR13, task_struct, thread.fpu.fpr[13]);
	OFFSET(THREAD_FPR14, task_struct, thread.fpu.fpr[14]);
	OFFSET(THREAD_FPR15, task_struct, thread.fpu.fpr[15]);
	OFFSET(THREAD_FPR16, task_struct, thread.fpu.fpr[16]);
	OFFSET(THREAD_FPR17, task_struct, thread.fpu.fpr[17]);
	OFFSET(THREAD_FPR18, task_struct, thread.fpu.fpr[18]);
	OFFSET(THREAD_FPR19, task_struct, thread.fpu.fpr[19]);
	OFFSET(THREAD_FPR20, task_struct, thread.fpu.fpr[20]);
	OFFSET(THREAD_FPR21, task_struct, thread.fpu.fpr[21]);
	OFFSET(THREAD_FPR22, task_struct, thread.fpu.fpr[22]);
	OFFSET(THREAD_FPR23, task_struct, thread.fpu.fpr[23]);
	OFFSET(THREAD_FPR24, task_struct, thread.fpu.fpr[24]);
	OFFSET(THREAD_FPR25, task_struct, thread.fpu.fpr[25]);
	OFFSET(THREAD_FPR26, task_struct, thread.fpu.fpr[26]);
	OFFSET(THREAD_FPR27, task_struct, thread.fpu.fpr[27]);
	OFFSET(THREAD_FPR28, task_struct, thread.fpu.fpr[28]);
	OFFSET(THREAD_FPR29, task_struct, thread.fpu.fpr[29]);
	OFFSET(THREAD_FPR30, task_struct, thread.fpu.fpr[30]);
	OFFSET(THREAD_FPR31, task_struct, thread.fpu.fpr[31]);

	OFFSET(THREAD_FCR31, task_struct, thread.fpu.fcr31);
	BLANK();
}

void output_mm_defines(void)
{
	COMMENT("Size of struct page");
	DEFINE(STRUCT_PAGE_SIZE, sizeof(struct page));
	BLANK();
	COMMENT("Linux mm_struct offsets.");
	OFFSET(MM_USERS, mm_struct, mm_users);
	OFFSET(MM_PGD, mm_struct, pgd);
	OFFSET(MM_CONTEXT, mm_struct, context);
	BLANK();
	DEFINE(_PAGE_SIZE, PAGE_SIZE);
	DEFINE(_PAGE_SHIFT, PAGE_SHIFT);
	BLANK();
	DEFINE(_PGD_T_SIZE, sizeof(pgd_t));
	DEFINE(_PMD_T_SIZE, sizeof(pmd_t));
	DEFINE(_PTE_T_SIZE, sizeof(pte_t));
	BLANK();
	DEFINE(_PGD_T_LOG2, PGD_T_LOG2);
	DEFINE(_PMD_T_LOG2, PMD_T_LOG2);
	DEFINE(_PTE_T_LOG2, PTE_T_LOG2);
	BLANK();
	DEFINE(_PGD_ORDER, PGD_ORDER);
	DEFINE(_PMD_ORDER, PMD_ORDER);
	DEFINE(_PTE_ORDER, PTE_ORDER);
	BLANK();
	DEFINE(_PMD_SHIFT, PMD_SHIFT);
	DEFINE(_PGDIR_SHIFT, PGDIR_SHIFT);
	BLANK();
	DEFINE(_PTRS_PER_PGD, PTRS_PER_PGD);
	DEFINE(_PTRS_PER_PMD, PTRS_PER_PMD);
	DEFINE(_PTRS_PER_PTE, PTRS_PER_PTE);
	BLANK();
}

#ifdef CONFIG_32BIT
void output_sc_defines(void)
{
	COMMENT("Linux sigcontext offsets.");
	OFFSET(SC_REGS, sigcontext, sc_regs);
	OFFSET(SC_FPREGS, sigcontext, sc_fpregs);
	OFFSET(SC_ACX, sigcontext, sc_acx);
	OFFSET(SC_MDHI, sigcontext, sc_mdhi);
	OFFSET(SC_MDLO, sigcontext, sc_mdlo);
	OFFSET(SC_PC, sigcontext, sc_pc);
	OFFSET(SC_FPC_CSR, sigcontext, sc_fpc_csr);
	OFFSET(SC_FPC_EIR, sigcontext, sc_fpc_eir);
	OFFSET(SC_HI1, sigcontext, sc_hi1);
	OFFSET(SC_LO1, sigcontext, sc_lo1);
	OFFSET(SC_HI2, sigcontext, sc_hi2);
	OFFSET(SC_LO2, sigcontext, sc_lo2);
	OFFSET(SC_HI3, sigcontext, sc_hi3);
	OFFSET(SC_LO3, sigcontext, sc_lo3);
	BLANK();
}
#endif

#ifdef CONFIG_64BIT
void output_sc_defines(void)
{
	COMMENT("Linux sigcontext offsets.");
	OFFSET(SC_REGS, sigcontext, sc_regs);
	OFFSET(SC_FPREGS, sigcontext, sc_fpregs);
	OFFSET(SC_MDHI, sigcontext, sc_mdhi);
	OFFSET(SC_MDLO, sigcontext, sc_mdlo);
	OFFSET(SC_PC, sigcontext, sc_pc);
	OFFSET(SC_FPC_CSR, sigcontext, sc_fpc_csr);
	BLANK();
}
#endif

#ifdef CONFIG_MIPS32_COMPAT
void output_sc32_defines(void)
{
	COMMENT("Linux 32-bit sigcontext offsets.");
	OFFSET(SC32_FPREGS, sigcontext32, sc_fpregs);
	OFFSET(SC32_FPC_CSR, sigcontext32, sc_fpc_csr);
	OFFSET(SC32_FPC_EIR, sigcontext32, sc_fpc_eir);
	BLANK();
}
#endif

void output_signal_defined(void)
{
	COMMENT("Linux signal numbers.");
	DEFINE(_SIGHUP, SIGHUP);
	DEFINE(_SIGINT, SIGINT);
	DEFINE(_SIGQUIT, SIGQUIT);
	DEFINE(_SIGILL, SIGILL);
	DEFINE(_SIGTRAP, SIGTRAP);
	DEFINE(_SIGIOT, SIGIOT);
	DEFINE(_SIGABRT, SIGABRT);
	DEFINE(_SIGEMT, SIGEMT);
	DEFINE(_SIGFPE, SIGFPE);
	DEFINE(_SIGKILL, SIGKILL);
	DEFINE(_SIGBUS, SIGBUS);
	DEFINE(_SIGSEGV, SIGSEGV);
	DEFINE(_SIGSYS, SIGSYS);
	DEFINE(_SIGPIPE, SIGPIPE);
	DEFINE(_SIGALRM, SIGALRM);
	DEFINE(_SIGTERM, SIGTERM);
	DEFINE(_SIGUSR1, SIGUSR1);
	DEFINE(_SIGUSR2, SIGUSR2);
	DEFINE(_SIGCHLD, SIGCHLD);
	DEFINE(_SIGPWR, SIGPWR);
	DEFINE(_SIGWINCH, SIGWINCH);
	DEFINE(_SIGURG, SIGURG);
	DEFINE(_SIGIO, SIGIO);
	DEFINE(_SIGSTOP, SIGSTOP);
	DEFINE(_SIGTSTP, SIGTSTP);
	DEFINE(_SIGCONT, SIGCONT);
	DEFINE(_SIGTTIN, SIGTTIN);
	DEFINE(_SIGTTOU, SIGTTOU);
	DEFINE(_SIGVTALRM, SIGVTALRM);
	DEFINE(_SIGPROF, SIGPROF);
	DEFINE(_SIGXCPU, SIGXCPU);
	DEFINE(_SIGXFSZ, SIGXFSZ);
	BLANK();
}

void output_irq_cpustat_t_defines(void)
{
	COMMENT("Linux irq_cpustat_t offsets.");
	DEFINE(IC_SOFTIRQ_PENDING,
			offsetof(irq_cpustat_t, __softirq_pending));
	DEFINE(IC_IRQ_CPUSTAT_T, sizeof(irq_cpustat_t));
	BLANK();
}

void output_gdbreg_defines(void)
{
	COMMENT("MIPS struct gdb_regs offsets.");
	OFFSET(GDB_FR_REG0, gdb_regs, reg0);
	OFFSET(GDB_FR_REG1, gdb_regs, reg1);
	OFFSET(GDB_FR_REG2, gdb_regs, reg2);
	OFFSET(GDB_FR_REG3, gdb_regs, reg3);
	OFFSET(GDB_FR_REG4, gdb_regs, reg4);
	OFFSET(GDB_FR_REG5, gdb_regs, reg5);
	OFFSET(GDB_FR_REG6, gdb_regs, reg6);
	OFFSET(GDB_FR_REG7, gdb_regs, reg7);
	OFFSET(GDB_FR_REG8, gdb_regs, reg8);
	OFFSET(GDB_FR_REG9, gdb_regs, reg9);
	OFFSET(GDB_FR_REG10, gdb_regs, reg10);
	OFFSET(GDB_FR_REG11, gdb_regs, reg11);
	OFFSET(GDB_FR_REG12, gdb_regs, reg12);
	OFFSET(GDB_FR_REG13, gdb_regs, reg13);
	OFFSET(GDB_FR_REG14, gdb_regs, reg14);
	OFFSET(GDB_FR_REG15, gdb_regs, reg15);
	OFFSET(GDB_FR_REG16, gdb_regs, reg16);
	OFFSET(GDB_FR_REG17, gdb_regs, reg17);
	OFFSET(GDB_FR_REG18, gdb_regs, reg18);
	OFFSET(GDB_FR_REG19, gdb_regs, reg19);
	OFFSET(GDB_FR_REG20, gdb_regs, reg20);
	OFFSET(GDB_FR_REG21, gdb_regs, reg21);
	OFFSET(GDB_FR_REG22, gdb_regs, reg22);
	OFFSET(GDB_FR_REG23, gdb_regs, reg23);
	OFFSET(GDB_FR_REG24, gdb_regs, reg24);
	OFFSET(GDB_FR_REG25, gdb_regs, reg25);
	OFFSET(GDB_FR_REG26, gdb_regs, reg26);
	OFFSET(GDB_FR_REG27, gdb_regs, reg27);
	OFFSET(GDB_FR_REG28, gdb_regs, reg28);
	OFFSET(GDB_FR_REG29, gdb_regs, reg29);
	OFFSET(GDB_FR_REG30, gdb_regs, reg30);
	OFFSET(GDB_FR_REG31, gdb_regs, reg31);
	BLANK();

	OFFSET(GDB_FR_STATUS, gdb_regs, cp0_status);
	OFFSET(GDB_FR_HI, gdb_regs, hi);
	OFFSET(GDB_FR_LO, gdb_regs, lo);
#ifdef CONFIG_CPU_HAS_SMARTMIPS
	OFFSET(GDB_FR_ACX, gdb_regs, acx);
#endif
	OFFSET(GDB_FR_BADVADDR, gdb_regs, cp0_badvaddr);
	OFFSET(GDB_FR_CAUSE, gdb_regs, cp0_cause);
	OFFSET(GDB_FR_EPC, gdb_regs, cp0_epc);
	BLANK();

	OFFSET(GDB_FR_FPR0, gdb_regs, fpr0);
	OFFSET(GDB_FR_FPR1, gdb_regs, fpr1);
	OFFSET(GDB_FR_FPR2, gdb_regs, fpr2);
	OFFSET(GDB_FR_FPR3, gdb_regs, fpr3);
	OFFSET(GDB_FR_FPR4, gdb_regs, fpr4);
	OFFSET(GDB_FR_FPR5, gdb_regs, fpr5);
	OFFSET(GDB_FR_FPR6, gdb_regs, fpr6);
	OFFSET(GDB_FR_FPR7, gdb_regs, fpr7);
	OFFSET(GDB_FR_FPR8, gdb_regs, fpr8);
	OFFSET(GDB_FR_FPR9, gdb_regs, fpr9);
	OFFSET(GDB_FR_FPR10, gdb_regs, fpr10);
	OFFSET(GDB_FR_FPR11, gdb_regs, fpr11);
	OFFSET(GDB_FR_FPR12, gdb_regs, fpr12);
	OFFSET(GDB_FR_FPR13, gdb_regs, fpr13);
	OFFSET(GDB_FR_FPR14, gdb_regs, fpr14);
	OFFSET(GDB_FR_FPR15, gdb_regs, fpr15);
	OFFSET(GDB_FR_FPR16, gdb_regs, fpr16);
	OFFSET(GDB_FR_FPR17, gdb_regs, fpr17);
	OFFSET(GDB_FR_FPR18, gdb_regs, fpr18);
	OFFSET(GDB_FR_FPR19, gdb_regs, fpr19);
	OFFSET(GDB_FR_FPR20, gdb_regs, fpr20);
	OFFSET(GDB_FR_FPR21, gdb_regs, fpr21);
	OFFSET(GDB_FR_FPR22, gdb_regs, fpr22);
	OFFSET(GDB_FR_FPR23, gdb_regs, fpr23);
	OFFSET(GDB_FR_FPR24, gdb_regs, fpr24);
	OFFSET(GDB_FR_FPR25, gdb_regs, fpr25);
	OFFSET(GDB_FR_FPR26, gdb_regs, fpr26);
	OFFSET(GDB_FR_FPR27, gdb_regs, fpr27);
	OFFSET(GDB_FR_FPR28, gdb_regs, fpr28);
	OFFSET(GDB_FR_FPR29, gdb_regs, fpr29);
	OFFSET(GDB_FR_FPR30, gdb_regs, fpr30);
	OFFSET(GDB_FR_FPR31, gdb_regs, fpr31);
	BLANK();

	OFFSET(GDB_FR_FSR, gdb_regs, cp1_fsr);
	OFFSET(GDB_FR_FIR, gdb_regs, cp1_fir);
	OFFSET(GDB_FR_FRP, gdb_regs, frame_ptr);
	OFFSET(GDB_FR_DUMMY, gdb_regs, dummy);

	OFFSET(GDB_FR_CP0_INDEX, gdb_regs, cp0_index);
	OFFSET(GDB_FR_CP0_RANDOM, gdb_regs, cp0_random);
	OFFSET(GDB_FR_CP0_ENTRYLO0, gdb_regs, cp0_entrylo0);
	OFFSET(GDB_FR_CP0_ENTRYLO1, gdb_regs, cp0_entrylo1);
	OFFSET(GDB_FR_CP0_CONTEXT, gdb_regs, cp0_context);
	OFFSET(GDB_FR_CP0_PAGEMASK, gdb_regs, cp0_pagemask);
	OFFSET(GDB_FR_CP0_WIRED, gdb_regs, cp0_wired);
	OFFSET(GDB_FR_CP0_REG7, gdb_regs, cp0_reg7);
	OFFSET(GDB_FR_CP0_REG8, gdb_regs, cp0_reg8);
	OFFSET(GDB_FR_CP0_REG9, gdb_regs, cp0_reg9);
	OFFSET(GDB_FR_CP0_ENTRYHI, gdb_regs, cp0_entryhi);
	OFFSET(GDB_FR_CP0_REG11, gdb_regs, cp0_reg11);
	OFFSET(GDB_FR_CP0_REG12, gdb_regs, cp0_reg12);
	OFFSET(GDB_FR_CP0_REG13, gdb_regs, cp0_reg13);
	OFFSET(GDB_FR_CP0_REG14, gdb_regs, cp0_reg14);
	OFFSET(GDB_FR_CP0_PRID, gdb_regs, cp0_prid);
	DEFINE(GDB_FR_SIZE, sizeof(struct gdb_regs));
}
