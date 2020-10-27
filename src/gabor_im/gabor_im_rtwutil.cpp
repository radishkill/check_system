//
// File: gabor_im_rtwutil.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include <cmath>
#include <float.h>
#include "rt_nonfinite.h"
#include "gabor_im_rtwutil.h"

// Function Definitions

//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
double rt_remd_snf(double u0, double u1)
{
  double y;
  double b_u1;
  double q;
  if (rtIsNaN(u0) || rtIsInf(u0) || (rtIsNaN(u1) || rtIsInf(u1))) {
    y = rtNaN;
  } else {
    if (u1 < 0.0) {
      b_u1 = std::ceil(u1);
    } else {
      b_u1 = std::floor(u1);
    }

    if ((u1 != 0.0) && (u1 != b_u1)) {
      q = std::abs(u0 / u1);
      if (std::abs(q - std::floor(q + 0.5)) <= DBL_EPSILON * q) {
        y = 0.0 * u0;
      } else {
        y = std::fmod(u0, u1);
      }
    } else {
      y = std::fmod(u0, u1);
    }
  }

  return y;
}

//
// File trailer for gabor_im_rtwutil.cpp
//
// [EOF]
//
