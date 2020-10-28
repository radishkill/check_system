//
// File: padarray.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include <cmath>
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "padarray.h"
#include "gabor_im_emxutil.h"

// Function Declarations
static double rt_roundd_snf(double u);

// Function Definitions

//
// Arguments    : double u
// Return Type  : double
//
static double rt_roundd_snf(double u)
{
  double y;
  if (std::abs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = std::floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = std::ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

//
// Arguments    : const emxArray_real_T *varargin_1
//                const double varargin_2[2]
//                emxArray_real_T *b
// Return Type  : void
//
void padarray(const emxArray_real_T *varargin_1, const double varargin_2[2],
              emxArray_real_T *b)
{
  double sizeA_idx_0;
  double sizeA_idx_1;
  unsigned int sizeA_idx_0_tmp;
  int i1;
  unsigned int sizeA_idx_1_tmp;
  double varargin_1_idx_0;
  double varargin_1_idx_1;
  int loop_ub;
  emxArray_int32_T *idxA;
  emxArray_int8_T *onesVector;
  emxArray_real_T *y;
  emxArray_uint32_T *idxDir;
  emxArray_int32_T *r0;
  int i2;
  int i;
  if ((varargin_1->size[0] == 0) || (varargin_1->size[1] == 0)) {
    sizeA_idx_0 = (double)varargin_1->size[0] + 2.0 * varargin_2[0];
    sizeA_idx_1 = (double)varargin_1->size[1] + 2.0 * varargin_2[1];
    i1 = b->size[0] * b->size[1];
    b->size[0] = (int)sizeA_idx_0;
    b->size[1] = (int)sizeA_idx_1;
    emxEnsureCapacity_real_T(b, i1);
    loop_ub = (int)sizeA_idx_0 * (int)sizeA_idx_1;
    for (i1 = 0; i1 < loop_ub; i1++) {
      b->data[i1] = 0.0;
    }
  } else {
    sizeA_idx_0 = varargin_1->size[0];
    sizeA_idx_1 = varargin_1->size[1];
    sizeA_idx_0_tmp = (unsigned int)sizeA_idx_0;
    sizeA_idx_1_tmp = (unsigned int)sizeA_idx_1;
    varargin_1_idx_0 = 2.0 * varargin_2[0] + (double)sizeA_idx_0_tmp;
    varargin_1_idx_1 = 2.0 * varargin_2[1] + (double)sizeA_idx_1_tmp;
    if ((varargin_1_idx_0 < varargin_1_idx_1) || (rtIsNaN(varargin_1_idx_0) && (
          !rtIsNaN(varargin_1_idx_1)))) {
      varargin_1_idx_0 = varargin_1_idx_1;
    }

    emxInit_int32_T(&idxA, 2);
    emxInit_int8_T(&onesVector, 2);
    i1 = idxA->size[0] * idxA->size[1];
    idxA->size[0] = (int)varargin_1_idx_0;
    idxA->size[1] = 2;
    emxEnsureCapacity_int32_T(idxA, i1);
    i1 = onesVector->size[0] * onesVector->size[1];
    onesVector->size[0] = 1;
    loop_ub = (int)varargin_2[0];
    onesVector->size[1] = loop_ub;
    emxEnsureCapacity_int8_T(onesVector, i1);
    for (i1 = 0; i1 < loop_ub; i1++) {
      onesVector->data[i1] = 1;
    }

    emxInit_real_T(&y, 2);
    i1 = y->size[0] * y->size[1];
    y->size[0] = 1;
    loop_ub = (int)((double)(unsigned int)sizeA_idx_0 - 1.0);
    y->size[1] = loop_ub + 1;
    emxEnsureCapacity_real_T(y, i1);
    for (i1 = 0; i1 <= loop_ub; i1++) {
      y->data[i1] = 1.0 + (double)i1;
    }

    emxInit_uint32_T(&idxDir, 2);
    i1 = idxDir->size[0] * idxDir->size[1];
    idxDir->size[0] = 1;
    idxDir->size[1] = (onesVector->size[1] + y->size[1]) + onesVector->size[1];
    emxEnsureCapacity_uint32_T(idxDir, i1);
    loop_ub = onesVector->size[1];
    for (i1 = 0; i1 < loop_ub; i1++) {
      idxDir->data[i1] = (unsigned int)onesVector->data[i1];
    }

    loop_ub = y->size[1];
    for (i1 = 0; i1 < loop_ub; i1++) {
      idxDir->data[i1 + onesVector->size[1]] = (unsigned int)rt_roundd_snf
        (y->data[i1]);
    }

    loop_ub = onesVector->size[1];
    for (i1 = 0; i1 < loop_ub; i1++) {
      idxDir->data[(i1 + onesVector->size[1]) + y->size[1]] = sizeA_idx_0_tmp *
        onesVector->data[i1];
    }

    emxInit_int32_T(&r0, 1);
    loop_ub = idxDir->size[1];
    i1 = r0->size[0];
    r0->size[0] = loop_ub;
    emxEnsureCapacity_int32_T(r0, i1);
    for (i1 = 0; i1 < loop_ub; i1++) {
      r0->data[i1] = i1;
    }

    loop_ub = r0->size[0];
    for (i1 = 0; i1 < loop_ub; i1++) {
      idxA->data[r0->data[i1]] = (int)idxDir->data[i1];
    }

    i1 = onesVector->size[0] * onesVector->size[1];
    onesVector->size[0] = 1;
    loop_ub = (int)varargin_2[1];
    onesVector->size[1] = loop_ub;
    emxEnsureCapacity_int8_T(onesVector, i1);
    for (i1 = 0; i1 < loop_ub; i1++) {
      onesVector->data[i1] = 1;
    }

    i1 = y->size[0] * y->size[1];
    y->size[0] = 1;
    loop_ub = (int)((double)(unsigned int)sizeA_idx_1 - 1.0);
    y->size[1] = loop_ub + 1;
    emxEnsureCapacity_real_T(y, i1);
    for (i1 = 0; i1 <= loop_ub; i1++) {
      y->data[i1] = 1.0 + (double)i1;
    }

    i1 = idxDir->size[0] * idxDir->size[1];
    idxDir->size[0] = 1;
    idxDir->size[1] = (onesVector->size[1] + y->size[1]) + onesVector->size[1];
    emxEnsureCapacity_uint32_T(idxDir, i1);
    loop_ub = onesVector->size[1];
    for (i1 = 0; i1 < loop_ub; i1++) {
      idxDir->data[i1] = (unsigned int)onesVector->data[i1];
    }

    loop_ub = y->size[1];
    for (i1 = 0; i1 < loop_ub; i1++) {
      idxDir->data[i1 + onesVector->size[1]] = (unsigned int)rt_roundd_snf
        (y->data[i1]);
    }

    loop_ub = onesVector->size[1];
    for (i1 = 0; i1 < loop_ub; i1++) {
      idxDir->data[(i1 + onesVector->size[1]) + y->size[1]] = sizeA_idx_1_tmp *
        onesVector->data[i1];
    }

    emxFree_real_T(&y);
    emxFree_int8_T(&onesVector);
    loop_ub = idxDir->size[1];
    i1 = r0->size[0];
    r0->size[0] = loop_ub;
    emxEnsureCapacity_int32_T(r0, i1);
    for (i1 = 0; i1 < loop_ub; i1++) {
      r0->data[i1] = i1;
    }

    loop_ub = r0->size[0];
    for (i1 = 0; i1 < loop_ub; i1++) {
      idxA->data[r0->data[i1] + idxA->size[0]] = (int)idxDir->data[i1];
    }

    emxFree_int32_T(&r0);
    emxFree_uint32_T(&idxDir);
    sizeA_idx_0 = (double)varargin_1->size[0] + 2.0 * varargin_2[0];
    sizeA_idx_1 = (double)varargin_1->size[1] + 2.0 * varargin_2[1];
    i1 = b->size[0] * b->size[1];
    b->size[0] = (int)sizeA_idx_0;
    b->size[1] = (int)sizeA_idx_1;
    emxEnsureCapacity_real_T(b, i1);
    i1 = b->size[1];
    for (loop_ub = 0; loop_ub < i1; loop_ub++) {
      i2 = b->size[0];
      for (i = 0; i < i2; i++) {
        b->data[i + b->size[0] * loop_ub] = varargin_1->data[(idxA->data[i] +
          varargin_1->size[0] * (idxA->data[loop_ub + idxA->size[0]] - 1)) - 1];
      }
    }

    emxFree_int32_T(&idxA);
  }
}

//
// File trailer for padarray.cpp
//
// [EOF]
//
