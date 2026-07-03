#ifndef MRUBYC_EXT_MRUBYC_EXT_H_
#define MRUBYC_EXT_MRUBYC_EXT_H_

#include "mrubyc.h"
#include "stm32h5_gpio.h"

/* sleep */
void c_sleep_ms(struct VM *vm, mrbc_value v[], int argc);

/* print */
void c_print(struct VM *vm, mrbc_value v[], int argc);

#endif /* MRUBYC_EXT_MRUBYC_EXT_H_ */
