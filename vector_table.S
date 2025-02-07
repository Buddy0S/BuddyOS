.global vector_table

@ ARM V7 Vector Table
@ https://developer.arm.com/documentation/den0013/d/Exception-Handling/Exception-priorities?lang=en#CEGHDCAE

.balign 32
vector_table:
    b init                   @ Used for Reset
    b undefined_instruction
    b supervisor_call
    b prefetch_abort
    b data_abort
    b not_used
    b irq_interrupt
    b fiq_interrupt


@ when an exception happens it jumps to the label 
@ depending on what type of exception it is

undefined_instruction:
    b loop

supervisor_call:
    b loop

prefetch_abort:
    b loop

data_abort:
    b loop

not_used:
    b loop

@ interrupt exception handler
irq_interrupt:
    b loop

fiq_interrupt:
    b loop


@ loop to stall unhandled exception types
loop:
   b loop
