//
// File: gabor_im.h
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//
#ifndef GABOR_IM_H
#define GABOR_IM_H

// Include Files
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "omp.h"
#include "gabor_im_types.h"

// Function Declarations
extern void gabor_im(const emxArray_uint8_T *image, double wavelength, double
                     angle, emxArray_real_T *Gimage_im, emxArray_boolean_T
                     *BW_im, emxArray_boolean_T *K);

#endif

//
// File trailer for gabor_im.h
//
// [EOF]
//
