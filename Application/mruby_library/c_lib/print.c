#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include "mrubyc.h"

void c_print(struct VM *vm, mrbc_value v[], int argc)
{
  if (argc < 1) return;

  switch(v[1].tt){

  case MRBC_TT_STRING:
    tm_putstring((UB*)v[1].string->data);
    break;

  case MRBC_TT_INTEGER:
    tm_printf((UB*)"%d", v[1].i);
    break;

  default:
    tm_putstring((UB*)"unknown");
    break;
  }
}
