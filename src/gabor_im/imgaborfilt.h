//
// File: imgaborfilt.h
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//
#ifndef IMGABORFILT_H
#define IMGABORFILT_H

// Include Files
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "omp.h"
#include "gabor_im_types.h"

// Function Declarations
extern void applyGaborFilterFFT(const emxArray_real_T *A, double
  params_wavelength, double params_orientation, double params_sigma_x, double
  params_sigma_y, emxArray_real_T *M, emxArray_real_T *P);

#endif

//
// File trailer for imgaborfilt.h
//
// [EOF]
//
