#ifndef MRUBY_LIBRAY_TIME_H_
#define MRUBY_LIBRAY_TIME_H_

//@cond
#include <stdint.h>
//@endcond

/*
 * Time object data.
 * Ruby の Time オブジェクトが内部で保持する時刻情報。
 * 0:00:00 からの経過秒として扱う。
 */
typedef struct MRBC_TIME {
  uint64_t total_sec;
} MRBC_TIME;


/*
 * function prototypes.
 */
void mrbc_init_class_time(void);

#endif /* MRUBY_LIBRAY_TIME_H_ */
