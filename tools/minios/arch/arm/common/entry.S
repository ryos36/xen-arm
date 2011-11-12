#define S_FRAME_SIZE    72
#define S_OLD_R0        68
#define S_PSR           64

#define S_PC            60
#define S_LR            56
#define S_SP            52
#define S_IP            48
#define S_FP            44
#define S_R10           40
#define S_R9            36
#define S_R8            32
#define S_R7            28
#define S_R6            24
#define S_R5            20
#define S_R4            16
#define S_R3            12
#define S_R2            8
#define S_R1            4
#define S_R0            0
#define S_OFF           8

.global hypervisor_callback
.align 0
hypervisor_callback:
	add		sp, sp, #16
	stmdb	sp!, {r0 - pc}
	mov		r0, #1
	mov		r1, #2
	bl		do_hypervisor_callback
	b		return_to_user
	
return_to_user:
	/*
	ldr		r1, [sp, #S_PC]
	adr		r2, .temp_pc
	str		r1, [r2]
	*/
	ldmia	sp, {r0 - pc}
	//ldr		pc, .temp_pc
	
.temp_pc :
	.long 0
	
.global failsafe_callback
.align 0
failsafe_callback:
	mov	r0, r0
	
