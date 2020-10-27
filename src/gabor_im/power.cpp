//
// File: power.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "power.h"
#include "gabor_im_emxutil.h"

// Function Definitions

//
// Arguments    : const emxArray_real_T *a
//                emxArray_real_T *y
// Return Type  : void
//
void power(const emxArray_real_T *a, emxArray_real_T *y)
{
  unsigned int unnamed_idx_0;
  unsigned int unnamed_idx_1;
  int nx;
  int k;
  unnamed_idx_0 = (unsigned int)a->size[0];
  unnamed_idx_1 = (unsigned int)a->size[1];
  nx = y->size[0] * y->size[1];
  y->size[0] = (int)unnamed_idx_0;
  y->size[1] = (int)unnamed_idx_1;
  emxEnsureCapacity_real_T(y, nx);
  nx = (int)unnamed_idx_0 * (int)unnamed_idx_1;
  for (k = 0; k < nx; k++) {
    y->data[k] = a->data[k] * a->data[k];
  }
}

//
// File trailer for power.cpp
//
// [EOF]
//
