#ifndef __ARM_PERCPU_H__
#define __ARM_PERCPU_H__

#ifdef CONFIG_SMP

#ifndef RELOC_HIDE
#define RELOC_HIDE(ptr, off)				\
({							\
	unsigned long __ptr = (unsigned long)(ptr);	\
	(typeof(ptr))(__ptr + (off));			\
})
#endif

#define PERCPU_SHIFT	12
#define PERCPU_SIZE	(1UL << PERCPU_SHIFT)

/* Separate out the type, so (int[3], foo) works. */
#define DEFINE_PER_CPU(type, name)			\
	__attribute__((__section__(".data.percpu")))	\
	__typeof__(type) per_cpu__##name

/* var is in discarded region: offset to particular copy we want */
#define per_cpu(var, cpu)  \
	(*RELOC_HIDE(&per_cpu__##var, ((unsigned int)(cpu))<< PERCPU_SHIFT))

#define __get_cpu_var(var)		per_cpu(var, smp_processor_id())

#define DECLARE_PER_CPU(type, name)	extern __typeof__(type) per_cpu__##name
#else /* !CONFIG_SMP */
#define DEFINE_PER_CPU(type,name)	__typeof__(type) per_cpu__##name

#define per_cpu(var, cpu)		(*((void)(cpu), &per_cpu__##var))
#define __get_cpu_var(var)		per_cpu__##var

#define DECLARE_PER_CPU(type,name)	extern __typeof__(type) per_cpu__##name

#endif /* CONFIG_SMP */
#endif /* __ARM_PERCPU_H__ */
