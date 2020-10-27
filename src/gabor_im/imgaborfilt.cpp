//
// File: imgaborfilt.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include <cmath>
#include "rt_nonfinite.h"
#include <math.h>
#include "rt_defines.h"
#include "gabor_im.h"
#include "imgaborfilt.h"
#include "gabor_im_emxutil.h"
#include "fft1.h"
#include "ipermute.h"
#include "fft2.h"
#include "power.h"
#include "cosd.h"
#include "sind.h"
#include "createNormalizedFrequencyVector.h"
#include "padarray.h"

// Function Declarations
static void getPaddingSize(double params_sigma_x, double params_sigma_y, double
  padSize[2]);
static double rt_atan2d_snf(double u0, double u1);
static double rt_hypotd_snf(double u0, double u1);

// Function Definitions

//
// Arguments    : double params_sigma_x
//                double params_sigma_y
//                double padSize[2]
// Return Type  : void
//
static void getPaddingSize(double params_sigma_x, double params_sigma_y, double
  padSize[2])
{
  double r;
  double u1;
  r = std::ceil(7.0 * params_sigma_x);
  u1 = std::ceil(7.0 * params_sigma_y);
  if ((r > u1) || rtIsNaN(u1)) {
  } else {
    r = u1;
  }

  padSize[0] = r;
  padSize[1] = r;
}

//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_atan2d_snf(double u0, double u1)
{
  double y;
  int b_u0;
  int b_u1;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = rtNaN;
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    if (u0 > 0.0) {
      b_u0 = 1;
    } else {
      b_u0 = -1;
    }

    if (u1 > 0.0) {
      b_u1 = 1;
    } else {
      b_u1 = -1;
    }

    y = atan2((double)b_u0, (double)b_u1);
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = atan2(u0, u1);
  }

  return y;
}

//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_hypotd_snf(double u0, double u1)
{
  double y;
  double a;
  double b;
  a = std::abs(u0);
  b = std::abs(u1);
  if (a < b) {
    a /= b;
    y = b * std::sqrt(a * a + 1.0);
  } else if (a > b) {
    b /= a;
    y = a * std::sqrt(b * b + 1.0);
  } else if (rtIsNaN(b)) {
    y = b;
  } else {
    y = a * 1.4142135623730951;
  }

  return y;
}

