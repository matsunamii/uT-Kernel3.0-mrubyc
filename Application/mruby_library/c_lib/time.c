#include "time.h"

#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include "mrubyc.h"
#include "main.h"
#include <stdint.h>

typedef struct {
  uint64_t total_sec;
} mrbc_time;

static mrbc_class *class_time;

static uint64_t base_clock_sec = 0;  // Time.set で設定した時刻
static uint64_t base_rtos_ms   = 0;  // Time.set した瞬間のRTOS時刻

static uint64_t stopwatch_elapsed_ms = 0;
static uint64_t stopwatch_start_ms   = 0;
static int stopwatch_running         = 0;

static uint64_t get_rtos_ms(void)
{
  SYSTIM tim;
  tk_get_tim(&tim);

  return ((uint64_t)tim.hi << 32) | (uint64_t)tim.lo;
}

static uint64_t make_sec(int hour, int min, int sec)
{
  return ((uint64_t)hour * 60 * 60)
       + ((uint64_t)min  * 60)
       + (uint64_t)sec;
}
static void c_time_new(mrbc_vm *vm, mrbc_value v[], int argc)
{
  if (argc < 3) {
    SET_NIL_RETURN();
    return;
  }

  int hour = GET_INT_ARG(1);
  int min  = GET_INT_ARG(2);
  int sec  = GET_INT_ARG(3);

  if (hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 59) {
    SET_NIL_RETURN();
    return;
  }

  base_clock_sec = make_sec(hour, min, sec);
  base_rtos_ms   = get_rtos_ms();

  mrbc_value obj = mrbc_instance_new(vm, class_time, sizeof(mrbc_time));

  MRBC_TIME *time_data =
      (MRBC_TIME *)((uint8_t *)obj.instance + sizeof(mrbc_instance));

  time_data->total_sec = base_clock_sec;

  v[0] = obj;
}

static uint64_t get_current_sec(void)
{
  uint64_t now_ms = get_rtos_ms();
  uint64_t elapsed_sec = (now_ms - base_rtos_ms) / 1000;

  return (base_clock_sec + elapsed_sec) % 86400;
}

static void c_time_now(mrbc_vm *vm, mrbc_value v[], int argc)
{
  mrbc_value obj = mrbc_instance_new(vm, class_time, sizeof(mrbc_time));

  mrbc_time *time_data =
      (mrbc_time *)((uint8_t *)obj.instance + sizeof(mrbc_instance));

  time_data->total_sec = get_current_sec();

  v[0] =obj;
}

static mrbc_time *get_time_data(mrbc_value *obj)
{
  if (obj->instance == NULL) {
    return NULL;
  }

  return (mrbc_time *)((uint8_t *)obj->instance + sizeof(mrbc_instance));
}

static void c_time_hour(mrbc_vm *vm, mrbc_value v[], int argc)
{
  mrbc_time *time_data = get_time_data(&v[0]);

  if (time_data == NULL) {
    SET_NIL_RETURN();
    return;
  }

  SET_INT_RETURN((int)((time_data->total_sec / 3600) % 24));
}

static void c_time_min(mrbc_vm *vm, mrbc_value v[], int argc)
{
  mrbc_time *time_data = get_time_data(&v[0]);

  if (time_data == NULL) {
    SET_NIL_RETURN();
    return;
  }

  SET_INT_RETURN((int)((time_data->total_sec / 60) % 60));
}

static void c_time_sec(mrbc_vm *vm, mrbc_value v[], int argc)
{
  mrbc_time *time_data = get_time_data(&v[0]);

  if (time_data == NULL) {
    SET_NIL_RETURN();
    return;
  }

  SET_INT_RETURN((int)(time_data->total_sec % 60));
}

static void c_time_start(mrbc_vm *vm, mrbc_value v[], int argc)
{
  if (!stopwatch_running) {
    stopwatch_start_ms = get_rtos_ms();
    stopwatch_running = 1;
  }

  SET_NIL_RETURN();
}

static void c_time_stop(mrbc_vm *vm, mrbc_value v[], int argc)
{
  if (stopwatch_running) {
    uint64_t now_ms = get_rtos_ms();

    stopwatch_elapsed_ms += now_ms - stopwatch_start_ms;
    stopwatch_running = 0;
  }

  SET_NIL_RETURN();
}

static void c_time_reset(mrbc_vm *vm, mrbc_value v[], int argc)
{
  stopwatch_elapsed_ms = 0;
  stopwatch_start_ms = get_rtos_ms();
  stopwatch_running = 0;

  SET_NIL_RETURN();
}

static uint64_t get_stopwatch_ms(void)
{
  uint64_t elapsed_ms = stopwatch_elapsed_ms;

  if (stopwatch_running) {
    elapsed_ms += get_rtos_ms() - stopwatch_start_ms;
  }

  return elapsed_ms;
}

static void c_time_elapsed(mrbc_vm *vm, mrbc_value v[], int argc)
{
  mrbc_value obj =
      mrbc_instance_new(vm, class_time, sizeof(mrbc_time));

  mrbc_time *time_data =
      (mrbc_time *)((uint8_t *)obj.instance +
                    sizeof(mrbc_instance));

  time_data->total_sec = get_stopwatch_ms() / 1000;

  v[0] = obj;
}

//================================================================
/*! set up the Time class.
*/
void mrbc_init_class_time( void )
{
  class_time = mrbc_define_class(0, "Time", 0);

  mrbc_define_method(0, class_time, "new", c_time_new);
  mrbc_define_method(0, class_time, "now",  c_time_now);

  mrbc_define_method(0, class_time, "hour", c_time_hour);
  mrbc_define_method(0, class_time, "min",  c_time_min);
  mrbc_define_method(0, class_time, "sec",  c_time_sec);

  /* ストップウォッチ機能 */
    mrbc_define_method(0, class_time, "start",   c_time_start);
    mrbc_define_method(0, class_time, "stop",    c_time_stop);
    mrbc_define_method(0, class_time, "reset",   c_time_reset);
    mrbc_define_method(0, class_time, "elapsed", c_time_elapsed);

}
