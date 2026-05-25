#include <asm/desc.h>

void my_store_idt(struct desc_ptr *idtr) {
asm volatile("sidt %0;"
    :"=m"(*idtr) /* output */
    : /* input */
    : /* clobbered */
    );
}

void my_load_idt(struct desc_ptr *idtr) {
asm volatile("lidt %0;"
    : /* output */
    :"m"(*idtr) /* input */
    : /* clobbered */
    );
}

void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
    gate->offset_low    = addr & 0xFFFF;               // bits 0–15
    gate->offset_middle = (addr >> 16) & 0xFFFF;        // bits 16–31
    gate->offset_high   = (addr >> 32) & 0xFFFFFFFF;    // bits 32–63
}

unsigned long my_get_gate_offset(gate_desc *gate) {
    unsigned long offset = 0;

    offset |= (unsigned long)gate->offset_low;
    offset |= (unsigned long)gate->offset_middle << 16;
    offset |= (unsigned long)gate->offset_high << 32;

    return offset;
}
