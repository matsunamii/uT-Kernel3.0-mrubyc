#include <tk/tkernel.h>
#include "mrubyc.h"

void c_sleep_ms(struct VM *vm, mrbc_value v[], int argc)
{
  if (argc != 1) return;

  int ms = GET_INT_ARG(1);

  if (ms < 0) ms = 0;

  tk_dly_tsk(ms);
}
