.text

.global MMU_enable
.type MMU_enable, %function
MMU_enable:
    //From the arm docs
    MRC p15, 0, r1, c1, c0, 0
    ORR r1, r1, #0x1 //Flag for the control register
    MCR p15, 0, r1, c1, c0, 0
    bx lr


.global MMU_disable
.type MMU_disable, %function
MMU_disable:
    MRC p15, 0, r1, c1, c0, 0
    BIC r1, r1, #0x1 //Removing the flag for the control register
    MCR p15, 0, r1, c1, c0, 0
    bx lr


