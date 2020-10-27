/*
 * File: _coder_gabor_im_api.h
 *
 * MATLAB Coder version            : 4.1
 * C/C++ source code generated on  : 15-Oct-2020 16:25:25
 */

#ifndef _CODER_GABOR_IM_API_H
#define _CODER_GABOR_IM_API_H

/* Include Files */
#include "tmwtypes.h"
#include "mex.h"
#include "emlrt.h"
#include <stddef.h>
#include <stdlib.h>
#include "_coder_gabor_im_api.h"

/* Type Definitions */
#ifndef struct_emxArray_boolean_T
#define struct_emxArray_boolean_T

struct emxArray_boolean_T
{
  boolean_T *data;
  int32_T *size;
  int32_T allocatedSize;
  int32_T numDimensions;
  boolean_T canFreeData;
};

#endif                                 /*struct_emxArray_boolean_T*/

#ifndef typedef_emxArray_boolean_T
#define typedef_emxArray_boolean_T

typedef struct emxArray_boolean_T emxArray_boolean_T;

#endif                                 /*typedef_emxArray_boolean_T*/

#ifndef struct_emxArray_real_T
#define struct_emxArray_real_T

struct emxArray_real_T
{
  real_T *data;
  int32_T *size;
  int32_T allocatedSize;
  int32_T numDimensions;
  boolean_T canFreeData;
};

#endif                                 /*struct_emxArray_real_T*/

#ifndef typedef_emxArray_real_T
#define typedef_emxArray_real_T

typedef struct emxArray_real_T emxArray_real_T;

#endif                                 /*typedef_emxArray_real_T*/

#ifndef struct_emxArray_uint8_T
#define struct_emxArray_uint8_T

struct emxArray_uint8_T
{
  uint8_T *data;
  int32_T *size;
  int32_T allocatedSize;
  int32_T numDimensions;
  boolean_T canFreeData;
};

#endif                                 /*struct_emxArray_uint8_T*/

#ifndef typedef_emxArray_uint8_T
#define typedef_emxArray_uint8_T

typedef struct emxArray_uint8_T emxArray_uint8_T;

#endif                                 /*typedef_emxArray_uint8_T*/

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

/* Function Declarations */
extern void gabor_im(emxArray_uint8_T *image, real_T wavelength, real_T angle,
                     emxArray_real_T *Gimage_im, emxArray_boolean_T *BW_im,
                     emxArray_boolean_T *K);
extern void gabor_im_api(const mxArray * const prhs[3], int32_T nlhs, const
  mxArray *plhs[3]);
extern void gabor_im_atexit(void);
extern void gabor_im_initialize(void);
extern void gabor_im_terminate(void);
extern void gabor_im_xil_terminate(void);

#endif

/*
 * File trailer for _coder_gabor_im_api.h
 *
 * [EOF]
 */