//
// Arguments    : const emxArray_real_T *A
//                double params_wavelength
//                double params_orientation
//                double params_sigma_x
//                double params_sigma_y
//                emxArray_real_T *M
//                emxArray_real_T *P
// Return Type  : void
//
void applyGaborFilterFFT(const emxArray_real_T *A, double params_wavelength,
  double params_orientation, double params_sigma_x, double params_sigma_y,
  emxArray_real_T *M, emxArray_real_T *P)
{
  emxArray_real_T *Apadded;
  double padSize[2];
  double sizeAPadded_idx_0;
  double sizeAPadded_idx_1;
  emxArray_real_T *u;
  emxArray_real_T *v;
  emxArray_real_T *U;
  emxArray_real_T *V;
  int nx;
  int ny;
  int vstride;
  int vlend2;
  int i1;
  int j;
  emxArray_real_T *b_U;
  int i;
  double freq;
  double b_A;
  double c;
  double d0;
  int vspread;
  emxArray_real_T *H;
  int k;
  emxArray_creal_T *x;
  int midoffset;
  emxArray_creal_T *y;
  int ia;
  int ib;
  emxArray_creal_T *b_y1;
  int ic;
  emxArray_real_T *sintabinv;
  boolean_T useRadix2;
  emxInit_real_T(&Apadded, 2);
  getPaddingSize(params_sigma_x, params_sigma_y, padSize);
  padarray(A, padSize, Apadded);
  sizeAPadded_idx_0 = Apadded->size[0];
  sizeAPadded_idx_1 = Apadded->size[1];
  emxInit_real_T(&u, 2);
  emxInit_real_T(&v, 2);
  emxInit_real_T(&U, 2);
  emxInit_real_T(&V, 2);
  createNormalizedFrequencyVector((double)(unsigned int)sizeAPadded_idx_1, u);
  createNormalizedFrequencyVector((double)(unsigned int)sizeAPadded_idx_0, v);
  nx = u->size[1];
  ny = v->size[1];
  vstride = v->size[1];
  vlend2 = u->size[1];
  i1 = U->size[0] * U->size[1];
  U->size[0] = vstride;
  U->size[1] = vlend2;
  emxEnsureCapacity_real_T(U, i1);
  vstride = v->size[1];
  vlend2 = u->size[1];
  i1 = V->size[0] * V->size[1];
  V->size[0] = vstride;
  V->size[1] = vlend2;
  emxEnsureCapacity_real_T(V, i1);
  if ((u->size[1] == 0) || (v->size[1] == 0)) {
  } else {
    for (j = 0; j < nx; j++) {
      for (i = 0; i < ny; i++) {
        U->data[i + U->size[0] * j] = u->data[j];
        V->data[i + V->size[0] * j] = v->data[i];
      }
    }
  }

  emxInit_real_T(&b_U, 2);
  sizeAPadded_idx_0 = 1.0 / (6.2831853071795862 * params_sigma_x);
  sizeAPadded_idx_1 = 1.0 / (6.2831853071795862 * params_sigma_y);
  freq = 1.0 / params_wavelength;
  b_A = 6.2831853071795862 * params_sigma_x * params_sigma_y;
  c = sizeAPadded_idx_0 * sizeAPadded_idx_0;
  sizeAPadded_idx_0 = sizeAPadded_idx_1 * sizeAPadded_idx_1;
  sizeAPadded_idx_1 = params_orientation;
  b_cosd(&sizeAPadded_idx_1);
  d0 = params_orientation;
  b_sind(&d0);
  i1 = b_U->size[0] * b_U->size[1];
  b_U->size[0] = U->size[0];
  b_U->size[1] = U->size[1];
  emxEnsureCapacity_real_T(b_U, i1);
  vspread = U->size[0] * U->size[1];
  for (i1 = 0; i1 < vspread; i1++) {
    b_U->data[i1] = (U->data[i1] * sizeAPadded_idx_1 - V->data[i1] * d0) - freq;
  }

  emxInit_real_T(&H, 2);
  power(b_U, H);
  sizeAPadded_idx_1 = params_orientation;
  b_sind(&sizeAPadded_idx_1);
  d0 = params_orientation;
  b_cosd(&d0);
  i1 = b_U->size[0] * b_U->size[1];
  b_U->size[0] = U->size[0];
  b_U->size[1] = U->size[1];
  emxEnsureCapacity_real_T(b_U, i1);
  vspread = U->size[0] * U->size[1];
  for (i1 = 0; i1 < vspread; i1++) {
    b_U->data[i1] = U->data[i1] * sizeAPadded_idx_1 + V->data[i1] * d0;
  }

  emxFree_real_T(&V);
  power(b_U, U);
  i1 = H->size[0] * H->size[1];
  vlend2 = H->size[0] * H->size[1];
  emxEnsureCapacity_real_T(H, vlend2);
  vspread = i1 - 1;
  emxFree_real_T(&b_U);
  for (i1 = 0; i1 <= vspread; i1++) {
    H->data[i1] = -0.5 * (H->data[i1] / c + U->data[i1] / sizeAPadded_idx_0);
  }

  emxFree_real_T(&U);
  nx = H->size[0] * H->size[1];
  for (k = 0; k < nx; k++) {
    H->data[k] = std::exp(H->data[k]);
  }

  i1 = H->size[0] * H->size[1];
  vlend2 = H->size[0] * H->size[1];
  emxEnsureCapacity_real_T(H, vlend2);
  vspread = i1 - 1;
  for (i1 = 0; i1 <= vspread; i1++) {
    H->data[i1] *= b_A;
  }

  if (H->size[0] > 1) {
    vlend2 = H->size[0] / 2;
    if (vlend2 << 1 == H->size[0]) {
      if (H->size[0] > 1) {
        vlend2 = H->size[0] / 2;
        ny = 1;
        for (k = 2; k < 3; k++) {
          ny *= H->size[1];
        }

        vspread = H->size[0] - 1;
        if (vlend2 << 1 == H->size[0]) {
          nx = 0;
          for (i = 0; i < ny; i++) {
            i1 = nx - 1;
            nx += vspread;
            for (j = 0; j < 1; j++) {
              i1++;
              nx++;
              ia = i1;
              ib = i1 + vlend2;
              for (k = 0; k < vlend2; k++) {
                sizeAPadded_idx_0 = H->data[ia];
                H->data[ia] = H->data[ib];
                H->data[ib] = sizeAPadded_idx_0;
                ia++;
                ib++;
              }
            }
          }
        } else {
          nx = 0;
          for (i = 0; i < ny; i++) {
            i1 = nx - 1;
            nx += vspread;
            for (j = 0; j < 1; j++) {
              i1++;
              nx++;
              ia = i1;
              ib = i1 + vlend2;
              sizeAPadded_idx_0 = H->data[ib];
              for (k = 0; k < vlend2; k++) {
                ic = ib + 1;
                H->data[ib] = H->data[ia];
                H->data[ia] = H->data[ic];
                ia++;
                ib = ic;
              }

              H->data[ib] = sizeAPadded_idx_0;
            }
          }
        }
      }
    } else {
      ny = 1;
      for (k = 2; k < 3; k++) {
        ny *= H->size[1];
      }

      vspread = H->size[0] - 1;
      nx = -1;
      for (i = 0; i < ny; i++) {
        i1 = nx;
        nx += vspread;
        for (j = 0; j < 1; j++) {
          i1++;
          nx++;
          ia = i1 + vlend2;
          ib = nx;
          sizeAPadded_idx_0 = H->data[nx];
          for (k = 0; k < vlend2; k++) {
            ia--;
            ic = ib;
            ib--;
            H->data[ic] = H->data[ia];
            H->data[ia] = H->data[ib];
          }

          H->data[ib] = sizeAPadded_idx_0;
        }
      }
    }
  }

  if (H->size[1] > 1) {
    vlend2 = H->size[1] / 2;
    if (vlend2 << 1 == H->size[1]) {
      if (H->size[1] > 1) {
        vlend2 = H->size[1] / 2;
        vstride = 1;
        for (k = 0; k < 1; k++) {
          vstride *= H->size[0];
        }

        vspread = (H->size[1] - 1) * vstride;
        midoffset = vlend2 * vstride;
        if (vlend2 << 1 == H->size[1]) {
          nx = 0;
          for (i = 0; i < 1; i++) {
            i1 = nx - 1;
            nx += vspread;
            for (j = 0; j < vstride; j++) {
              i1++;
              nx++;
              ia = i1;
              ib = i1 + midoffset;
              for (k = 0; k < vlend2; k++) {
                sizeAPadded_idx_0 = H->data[ia];
                H->data[ia] = H->data[ib];
                H->data[ib] = sizeAPadded_idx_0;
                ia += vstride;
                ib += vstride;
              }
            }
          }
        } else {
          nx = 0;
          for (i = 0; i < 1; i++) {
            i1 = nx - 1;
            nx += vspread;
            for (j = 0; j < vstride; j++) {
              i1++;
              nx++;
              ia = i1;
              ib = i1 + midoffset;
              sizeAPadded_idx_0 = H->data[ib];
              for (k = 0; k < vlend2; k++) {
                ic = ib + vstride;
                H->data[ib] = H->data[ia];
                H->data[ia] = H->data[ic];
                ia += vstride;
                ib = ic;
              }

              H->data[ib] = sizeAPadded_idx_0;
            }
          }
        }
      }
    } else {
      vstride = 1;
      for (k = 0; k < 1; k++) {
        vstride *= H->size[0];
      }

      vspread = (H->size[1] - 1) * vstride;
      midoffset = vlend2 * vstride;
      nx = -1;
      for (i = 0; i < 1; i++) {
        i1 = nx;
        nx += vspread;
        for (j = 0; j < vstride; j++) {
          i1++;
          nx++;
          ia = i1 + midoffset;
          ib = nx;
          sizeAPadded_idx_0 = H->data[nx];
          for (k = 0; k < vlend2; k++) {
            ia -= vstride;
            ic = ib;
            ib -= vstride;
            H->data[ic] = H->data[ia];
            H->data[ia] = H->data[ib];
          }

          H->data[ib] = sizeAPadded_idx_0;
        }
      }
    }
  }

  emxInit_creal_T(&x, 2);
  fft2(Apadded, x);
  i1 = x->size[0] * x->size[1];
  vlend2 = x->size[0] * x->size[1];
  emxEnsureCapacity_creal_T(x, vlend2);
  vspread = i1 - 1;
  emxFree_real_T(&Apadded);
  for (i1 = 0; i1 <= vspread; i1++) {
    x->data[i1].re *= H->data[i1];
    x->data[i1].im *= H->data[i1];
  }

  emxFree_real_T(&H);
  emxInit_creal_T(&y, 2);
  i1 = y->size[0] * y->size[1];
  y->size[0] = x->size[1];
  y->size[1] = x->size[0];
  emxEnsureCapacity_creal_T(y, i1);
  vspread = x->size[0];
  for (i1 = 0; i1 < vspread; i1++) {
    midoffset = x->size[1];
    for (vlend2 = 0; vlend2 < midoffset; vlend2++) {
      y->data[vlend2 + y->size[0] * i1] = x->data[i1 + x->size[0] * vlend2];
    }
  }

  emxInit_creal_T(&b_y1, 2);
  emxInit_real_T(&sintabinv, 2);
  if ((y->size[0] == 0) || (y->size[1] == 0) || (x->size[1] == 0)) {
    vlend2 = x->size[1];
    i1 = b_y1->size[0] * b_y1->size[1];
    b_y1->size[0] = vlend2;
    b_y1->size[1] = y->size[1];
    emxEnsureCapacity_creal_T(b_y1, i1);
    if (x->size[1] > y->size[0]) {
      i1 = b_y1->size[0] * b_y1->size[1];
      emxEnsureCapacity_creal_T(b_y1, i1);
      vspread = b_y1->size[1];
      for (i1 = 0; i1 < vspread; i1++) {
        midoffset = b_y1->size[0];
        for (vlend2 = 0; vlend2 < midoffset; vlend2++) {
          b_y1->data[vlend2 + b_y1->size[0] * i1].re = 0.0;
          b_y1->data[vlend2 + b_y1->size[0] * i1].im = 0.0;
        }
      }
    }
  } else {
    useRadix2 = ((x->size[1] & (x->size[1] - 1)) == 0);
    get_algo_sizes(x->size[1], useRadix2, &vlend2, &vstride);
    b_generate_twiddle_tables(vstride, useRadix2, u, v, sintabinv);
    if (useRadix2) {
      d_r2br_r2dit_trig(y, x->size[1], u, v, b_y1);
    } else {
      c_dobluesteinfft(y, vlend2, x->size[1], u, v, sintabinv, b_y1);
    }
  }

  ipermute(b_y1, y);
  if ((y->size[0] == 0) || (y->size[1] == 0)) {
    vlend2 = y->size[0];
    i1 = b_y1->size[0] * b_y1->size[1];
    b_y1->size[0] = vlend2;
    b_y1->size[1] = y->size[1];
    emxEnsureCapacity_creal_T(b_y1, i1);
  } else {
    useRadix2 = ((y->size[0] & (y->size[0] - 1)) == 0);
    get_algo_sizes(y->size[0], useRadix2, &vlend2, &vstride);
    b_generate_twiddle_tables(vstride, useRadix2, u, v, sintabinv);
    if (useRadix2) {
      d_r2br_r2dit_trig(y, y->size[0], u, v, b_y1);
    } else {
      c_dobluesteinfft(y, vlend2, y->size[0], u, v, sintabinv, b_y1);
    }
  }

  emxFree_real_T(&sintabinv);
  emxFree_creal_T(&y);
  emxFree_real_T(&v);
  emxFree_real_T(&u);
  padSize[0]++;
  padSize[1]++;
  sizeAPadded_idx_0 = (padSize[0] + (double)A->size[0]) - 1.0;
  sizeAPadded_idx_1 = (padSize[1] + (double)A->size[1]) - 1.0;
  if (padSize[0] > sizeAPadded_idx_0) {
    i1 = 0;
    vlend2 = 0;
  } else {
    i1 = (int)padSize[0] - 1;
    vlend2 = (int)sizeAPadded_idx_0;
  }

  if (padSize[1] > sizeAPadded_idx_1) {
    vstride = 0;
    ny = 0;
  } else {
    vstride = (int)padSize[1] - 1;
    ny = (int)sizeAPadded_idx_1;
  }

  nx = x->size[0] * x->size[1];
  vspread = vlend2 - i1;
  x->size[0] = vspread;
  midoffset = ny - vstride;
  x->size[1] = midoffset;
  emxEnsureCapacity_creal_T(x, nx);
  for (vlend2 = 0; vlend2 < midoffset; vlend2++) {
    for (ny = 0; ny < vspread; ny++) {
      x->data[ny + x->size[0] * vlend2] = b_y1->data[(i1 + ny) + b_y1->size[0] *
        (vstride + vlend2)];
    }
  }

  nx = vspread * midoffset;
  i1 = M->size[0] * M->size[1];
  M->size[0] = vspread;
  M->size[1] = midoffset;
  emxEnsureCapacity_real_T(M, i1);
  for (k = 0; k < nx; k++) {
    M->data[k] = rt_hypotd_snf(x->data[k].re, x->data[k].im);
  }

  if (padSize[0] > sizeAPadded_idx_0) {
    i1 = 0;
    vlend2 = 0;
  } else {
    i1 = (int)padSize[0] - 1;
    vlend2 = (int)sizeAPadded_idx_0;
  }

  if (padSize[1] > sizeAPadded_idx_1) {
    vstride = 0;
    ny = 0;
  } else {
    vstride = (int)padSize[1] - 1;
    ny = (int)sizeAPadded_idx_1;
  }

  nx = x->size[0] * x->size[1];
  vspread = vlend2 - i1;
  x->size[0] = vspread;
  midoffset = ny - vstride;
  x->size[1] = midoffset;
  emxEnsureCapacity_creal_T(x, nx);
  for (vlend2 = 0; vlend2 < midoffset; vlend2++) {
    for (ny = 0; ny < vspread; ny++) {
      x->data[ny + x->size[0] * vlend2] = b_y1->data[(i1 + ny) + b_y1->size[0] *
        (vstride + vlend2)];
    }
  }

  emxFree_creal_T(&b_y1);
  nx = vspread * midoffset;
  i1 = P->size[0] * P->size[1];
  P->size[0] = vspread;
  P->size[1] = midoffset;
  emxEnsureCapacity_real_T(P, i1);
  for (k = 0; k < nx; k++) {
    P->data[k] = rt_atan2d_snf(x->data[k].im, x->data[k].re);
  }

  emxFree_creal_T(&x);
}

//
// File trailer for imgaborfilt.cpp
//
// [EOF]
//
