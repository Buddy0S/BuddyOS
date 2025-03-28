.global READ32
.global READ16
.global READ8
.global WRITE32
.global WRITE16
.global WRITE8


WRITE32:
    str r1,[r0]
    bx lr

READ32:
    ldr r0,[r0]
    bx lr

WRITE16:
    strh r1,[r0]
    bx lr

READ16:
    ldrh r0,[r0]
    bx lr

WRITE8:
    strb r1,[r0]
    bx lr

READ8:
    ldrb r0,[r0]
    bx lr

