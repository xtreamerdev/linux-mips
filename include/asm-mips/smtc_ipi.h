/*
 * Definitions used in MIPS MT SMTC "Interprocessor Interrupt" code.
 */

//#define SMTC_IPI_DEBUG

#ifdef SMTC_IPI_DEBUG
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#endif /* SMTC_IPI_DEBUG */

/*
 * An IPI "message"
 */

typedef struct smtc_ipi {
	struct smtc_ipi *flink;
	int type;
	void *arg;
	int dest;
#ifdef	SMTC_IPI_DEBUG
	int sender;
	long stamp;
#endif /* SMTC_IPI_DEBUG */
} smtc_ipi;

/*
 * Defined IPI Types
 */

#define LINUX_SMP_IPI 1
#define SMTC_CLOCK_TICK 2

/*
 * A queue of IPI messages
 */

typedef struct smtc_ipi_q {
	smtc_ipi *head;
	spinlock_t lock;
	smtc_ipi *tail;
	int depth;
} smtc_ipi_q;

extern smtc_ipi_q IPIQ[NR_CPUS];
extern smtc_ipi_q freeIPIq;

static inline void smtc_ipi_nq(smtc_ipi_q *q, smtc_ipi *p)
{
	long flags;

	spin_lock_irqsave(&q->lock, flags);
	if (q->head == NULL)
		q->head = q->tail = p;
	else
		q->tail->flink = p;
	p->flink = NULL;
	q->tail = p;
	q->depth++;
#ifdef	SMTC_IPI_DEBUG
	p->sender = read_c0_tcbind();
	p->stamp = read_c0_count();
#endif /* SMTC_IPI_DEBUG */
	spin_unlock_irqrestore(&q->lock, flags);
}

static inline smtc_ipi *smtc_ipi_dq(smtc_ipi_q *q)
{
	smtc_ipi *p;
	long flags;

	spin_lock_irqsave(&q->lock, flags);
	if (q->head == NULL)
		p = NULL;
	else {
		p = q->head;
		q->head = q->head->flink;
		q->depth--;
		/* Arguably unnecessary, but leaves queue cleaner */
		if (q->head == NULL)
			q->tail = NULL;
	}
	spin_unlock_irqrestore(&q->lock, flags);
	return p;
}

static inline void smtc_ipi_req(smtc_ipi_q *q, smtc_ipi *p)
{
	long flags;

	spin_lock_irqsave(&q->lock, flags);
	if (q->head == NULL) {
		q->head = q->tail = p;
		p->flink = NULL;
	} else {
		p->flink = q->head;
		q->head = p;
	}
	q->depth++;
	spin_unlock_irqrestore(&q->lock, flags);
}

static inline int smtc_ipi_qdepth(smtc_ipi_q *q)
{
	long flags;
	int retval;

	spin_lock_irqsave(&q->lock, flags);
	retval = q->depth;
	spin_unlock_irqrestore(&q->lock, flags);
	return retval;
}

extern void smtc_send_ipi(int cpu, int type, unsigned int action);
