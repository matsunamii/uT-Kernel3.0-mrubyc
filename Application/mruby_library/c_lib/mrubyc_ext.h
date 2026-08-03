#ifndef MRUBY_LIBRAY_MRUBYC_EXT_H_
#define MRUBY_LIBRAY_MRUBYC_EXT_H_

#include "mrubyc.h"

#include "../../mruby_library/c_lib/stm32h5_gpio.h"
#include "../../mruby_library/c_lib/time.h"

/* sleep */
void c_delay_ms(struct VM *vm, mrbc_value v[], int argc);

/* print */
void c_print(struct VM *vm, mrbc_value v[], int argc);

#endif /* MRUBY_LIBRAY_MRUBYC_EXT_H_ */
