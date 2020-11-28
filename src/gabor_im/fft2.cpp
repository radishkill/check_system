//
// File: fft2.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

#include <iostream>
#include <chrono>

// Include Files
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "fft2.h"
#include "gabor_im_emxutil.h"
#include "fft1.h"
#include "ipermute.h"

// Function Definitions

//
// Arguments    : const emxArray_real_T *x
//                emxArray_creal_T *f
// Return Type  : void
//
void fft2(const emxArray_real_T *x, emxArray_creal_T *f)
{
  auto begin_tick = std::chrono::steady_clock::now();
  auto end_tick = std::chrono::steady_clock::now();
  emxArray_creal_T *y;
  emxArray_real_T *b;
  int i3;
  int sz_idx_1;
  emxArray_creal_T *b_y1;
  int sz_idx_0;
  emxArray_real_T *costab;
  int i4;
  emxArray_real_T *sintab;
  emxArray_real_T *sintabinv;
  boolean_T useRadix2;
  emxInit_creal_T(&y, 2);
  emxInit_real_T(&b, 2);
  i3 = b->size[0] * b->size[1];
  b->size[0] = x->size[1];
  b->size[1] = x->size[0];
  emxEnsureCapacity_real_T(b, i3);
  sz_idx_1 = x->size[0];
  for (i3 = 0; i3 < sz_idx_1; i3++) {
    sz_idx_0 = x->size[1];
    for (i4 = 0; i4 < sz_idx_0; i4++) {
      b->data[i4 + b->size[0] * i3] = x->data[i3 + x->size[0] * i4];
    }
  }

  emxInit_creal_T(&b_y1, 2);
  emxInit_real_T(&costab, 2);
  emxInit_real_T(&sintab, 2);
  emxInit_real_T(&sintabinv, 2);
  if ((b->size[0] == 0) || (b->size[1] == 0) || (x->size[1] == 0)) {
    sz_idx_1 = b->size[1];
    sz_idx_0 = x->size[1];
    i3 = b_y1->size[0] * b_y1->size[1];
    b_y1->size[0] = sz_idx_0;
    b_y1->size[1] = sz_idx_1;
    emxEnsureCapacity_creal_T(b_y1, i3);
    if (x->size[1] > b->size[0]) {
      i3 = b_y1->size[0] * b_y1->size[1];
      emxEnsureCapacity_creal_T(b_y1, i3);
      sz_idx_1 = b_y1->size[1];
      for (i3 = 0; i3 < sz_idx_1; i3++) {
        sz_idx_0 = b_y1->size[0];
        for (i4 = 0; i4 < sz_idx_0; i4++) {
          b_y1->data[i4 + b_y1->size[0] * i3].re = 0.0;
          b_y1->data[i4 + b_y1->size[0] * i3].im = 0.0;
        }
      }
    }
  } else {
    useRadix2 = ((x->size[1] & (x->size[1] - 1)) == 0);
    get_algo_sizes(x->size[1], useRadix2, &sz_idx_1, &sz_idx_0);
    generate_twiddle_tables(sz_idx_0, useRadix2, costab, sintab, sintabinv);
   
    if (useRadix2) {
      r2br_r2dit_trig(b, x->size[1], costab, sintab, b_y1);
    } else {
      dobluesteinfft(b, sz_idx_1, x->size[1], costab, sintab, sintabinv, b_y1);
    }
 
  }

  emxFree_real_T(&b);
  ipermute(b_y1, y);
  emxFree_creal_T(&b_y1);
  if ((y->size[0] == 0) || (y->size[1] == 0)) {
    sz_idx_0 = y->size[0];
    i3 = f->size[0] * f->size[1];
    f->size[0] = sz_idx_0;
    f->size[1] = y->size[1];
    emxEnsureCapacity_creal_T(f, i3);
  } else {
    useRadix2 = ((y->size[0] & (y->size[0] - 1)) == 0);
    get_algo_sizes(y->size[0], useRadix2, &sz_idx_1, &sz_idx_0);
    
    generate_twiddle_tables(sz_idx_0, useRadix2, costab, sintab, sintabinv);
    if (useRadix2) {
      c_r2br_r2dit_trig(y, y->size[0], costab, sintab, f);
    } else {
      b_dobluesteinfft(y, sz_idx_1, y->size[0], costab, sintab, sintabinv, f);
    }
   
  }
 
 

  emxFree_real_T(&sintabinv);
  emxFree_real_T(&sintab);
  emxFree_real_T(&costab);
  emxFree_creal_T(&y);
}

//
// File trailer for fft2.cpp
//
// [EOF]
//
