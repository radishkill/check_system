//
// File: createNormalizedFrequencyVector.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include <cmath>
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "createNormalizedFrequencyVector.h"
#include "gabor_im_emxutil.h"

// Function Definitions

//
// Arguments    : double N
//                emxArray_real_T *u
// Return Type  : void
//
void createNormalizedFrequencyVector(double N, emxArray_real_T *u)
{
  double delta2;
  double d1_tmp;
  int i17;
  double delta1;
  int k;
  if ((!rtIsInf(N)) && (!rtIsNaN(N))) {
    if (N == 0.0) {
      delta2 = 0.0;
    } else {
      delta2 = std::fmod(N, 2.0);
      if (delta2 == 0.0) {
        delta2 = 0.0;
      }
    }
  } else {
    delta2 = rtNaN;
  }

  if (delta2 != 0.0) {
    d1_tmp = 1.0 / (2.0 * N);
    i17 = u->size[0] * u->size[1];
    u->size[0] = 1;
    u->size[1] = (int)N;
    emxEnsureCapacity_real_T(u, i17);
    if (u->size[1] >= 1) {
      u->data[u->size[1] - 1] = 0.5 - d1_tmp;
      if (u->size[1] >= 2) {
        u->data[0] = -0.5 + d1_tmp;
        if (u->size[1] >= 3) {
          if (((-0.5 + d1_tmp < 0.0) != (0.5 - d1_tmp < 0.0)) && ((std::abs(-0.5
                 + d1_tmp) > 8.9884656743115785E+307) || (std::abs(0.5 - d1_tmp)
                > 8.9884656743115785E+307))) {
            delta1 = (-0.5 + d1_tmp) / ((double)u->size[1] - 1.0);
            delta2 = (0.5 - d1_tmp) / ((double)u->size[1] - 1.0);
            i17 = u->size[1];
            for (k = 0; k <= i17 - 3; k++) {
              u->data[k + 1] = ((-0.5 + d1_tmp) + delta2 * (1.0 + (double)k)) -
                delta1 * (1.0 + (double)k);
            }
          } else {
            delta1 = ((0.5 - d1_tmp) - (-0.5 + d1_tmp)) / ((double)u->size[1] -
              1.0);
            i17 = u->size[1];
            for (k = 0; k <= i17 - 3; k++) {
              u->data[k + 1] = (-0.5 + d1_tmp) + (1.0 + (double)k) * delta1;
            }
          }
        }
      }
    }
  } else {
    delta2 = 0.5 - 1.0 / N;
    i17 = u->size[0] * u->size[1];
    u->size[0] = 1;
    u->size[1] = (int)N;
    emxEnsureCapacity_real_T(u, i17);
    if (u->size[1] >= 1) {
      u->data[u->size[1] - 1] = delta2;
      if (u->size[1] >= 2) {
        u->data[0] = -0.5;
        if (u->size[1] >= 3) {
          if ((!(delta2 < 0.0)) && (delta2 > 8.9884656743115785E+307)) {
            delta1 = -0.5 / ((double)u->size[1] - 1.0);
            delta2 /= (double)u->size[1] - 1.0;
            i17 = u->size[1];
            for (k = 0; k <= i17 - 3; k++) {
              u->data[k + 1] = (-0.5 + delta2 * (1.0 + (double)k)) - delta1 *
                (1.0 + (double)k);
            }
          } else {
            delta1 = (delta2 - -0.5) / ((double)u->size[1] - 1.0);
            i17 = u->size[1];
            for (k = 0; k <= i17 - 3; k++) {
              u->data[k + 1] = -0.5 + (1.0 + (double)k) * delta1;
            }
          }
        }
      }
    }
  }
}

//
// File trailer for createNormalizedFrequencyVector.cpp
//
// [EOF]
//
