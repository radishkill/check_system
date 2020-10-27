//
// File: bluesteinSetup.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include <cmath>
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "bluesteinSetup.h"
#include "gabor_im_emxutil.h"

// Function Definitions

//
// Arguments    : int nRows
//                emxArray_creal_T *wwc
// Return Type  : void
//
void bluestein_setup_impl(int nRows, emxArray_creal_T *wwc)
{
  int nInt2m1;
  int rt;
  int idx;
  int nInt2;
  int k;
  int y;
  double nt_im;
  double nt_re;
  nInt2m1 = (nRows + nRows) - 1;
  rt = wwc->size[0];
  wwc->size[0] = nInt2m1;
  emxEnsureCapacity_creal_T(wwc, rt);
  idx = nRows;
  rt = 0;
  wwc->data[nRows - 1].re = 1.0;
  wwc->data[nRows - 1].im = 0.0;
  nInt2 = nRows << 1;
  for (k = 0; k <= nRows - 2; k++) {
    y = ((1 + k) << 1) - 1;
    if (nInt2 - rt <= y) {
      rt += y - nInt2;
    } else {
      rt += y;
    }

    nt_im = -3.1415926535897931 * (double)rt / (double)nRows;
    if (nt_im == 0.0) {
      nt_re = 1.0;
      nt_im = 0.0;
    } else {
      nt_re = std::cos(nt_im);
      nt_im = std::sin(nt_im);
    }

    wwc->data[idx - 2].re = nt_re;
    wwc->data[idx - 2].im = -nt_im;
    idx--;
  }

  idx = 0;
  rt = nInt2m1 - 1;
  for (k = rt; k >= nRows; k--) {
    wwc->data[k] = wwc->data[idx];
    idx++;
  }
}

//
// File trailer for bluesteinSetup.cpp
//
// [EOF]
//
