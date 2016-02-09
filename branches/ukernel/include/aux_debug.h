#ifndef __aux_debug_h
#define __aux_debug_h

void inspect_elf(void * elf);
void inspect_as(void * as);
void inspect_cs(void * cs);
void inspect_code(void * code);
void inspect_ds(void * ds);
void inspect_data(void * data);
void inspect_entry(int (* entry)());
void inspect_task(void * task);
void inspect_comm(void * comm);


#endif
