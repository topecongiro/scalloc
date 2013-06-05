// Copyright (c) 2012-2013, the scalloc Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef SCALLOC_COMMON_H_
#define SCALLOC_COMMON_H_

#include <stddef.h>  // size_t

#include "config.h"
#include "log.h"

#define UNLIKELY(x)   __builtin_expect((x), 0)
#define LIKELY(x)     __builtin_expect((x), 1)

#define cache_aligned __attribute__((aligned(64)))

#define always_inline inline __attribute__((always_inline))
#define no_inline __attribute__((noinline))

#define TLS_MODE __attribute__((tls_model ("initial-exec")))

// Number of system pages forming a block
const size_t kPageMultiple = SPAN_SIZE;

const size_t kMinAlignment = 16;

const size_t kMaxSmallShift = 9;
const size_t kMaxSmallSize = 1UL << kMaxSmallShift;
const size_t kMaxMediumShift = 21;
const size_t kMaxMediumSize = 1UL << kMaxMediumShift;

const size_t kMediumSpanSize = 1UL << 28;  // 256 MiB

const size_t kFineClasses = kMaxSmallSize / kMinAlignment + 1;
//const size_t kNumClasses = kMaxSmallSize / kMinAlignment + 1;
const size_t kCoarseClasses = kMaxMediumShift - kMaxSmallShift;
const size_t kNumClasses = kFineClasses + kCoarseClasses;


#ifdef SMALL_SPACE_SIZE
const size_t kSmallSpace = SMALL_SPACE_SIZE;
#elif defined  __x86_64__
//const size_t kSmallSpace = 1UL << 35;  // 32GiB
const size_t kSmallSpace = 1UL << 44;  // 16TiB
#elif defined __i386__
const size_t kSmallSpace = 1UL << 31;  // 2GiB
#else
#error "platform not supported"
#endif

#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
static const char log_table[256] = {
  -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
  LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
  LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
};

// base-2 logarithm of 32-bit integers
always_inline int Log2(size_t v) {
  unsigned int t, tt, r;  // temp vars
  if ((tt = (v >> 16))) {
    r =  (t = (tt >> 8)) ? 24 + log_table[t] : 16 + log_table[tt];
  } else {
    r =  (t = (v >> 8)) ? 8 + log_table[t] : log_table[v];
  }
  return r;
}

const size_t kSpanReuseThreshold = SPAN_REUSE_THRESHOLD;
const size_t kLocalReuseThreshold = LOCAL_REUSE_THRESHOLD;

// Prohibit reordering of instructions by the compiler.
inline void CompilerBarrier() {
  __asm__ __volatile__("" : : : "memory");
}

// Full memory fence on x86-64
inline void MemoryBarrier() {
  __asm__ __volatile__("mfence" : : : "memory");
}

always_inline size_t PadSize(size_t size, size_t multiple) {
  return (size + multiple - 1) / multiple * multiple;
}

#define ScallocAssert(c,msg)  \
  if (!(c)) {                 \
    ErrorOut(msg);            \
  }

#endif  // SCALLOC_COMMON_H_
