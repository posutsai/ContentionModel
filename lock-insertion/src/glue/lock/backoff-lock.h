#include "padding.h"
#include "utils.h"
#include <stdio.h>

#ifndef __DYLINX_BACKOFF_LOCK__
#define __DYLINX_BACKOFF_LOCK__

// Source code implementation refers to following two places.
// 1. backoff-lock implementation in LITL
// https://github.com/multicore-locks/litl/blob/master/src/backoff.c
// 2. backoff-lock implementation in concurrencykit
// https://github.com/concurrencykit/ck/blob/master/include/ck_backoff.h

#define DEFAULT_BACKOFF_DELAY (1 << 9)
#define MAX_BACKOFF_DELAY ((1 << 20) -1)

typedef struct backoff_lock {
  volatile uint8_t spin_lock __attribute__((aligned(L_CACHE_LINE_SIZE)));
  char __pad[pad_to_cache_line(sizeof(uint8_t))];
} backoff_lock_t __attribute__((aligned(L_CACHE_LINE_SIZE)));

int backoff_init(void **entity, pthread_mutexattr_t *attr) {
#ifdef __DYLINX_DEBUG__
  printf("backoff-lock is initialized\n");
#endif
  backoff_lock_t *mtx = *entity;
  mtx = (backoff_lock_t *)alloc_cache_align(sizeof(backoff_lock_t));
  mtx->spin_lock = UNLOCKED;
  return 0;
}

int backoff_lock(void **entity) {
  uint32_t delay = DEFAULT_BACKOFF_DELAY;
  backoff_lock_t *mtx = *entity;
  while (1) {
    while (mtx->spin_lock != UNLOCKED) {
      for (uint32_t i = 0; i < delay; i++)
        CPU_PAUSE();
      if (delay < MAX_BACKOFF_DELAY)
        delay *= 2;
    }

    if (l_tas_uint8(&mtx->spin_lock) == UNLOCKED)
      break;
  }
  return 0;
}

int backoff_unlock(void **entity) {
  COMPILER_BARRIER();
  backoff_lock_t *mtx = *entity;
  mtx->spin_lock = UNLOCKED;
  return 1;
}

int backoff_destroy(void **entity) {
  backoff_lock_t *mtx = *entity;
  free(mtx);
  return 1;
}

DYLINX_INIT_LOCK(backoff, 3);
#endif