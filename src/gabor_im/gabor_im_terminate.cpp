//
// File: gabor_im_terminate.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "gabor_im_terminate.h"
#include "gabor_im_data.h"

// Function Definitions

//
// Arguments    : void
// Return Type  : void
//
void gabor_im_terminate()
{
  omp_destroy_nest_lock(&emlrtNestLockGlobal);
}

//
// File trailer for gabor_im_terminate.cpp
//
// [EOF]
//
