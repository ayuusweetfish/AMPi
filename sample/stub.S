.section .init
.global _start
_start:
    # Stack pointer for IRQ mode
    cps     #0x12
    ldr     sp, =0x4008000
    # Stack pointer for System mode
    cps     #0x1F
    ldr     sp, =0x4010000
    b	    sysinit

.global _irq_stub
_irq_stub:
    sub		lr, lr, #4            /* lr: return address */
    stmfd	sp!, {r0-r3, r12, lr}        /* save r0-r3, r12 and return address */
    bl		_int_irq
    ldmfd   sp!, {r0-r3, r12, pc}^        /* restore registers and return */

.global EnableMMU
EnableMMU:
    mov     r1, #0
    # Invalidate d/i/unified caches (ARM ARM p. B6-21)
    mcr     p15, 0, r1, c7, c7, 0
    # Invalidate d/i/unified TLBs (ARM ARM p. B4-45)
    mcr     p15, 0, r1, c8, c7, 0
    # Tell the coprocessor about the table address (ARM ARM p. B4-41/B5-18)
    orr     r0, #1
    mcr     p15, 0, r0, c2, c0, 0
    mcr     p15, 0, r0, c2, c0, 1
    # Set domain control access to Manager (ARM ARM p. B4-10/B5-18)
    # Also http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0360f/CHDGIJFB.html
    mov     r1, #0xf
    mcr     p15, 0, r1, c3, c0, 0
    # Turn on MMU, with i/d caches (ARM ARM p. B3-12/B4-40/B5-18/B6-18)
    mrc     p15, 0, r1, c1, c0, 0
    orr     r1, r1, #0xd    // MMU & d-cache (B3-12)
    orr     r1, r1, #0x1f00 // i-cache & branch prediction (B3-12)
    mcr     p15, 0, r1, c1, c0, 0
    bx      lr
