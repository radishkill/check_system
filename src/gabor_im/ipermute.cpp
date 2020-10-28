//
// File: ipermute.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "ipermute.h"
#include "gabor_im_emxutil.h"

// Function Definitions

//
// Arguments    : const emxArray_creal_T *b
//                emxArray_creal_T *a
// Return Type  : void
//
void ipermute(const emxArray_creal_T *b, emxArray_creal_T *a)
{
  int i12;
  int loop_ub;
  int b_loop_ub;
  int i13;
  i12 = a->size[0] * a->size[1];
  a->size[0] = b->size[1];
  a->size[1] = b->size[0];
  emxEnsureCapacity_creal_T(a, i12);
  loop_ub = b->size[0];
  for (i12 = 0; i12 < loop_ub; i12++) {
    b_loop_ub = b->size[1];
    for (i13 = 0; i13 < b_loop_ub; i13++) {
      a->data[i13 + a->size[0] * i12] = b->data[i12 + b->size[0] * i13];
    }
  }
}

//
// File trailer for ipermute.cpp
//
// [EOF]
//
