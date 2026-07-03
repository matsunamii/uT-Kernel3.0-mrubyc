#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include "mrubyc_ext.h"
#include "mrubyc.h"

extern const uint8_t led[];
extern const uint8_t print[];

/* Task Information */
typedef struct {
  const uint8_t *script;
  mrbc_vm *vm;

  ID task_id;
  ID alarm_id;
  RELTIM timeslice;

  T_CTSK ctsk;
  T_CALM calm;
} mruby_task_info_t;

#define MRBC_MEMORY_SIZE (1024 * 40)
static uint8_t memory_pool[MRBC_MEMORY_SIZE];

/* Forward declarations */
LOCAL void mruby_task_common(mruby_task_info_t *info);
LOCAL void mruby_task_entry(INT stacd, void *exinf);
LOCAL void alarm_handler(void *exinf);

/* Ruby Task Table */
mruby_task_info_t mruby_tasks[] = {
  {
    .script = led,
    .vm = NULL,
    .task_id = 0,
    .alarm_id = 0,
    .timeslice = 100,

    .ctsk = {
      .itskpri = 10,
      .stksz   = 1024,
      .task    = mruby_task_entry,
      .tskatr  = TA_HLNG | TA_RNG3,
    },

    .calm = {
      .almatr = TA_HLNG,
      .almhdr = alarm_handler,
      .exinf  = NULL,
    }
  },

  {
    .script = print,
    .vm = NULL,
    .task_id = 0,
    .alarm_id = 0,
    .timeslice = 100,

    .ctsk = {
      .itskpri = 10,
      .stksz   = 1024,
      .task    = mruby_task_entry,
      .tskatr  = TA_HLNG | TA_RNG3,
    },

    .calm = {
      .almatr = TA_HLNG,
      .almhdr = alarm_handler,
      .exinf  = NULL,
    }
  }
};

#define NUM_MRUBY_TASKS \
  (sizeof(mruby_tasks) / sizeof(mruby_tasks[0]))

/* Alarm Handler */
LOCAL void alarm_handler(void *exinf)
{
  mruby_task_info_t *info = (mruby_task_info_t *)exinf;

  if (info != NULL && info->vm != NULL) {
    info->vm->flag_preemption = 1;
  }
}

/* Common mruby Task */
LOCAL void mruby_task_common(mruby_task_info_t *info)
{
  while (1) {
    info->vm->flag_preemption = 0;

    tk_sta_alm(info->alarm_id, info->timeslice);

    int ret = mrbc_vm_run(info->vm);

    tk_stp_alm(info->alarm_id);

    if (ret == 1) {
      mrbc_vm_end(info->vm);
      mrbc_vm_close(info->vm);
      info->vm = NULL;
      tk_ext_tsk();
    }

    tk_dly_tsk(1);
  }
}

LOCAL void mruby_task_entry(INT stacd, void *exinf)
{
  mruby_task_info_t *info = (mruby_task_info_t *)exinf;

  if (info == NULL) {
    tm_putstring((UB*)"Task exinf is NULL\n");
    tk_ext_tsk();
  }

  mruby_task_common(info);
}

/* usermain */
EXPORT INT usermain(void)
{
  tm_putstring((UB*)"Start User-main program.\n");

  out_w(GPIO_ODR(A),
        in_w(GPIO_ODR(A)) & ~(1 << 5));

  mrbc_init_alloc(memory_pool, MRBC_MEMORY_SIZE);
  mrbc_init_global();
  mrbc_init_class();

  mrbc_init_class_gpio();

  mrbc_define_method(0, MRBC_CLASS(Object), "print", c_print);
  mrbc_define_method(0, MRBC_CLASS(Object), "sleep_ms", c_sleep_ms);

  for (int i = 0; i < NUM_MRUBY_TASKS; i++) {
    mruby_tasks[i].vm = mrbc_vm_open(NULL);

    if (mruby_tasks[i].vm == NULL) {
      tm_putstring((UB*)"VM open failed\n");
      tk_slp_tsk(TMO_FEVR);
    }

    if (mrbc_load_mrb(mruby_tasks[i].vm,
                      mruby_tasks[i].script) != 0) {
      mrbc_print_vm_exception(mruby_tasks[i].vm);
      mrbc_vm_close(mruby_tasks[i].vm);
      tm_putstring((UB*)"MRB load failed\n");
      tk_slp_tsk(TMO_FEVR);
    }

    mrbc_vm_begin(mruby_tasks[i].vm);

    mruby_tasks[i].ctsk.exinf = (void *)&mruby_tasks[i];

    mruby_tasks[i].calm.exinf = (void *)&mruby_tasks[i];

    mruby_tasks[i].task_id = tk_cre_tsk(&mruby_tasks[i].ctsk);

    if (mruby_tasks[i].task_id < 0) {
      tm_putstring((UB*)"Task create failed\n");
      tk_slp_tsk(TMO_FEVR);
    }

    mruby_tasks[i].alarm_id = tk_cre_alm(&mruby_tasks[i].calm);

    if (mruby_tasks[i].alarm_id < 0) {
      tm_putstring((UB*)"Alarm create failed\n");
      tk_slp_tsk(TMO_FEVR);
    }

    tk_sta_tsk(mruby_tasks[i].task_id, 0);
  }

  tk_slp_tsk(TMO_FEVR);

  return 0;
}
