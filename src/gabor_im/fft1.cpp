//
// File: fft1.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include <cmath>
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "fft1.h"
#include "gabor_im_emxutil.h"
#include "bluesteinSetup.h"

// Function Declarations
static void b_r2br_r2dit_trig(const emxArray_creal_T *x, int n1_unsigned, const
  emxArray_real_T *costab, const emxArray_real_T *sintab, emxArray_creal_T *y);
static void b_r2br_r2dit_trig_impl(const emxArray_creal_T *x, int xoffInit, int
  unsigned_nRows, const emxArray_real_T *costab, const emxArray_real_T *sintab,
  emxArray_creal_T *y);
static void r2br_r2dit_trig_impl(const emxArray_creal_T *x, int unsigned_nRows,
  const emxArray_real_T *costab, const emxArray_real_T *sintab, emxArray_creal_T
  *y);

// Function Definitions

//
// Arguments    : const emxArray_creal_T *x
//                int n1_unsigned
//                const emxArray_real_T *costab
//                const emxArray_real_T *sintab
//                emxArray_creal_T *y
// Return Type  : void
//
static void b_r2br_r2dit_trig(const emxArray_creal_T *x, int n1_unsigned, const
  emxArray_real_T *costab, const emxArray_real_T *sintab, emxArray_creal_T *y)
{
  double b;
  int i11;
  int loop_ub;
  r2br_r2dit_trig_impl(x, n1_unsigned, costab, sintab, y);
  if (y->size[0] > 1) {
    b = 1.0 / (double)y->size[0];
    i11 = y->size[0];
    emxEnsureCapacity_creal_T(y, i11);
    loop_ub = y->size[0];
    for (i11 = 0; i11 < loop_ub; i11++) {
      y->data[i11].re *= b;
      y->data[i11].im *= b;
    }
  }
}

//
// Arguments    : const emxArray_creal_T *x
//                int xoffInit
//                int unsigned_nRows
//                const emxArray_real_T *costab
//                const emxArray_real_T *sintab
//                emxArray_creal_T *y
// Return Type  : void
//
static void b_r2br_r2dit_trig_impl(const emxArray_creal_T *x, int xoffInit, int
  unsigned_nRows, const emxArray_real_T *costab, const emxArray_real_T *sintab,
  emxArray_creal_T *y)
{
  int j;
  int nRowsM2;
  int nRowsD2;
  int nRowsD4;
  int iy;
  int iDelta;
  int ix;
  int ju;
  int i;
  boolean_T tst;
  double temp_re;
  double temp_im;
  int temp_re_tmp;
  double twid_re;
  double twid_im;
  j = x->size[0];
  if (j >= unsigned_nRows) {
    j = unsigned_nRows;
  }

  nRowsM2 = unsigned_nRows - 2;
  nRowsD2 = unsigned_nRows / 2;
  nRowsD4 = nRowsD2 / 2;
  iy = y->size[0];
  y->size[0] = unsigned_nRows;
  emxEnsureCapacity_creal_T(y, iy);
  if (unsigned_nRows > x->size[0]) {
    iDelta = y->size[0];
    iy = y->size[0];
    y->size[0] = iDelta;
    emxEnsureCapacity_creal_T(y, iy);
    for (iy = 0; iy < iDelta; iy++) {
      y->data[iy].re = 0.0;
      y->data[iy].im = 0.0;
    }
  }

  ix = xoffInit;
  ju = 0;
  iy = 0;
  for (i = 0; i <= j - 2; i++) {
    y->data[iy] = x->data[ix];
    iDelta = unsigned_nRows;
    tst = true;
    while (tst) {
      iDelta >>= 1;
      ju ^= iDelta;
      tst = ((ju & iDelta) == 0);
    }

    iy = ju;
    ix++;
  }

  y->data[iy] = x->data[ix];
  if (unsigned_nRows > 1) {
    for (i = 0; i <= nRowsM2; i += 2) {
      temp_re = y->data[i + 1].re;
      temp_im = y->data[i + 1].im;
      y->data[i + 1].re = y->data[i].re - y->data[i + 1].re;
      y->data[i + 1].im = y->data[i].im - y->data[i + 1].im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }
  }

  iDelta = 2;
  iy = 4;
  ix = 1 + ((nRowsD4 - 1) << 2);
  while (nRowsD4 > 0) {
    for (i = 0; i < ix; i += iy) {
      temp_re_tmp = i + iDelta;
      temp_re = y->data[temp_re_tmp].re;
      temp_im = y->data[temp_re_tmp].im;
      y->data[temp_re_tmp].re = y->data[i].re - temp_re;
      y->data[temp_re_tmp].im = y->data[i].im - temp_im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }

    ju = 1;
    for (j = nRowsD4; j < nRowsD2; j += nRowsD4) {
      twid_re = costab->data[j];
      twid_im = sintab->data[j];
      i = ju;
      nRowsM2 = ju + ix;
      while (i < nRowsM2) {
        temp_re_tmp = i + iDelta;
        temp_re = twid_re * y->data[temp_re_tmp].re - twid_im * y->
          data[temp_re_tmp].im;
        temp_im = twid_re * y->data[temp_re_tmp].im + twid_im * y->
          data[temp_re_tmp].re;
        y->data[temp_re_tmp].re = y->data[i].re - temp_re;
        y->data[temp_re_tmp].im = y->data[i].im - temp_im;
        y->data[i].re += temp_re;
        y->data[i].im += temp_im;
        i += iy;
      }

      ju++;
    }

    nRowsD4 /= 2;
    iDelta = iy;
    iy += iy;
    ix -= iDelta;
  }
}

//
// Arguments    : const emxArray_creal_T *x
//                int unsigned_nRows
//                const emxArray_real_T *costab
//                const emxArray_real_T *sintab
//                emxArray_creal_T *y
// Return Type  : void
//
static void r2br_r2dit_trig_impl(const emxArray_creal_T *x, int unsigned_nRows,
  const emxArray_real_T *costab, const emxArray_real_T *sintab, emxArray_creal_T
  *y)
{
  int j;
  int nRowsM2;
  int nRowsD2;
  int nRowsD4;
  int iy;
  int iDelta;
  int ix;
  int ju;
  int i;
  boolean_T tst;
  double temp_re;
  double temp_im;
  int temp_re_tmp;
  double twid_re;
  double twid_im;
  j = x->size[0];
  if (j >= unsigned_nRows) {
    j = unsigned_nRows;
  }

  nRowsM2 = unsigned_nRows - 2;
  nRowsD2 = unsigned_nRows / 2;
  nRowsD4 = nRowsD2 / 2;
  iy = y->size[0];
  y->size[0] = unsigned_nRows;
  emxEnsureCapacity_creal_T(y, iy);
  if (unsigned_nRows > x->size[0]) {
    iDelta = y->size[0];
    iy = y->size[0];
    y->size[0] = iDelta;
    emxEnsureCapacity_creal_T(y, iy);
    for (iy = 0; iy < iDelta; iy++) {
      y->data[iy].re = 0.0;
      y->data[iy].im = 0.0;
    }
  }

  ix = 0;
  ju = 0;
  iy = 0;
  for (i = 0; i <= j - 2; i++) {
    y->data[iy] = x->data[ix];
    iDelta = unsigned_nRows;
    tst = true;
    while (tst) {
      iDelta >>= 1;
      ju ^= iDelta;
      tst = ((ju & iDelta) == 0);
    }

    iy = ju;
    ix++;
  }

  y->data[iy] = x->data[ix];
  if (unsigned_nRows > 1) {
    for (i = 0; i <= nRowsM2; i += 2) {
      temp_re = y->data[i + 1].re;
      temp_im = y->data[i + 1].im;
      y->data[i + 1].re = y->data[i].re - y->data[i + 1].re;
      y->data[i + 1].im = y->data[i].im - y->data[i + 1].im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }
  }

  iDelta = 2;
  iy = 4;
  ix = 1 + ((nRowsD4 - 1) << 2);
  while (nRowsD4 > 0) {
    for (i = 0; i < ix; i += iy) {
      temp_re_tmp = i + iDelta;
      temp_re = y->data[temp_re_tmp].re;
      temp_im = y->data[temp_re_tmp].im;
      y->data[temp_re_tmp].re = y->data[i].re - temp_re;
      y->data[temp_re_tmp].im = y->data[i].im - temp_im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }

    ju = 1;
    for (j = nRowsD4; j < nRowsD2; j += nRowsD4) {
      twid_re = costab->data[j];
      twid_im = sintab->data[j];
      i = ju;
      nRowsM2 = ju + ix;
      while (i < nRowsM2) {
        temp_re_tmp = i + iDelta;
        temp_re = twid_re * y->data[temp_re_tmp].re - twid_im * y->
          data[temp_re_tmp].im;
        temp_im = twid_re * y->data[temp_re_tmp].im + twid_im * y->
          data[temp_re_tmp].re;
        y->data[temp_re_tmp].re = y->data[i].re - temp_re;
        y->data[temp_re_tmp].im = y->data[i].im - temp_im;
        y->data[i].re += temp_re;
        y->data[i].im += temp_im;
        i += iy;
      }

      ju++;
    }

    nRowsD4 /= 2;
    iDelta = iy;
    iy += iy;
    ix -= iDelta;
  }
}

//
// Arguments    : const emxArray_creal_T *x
//                int N2
//                int n1
//                const emxArray_real_T *costab
//                const emxArray_real_T *sintab
//                const emxArray_real_T *sintabinv
//                emxArray_creal_T *y
// Return Type  : void
//
void b_dobluesteinfft(const emxArray_creal_T *x, int N2, int n1, const
                      emxArray_real_T *costab, const emxArray_real_T *sintab,
                      const emxArray_real_T *sintabinv, emxArray_creal_T *y)
{
  unsigned int sx_idx_1;
  emxArray_creal_T *wwc;
  int nrows;
  int ub_loop;
  int k;
  int loop_ub;
  emxArray_creal_T *fv;
  emxArray_creal_T *r2;
  int b_loop_ub;
  emxArray_creal_T *b_fv;
  int i15;
  emxArray_creal_T *rwork;
  int idx;
  int b_k;
  int xoff;
  int minNrowsNx;
  int i16;
  double a_re;
  double a_im;
  double x_re;
  double x_im;
  double b_x_im;
  double b_x_re;
  double fv_re;
  double fv_im;
  sx_idx_1 = (unsigned int)x->size[1];
  emxInit_creal_T(&wwc, 1);
  bluestein_setup_impl(n1, wwc);
  nrows = x->size[0];
  ub_loop = y->size[0] * y->size[1];
  y->size[0] = n1;
  y->size[1] = x->size[1];
  emxEnsureCapacity_creal_T(y, ub_loop);
  if (n1 > x->size[0]) {
    ub_loop = y->size[0] * y->size[1];
    emxEnsureCapacity_creal_T(y, ub_loop);
    loop_ub = y->size[1];
    for (ub_loop = 0; ub_loop < loop_ub; ub_loop++) {
      b_loop_ub = y->size[0];
      for (i15 = 0; i15 < b_loop_ub; i15++) {
        y->data[i15 + y->size[0] * ub_loop].re = 0.0;
        y->data[i15 + y->size[0] * ub_loop].im = 0.0;
      }
    }
  }

  ub_loop = (int)sx_idx_1 - 1;

#pragma omp parallel \
 num_threads(omp_get_max_threads()) \
 private(fv,r2,b_fv,rwork,idx,b_k,xoff,minNrowsNx,i16,a_re,a_im,x_re,x_im,b_x_im,b_x_re,fv_re,fv_im)

  {
    emxInit_creal_T(&fv, 1);
    emxInit_creal_T(&r2, 1);
    emxInit_creal_T(&b_fv, 1);
    emxInit_creal_T(&rwork, 1);

#pragma omp for nowait

    for (k = 0; k <= ub_loop; k++) {
      xoff = k * nrows;
      minNrowsNx = x->size[0];
      if (n1 < minNrowsNx) {
        minNrowsNx = n1;
      }

      i16 = rwork->size[0];
      rwork->size[0] = n1;
      emxEnsureCapacity_creal_T(rwork, i16);
      if (n1 > x->size[0]) {
        b_k = rwork->size[0];
        i16 = rwork->size[0];
        rwork->size[0] = b_k;
        emxEnsureCapacity_creal_T(rwork, i16);
        for (i16 = 0; i16 < b_k; i16++) {
          rwork->data[i16].re = 0.0;
          rwork->data[i16].im = 0.0;
        }
      }

      for (b_k = 0; b_k < minNrowsNx; b_k++) {
        idx = (n1 + b_k) - 1;
        a_re = wwc->data[idx].re;
        a_im = wwc->data[idx].im;
        x_re = x->data[xoff].re;
        x_im = x->data[xoff].im;
        b_x_im = x->data[xoff].im;
        b_x_re = x->data[xoff].re;
        rwork->data[b_k].re = a_re * x_re + a_im * x_im;
        rwork->data[b_k].im = a_re * b_x_im - a_im * b_x_re;
        xoff++;
      }

      i16 = minNrowsNx + 1;
      for (b_k = i16; b_k <= n1; b_k++) {
        rwork->data[b_k - 1].re = 0.0;
        rwork->data[b_k - 1].im = 0.0;
      }

      r2br_r2dit_trig_impl(rwork, N2, costab, sintab, b_fv);
      r2br_r2dit_trig_impl(wwc, N2, costab, sintab, r2);
      i16 = fv->size[0];
      fv->size[0] = b_fv->size[0];
      emxEnsureCapacity_creal_T(fv, i16);
      b_k = b_fv->size[0];
      for (i16 = 0; i16 < b_k; i16++) {
        fv_re = b_fv->data[i16].re;
        fv_im = b_fv->data[i16].im;
        a_re = r2->data[i16].re;
        a_im = r2->data[i16].im;
        fv->data[i16].re = fv_re * a_re - fv_im * a_im;
        fv->data[i16].im = fv_re * a_im + fv_im * a_re;
      }

      b_r2br_r2dit_trig(fv, N2, costab, sintabinv, b_fv);
      idx = 0;
      i16 = wwc->size[0];
      for (b_k = n1; b_k <= i16; b_k++) {
        a_re = wwc->data[b_k - 1].re;
        fv_re = b_fv->data[b_k - 1].re;
        a_im = wwc->data[b_k - 1].im;
        fv_im = b_fv->data[b_k - 1].im;
        x_re = wwc->data[b_k - 1].re;
        x_im = b_fv->data[b_k - 1].im;
        b_x_im = wwc->data[b_k - 1].im;
        b_x_re = b_fv->data[b_k - 1].re;
        rwork->data[idx].re = a_re * fv_re + a_im * fv_im;
        rwork->data[idx].im = x_re * x_im - b_x_im * b_x_re;
        idx++;
      }

      for (b_k = 0; b_k < n1; b_k++) {
        idx = 1 + b_k;
        y->data[(idx + y->size[0] * k) - 1] = rwork->data[idx - 1];
      }
    }

    emxFree_creal_T(&rwork);
    emxFree_creal_T(&b_fv);
    emxFree_creal_T(&r2);
    emxFree_creal_T(&fv);
  }

  emxFree_creal_T(&wwc);
}

//
// Arguments    : int nRows
//                boolean_T useRadix2
//                emxArray_real_T *costab
//                emxArray_real_T *sintab
//                emxArray_real_T *sintabinv
// Return Type  : void
//
void b_generate_twiddle_tables(int nRows, boolean_T useRadix2, emxArray_real_T
  *costab, emxArray_real_T *sintab, emxArray_real_T *sintabinv)
{
  emxArray_real_T *costab1q;
  double e;
  int n;
  int i18;
  int nd2;
  int k;
  emxInit_real_T(&costab1q, 2);
  e = 6.2831853071795862 / (double)nRows;
  n = nRows / 2 / 2;
  i18 = costab1q->size[0] * costab1q->size[1];
  costab1q->size[0] = 1;
  costab1q->size[1] = n + 1;
  emxEnsureCapacity_real_T(costab1q, i18);
  costab1q->data[0] = 1.0;
  nd2 = n / 2 - 1;
  for (k = 0; k <= nd2; k++) {
    costab1q->data[1 + k] = std::cos(e * (1.0 + (double)k));
  }

  i18 = nd2 + 2;
  nd2 = n - 1;
  for (k = i18; k <= nd2; k++) {
    costab1q->data[k] = std::sin(e * (double)(n - k));
  }

  costab1q->data[n] = 0.0;
  if (!useRadix2) {
    n = costab1q->size[1] - 1;
    nd2 = (costab1q->size[1] - 1) << 1;
    i18 = costab->size[0] * costab->size[1];
    costab->size[0] = 1;
    costab->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(costab, i18);
    i18 = sintab->size[0] * sintab->size[1];
    sintab->size[0] = 1;
    sintab->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(sintab, i18);
    costab->data[0] = 1.0;
    sintab->data[0] = 0.0;
    i18 = sintabinv->size[0] * sintabinv->size[1];
    sintabinv->size[0] = 1;
    sintabinv->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(sintabinv, i18);
    for (k = 0; k < n; k++) {
      sintabinv->data[1 + k] = costab1q->data[(n - k) - 1];
    }

    i18 = costab1q->size[1];
    for (k = i18; k <= nd2; k++) {
      sintabinv->data[k] = costab1q->data[k - n];
    }

    for (k = 0; k < n; k++) {
      costab->data[1 + k] = costab1q->data[1 + k];
      sintab->data[1 + k] = -costab1q->data[(n - k) - 1];
    }

    i18 = costab1q->size[1];
    for (k = i18; k <= nd2; k++) {
      costab->data[k] = -costab1q->data[nd2 - k];
      sintab->data[k] = -costab1q->data[k - n];
    }
  } else {
    n = costab1q->size[1] - 1;
    nd2 = (costab1q->size[1] - 1) << 1;
    i18 = costab->size[0] * costab->size[1];
    costab->size[0] = 1;
    costab->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(costab, i18);
    i18 = sintab->size[0] * sintab->size[1];
    sintab->size[0] = 1;
    sintab->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(sintab, i18);
    costab->data[0] = 1.0;
    sintab->data[0] = 0.0;
    for (k = 0; k < n; k++) {
      costab->data[1 + k] = costab1q->data[1 + k];
      sintab->data[1 + k] = costab1q->data[(n - k) - 1];
    }

    i18 = costab1q->size[1];
    for (k = i18; k <= nd2; k++) {
      costab->data[k] = -costab1q->data[nd2 - k];
      sintab->data[k] = costab1q->data[k - n];
    }

    sintabinv->size[0] = 1;
    sintabinv->size[1] = 0;
  }

  emxFree_real_T(&costab1q);
}

//
// Arguments    : const emxArray_creal_T *x
//                int N2
//                int n1
//                const emxArray_real_T *costab
//                const emxArray_real_T *sintab
//                const emxArray_real_T *sintabinv
//                emxArray_creal_T *y
// Return Type  : void
//
void c_dobluesteinfft(const emxArray_creal_T *x, int N2, int n1, const
                      emxArray_real_T *costab, const emxArray_real_T *sintab,
                      const emxArray_real_T *sintabinv, emxArray_creal_T *y)
{
  unsigned int sx_idx_1;
  emxArray_creal_T *wwc;
  int nInt2m1;
  int i20;
  int idx;
  int rt;
  int nInt2;
  int k;
  int b_y;
  double nt_im;
  double nt_re;
  int b_k;
  emxArray_creal_T *fv;
  emxArray_creal_T *r3;
  emxArray_creal_T *b_fv;
  emxArray_creal_T *rwork;
  int b_idx;
  int c_k;
  int xoff;
  int minNrowsNx;
  int i21;
  double denom_re;
  double denom_im;
  double x_re;
  double x_im;
  double b_x_im;
  double b_x_re;
  double fv_re;
  double fv_im;
  sx_idx_1 = (unsigned int)x->size[1];
  emxInit_creal_T(&wwc, 1);
  nInt2m1 = (n1 + n1) - 1;
  i20 = wwc->size[0];
  wwc->size[0] = nInt2m1;
  emxEnsureCapacity_creal_T(wwc, i20);
  idx = n1;
  rt = 0;
  wwc->data[n1 - 1].re = 1.0;
  wwc->data[n1 - 1].im = 0.0;
  nInt2 = n1 << 1;
  for (k = 0; k <= n1 - 2; k++) {
    b_y = ((1 + k) << 1) - 1;
    if (nInt2 - rt <= b_y) {
      rt += b_y - nInt2;
    } else {
      rt += b_y;
    }

    nt_im = 3.1415926535897931 * (double)rt / (double)n1;
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
  i20 = nInt2m1 - 1;
  for (k = i20; k >= n1; k--) {
    wwc->data[k] = wwc->data[idx];
    idx++;
  }

  nInt2m1 = x->size[0];
  i20 = y->size[0] * y->size[1];
  y->size[0] = n1;
  y->size[1] = x->size[1];
  emxEnsureCapacity_creal_T(y, i20);
  if (n1 > x->size[0]) {
    i20 = y->size[0] * y->size[1];
    emxEnsureCapacity_creal_T(y, i20);
    rt = y->size[1];
    for (i20 = 0; i20 < rt; i20++) {
      nInt2 = y->size[0];
      for (b_y = 0; b_y < nInt2; b_y++) {
        y->data[b_y + y->size[0] * i20].re = 0.0;
        y->data[b_y + y->size[0] * i20].im = 0.0;
      }
    }
  }

  rt = (int)sx_idx_1 - 1;

#pragma omp parallel \
 num_threads(omp_get_max_threads()) \
 private(fv,r3,b_fv,rwork,b_idx,c_k,xoff,minNrowsNx,i21,denom_re,denom_im,x_re,x_im,b_x_im,b_x_re,fv_re,fv_im)

  {
    emxInit_creal_T(&fv, 1);
    emxInit_creal_T(&r3, 1);
    emxInit_creal_T(&b_fv, 1);
    emxInit_creal_T(&rwork, 1);

#pragma omp for nowait

    for (b_k = 0; b_k <= rt; b_k++) {
      xoff = b_k * nInt2m1;
      minNrowsNx = x->size[0];
      if (n1 < minNrowsNx) {
        minNrowsNx = n1;
      }

      i21 = rwork->size[0];
      rwork->size[0] = n1;
      emxEnsureCapacity_creal_T(rwork, i21);
      if (n1 > x->size[0]) {
        c_k = rwork->size[0];
        i21 = rwork->size[0];
        rwork->size[0] = c_k;
        emxEnsureCapacity_creal_T(rwork, i21);
        for (i21 = 0; i21 < c_k; i21++) {
          rwork->data[i21].re = 0.0;
          rwork->data[i21].im = 0.0;
        }
      }

      for (c_k = 0; c_k < minNrowsNx; c_k++) {
        b_idx = (n1 + c_k) - 1;
        denom_re = wwc->data[b_idx].re;
        denom_im = wwc->data[b_idx].im;
        x_re = x->data[xoff].re;
        x_im = x->data[xoff].im;
        b_x_im = x->data[xoff].im;
        b_x_re = x->data[xoff].re;
        rwork->data[c_k].re = denom_re * x_re + denom_im * x_im;
        rwork->data[c_k].im = denom_re * b_x_im - denom_im * b_x_re;
        xoff++;
      }

      i21 = minNrowsNx + 1;
      for (c_k = i21; c_k <= n1; c_k++) {
        rwork->data[c_k - 1].re = 0.0;
        rwork->data[c_k - 1].im = 0.0;
      }

      r2br_r2dit_trig_impl(rwork, N2, costab, sintab, b_fv);
      r2br_r2dit_trig_impl(wwc, N2, costab, sintab, r3);
      i21 = fv->size[0];
      fv->size[0] = b_fv->size[0];
      emxEnsureCapacity_creal_T(fv, i21);
      c_k = b_fv->size[0];
      for (i21 = 0; i21 < c_k; i21++) {
        fv_re = b_fv->data[i21].re;
        fv_im = b_fv->data[i21].im;
        denom_re = r3->data[i21].re;
        denom_im = r3->data[i21].im;
        fv->data[i21].re = fv_re * denom_re - fv_im * denom_im;
        fv->data[i21].im = fv_re * denom_im + fv_im * denom_re;
      }

      b_r2br_r2dit_trig(fv, N2, costab, sintabinv, b_fv);
      b_idx = 0;
      i21 = wwc->size[0];
      for (c_k = n1; c_k <= i21; c_k++) {
        denom_re = wwc->data[c_k - 1].re;
        fv_re = b_fv->data[c_k - 1].re;
        denom_im = wwc->data[c_k - 1].im;
        fv_im = b_fv->data[c_k - 1].im;
        x_re = wwc->data[c_k - 1].re;
        x_im = b_fv->data[c_k - 1].im;
        b_x_im = wwc->data[c_k - 1].im;
        b_x_re = b_fv->data[c_k - 1].re;
        rwork->data[b_idx].re = denom_re * fv_re + denom_im * fv_im;
        rwork->data[b_idx].im = x_re * x_im - b_x_im * b_x_re;
        denom_re = wwc->data[c_k - 1].re;
        fv_re = b_fv->data[c_k - 1].re;
        denom_im = wwc->data[c_k - 1].im;
        fv_im = b_fv->data[c_k - 1].im;
        x_re = wwc->data[c_k - 1].re;
        x_im = b_fv->data[c_k - 1].im;
        b_x_im = wwc->data[c_k - 1].im;
        b_x_re = b_fv->data[c_k - 1].re;
        rwork->data[b_idx].re = denom_re * fv_re + denom_im * fv_im;
        rwork->data[b_idx].im = x_re * x_im - b_x_im * b_x_re;
        denom_re = rwork->data[b_idx].re;
        denom_im = rwork->data[b_idx].im;
        if (denom_im == 0.0) {
          rwork->data[b_idx].re = denom_re / (double)n1;
          rwork->data[b_idx].im = 0.0;
        } else if (denom_re == 0.0) {
          rwork->data[b_idx].re = 0.0;
          rwork->data[b_idx].im = denom_im / (double)n1;
        } else {
          rwork->data[b_idx].re = denom_re / (double)n1;
          rwork->data[b_idx].im = denom_im / (double)n1;
        }

        b_idx++;
      }

      for (c_k = 0; c_k < n1; c_k++) {
        b_idx = 1 + c_k;
        y->data[(b_idx + y->size[0] * b_k) - 1] = rwork->data[b_idx - 1];
      }
    }

    emxFree_creal_T(&rwork);
    emxFree_creal_T(&b_fv);
    emxFree_creal_T(&r3);
    emxFree_creal_T(&fv);
  }

  emxFree_creal_T(&wwc);
}

//
// Arguments    : const emxArray_creal_T *x
//                int n1_unsigned
//                const emxArray_real_T *costab
//                const emxArray_real_T *sintab
//                emxArray_creal_T *y
// Return Type  : void
//
void c_r2br_r2dit_trig(const emxArray_creal_T *x, int n1_unsigned, const
  emxArray_real_T *costab, const emxArray_real_T *sintab, emxArray_creal_T *y)
{
  unsigned int sx_idx_1;
  int n1;
  int nrows;
  int ub_loop;
  int k;
  int loop_ub;
  emxArray_creal_T *rwork;
  int b_loop_ub;
  int i14;
  int xoff;
  int l;
  sx_idx_1 = (unsigned int)x->size[1];
  n1 = n1_unsigned;
  nrows = x->size[0];
  ub_loop = y->size[0] * y->size[1];
  y->size[0] = n1_unsigned;
  y->size[1] = x->size[1];
  emxEnsureCapacity_creal_T(y, ub_loop);
  if (n1_unsigned > x->size[0]) {
    ub_loop = y->size[0] * y->size[1];
    emxEnsureCapacity_creal_T(y, ub_loop);
    loop_ub = y->size[1];
    for (ub_loop = 0; ub_loop < loop_ub; ub_loop++) {
      b_loop_ub = y->size[0];
      for (i14 = 0; i14 < b_loop_ub; i14++) {
        y->data[i14 + y->size[0] * ub_loop].re = 0.0;
        y->data[i14 + y->size[0] * ub_loop].im = 0.0;
      }
    }
  }

  ub_loop = (int)sx_idx_1 - 1;

#pragma omp parallel \
 num_threads(omp_get_max_threads()) \
 private(rwork,xoff,l)

  {
    emxInit_creal_T(&rwork, 1);

#pragma omp for nowait

    for (k = 0; k <= ub_loop; k++) {
      xoff = k * nrows;
      b_r2br_r2dit_trig_impl(x, xoff, n1_unsigned, costab, sintab, rwork);
      for (l = 0; l < n1; l++) {
        xoff = 1 + l;
        y->data[(xoff + y->size[0] * k) - 1] = rwork->data[xoff - 1];
      }
    }

    emxFree_creal_T(&rwork);
  }
}

//
// Arguments    : const emxArray_creal_T *x
//                int n1_unsigned
//                const emxArray_real_T *costab
//                const emxArray_real_T *sintab
//                emxArray_creal_T *y
// Return Type  : void
//
void d_r2br_r2dit_trig(const emxArray_creal_T *x, int n1_unsigned, const
  emxArray_real_T *costab, const emxArray_real_T *sintab, emxArray_creal_T *y)
{
  unsigned int sx_idx_1;
  int n1;
  int nrows;
  int ub_loop;
  int k;
  int loop_ub;
  emxArray_creal_T *rwork;
  double b;
  int b_loop_ub;
  int i19;
  int xoff;
  int l;
  sx_idx_1 = (unsigned int)x->size[1];
  n1 = n1_unsigned;
  nrows = x->size[0];
  ub_loop = y->size[0] * y->size[1];
  y->size[0] = n1_unsigned;
  y->size[1] = x->size[1];
  emxEnsureCapacity_creal_T(y, ub_loop);
  if (n1_unsigned > x->size[0]) {
    ub_loop = y->size[0] * y->size[1];
    emxEnsureCapacity_creal_T(y, ub_loop);
    loop_ub = y->size[1];
    for (ub_loop = 0; ub_loop < loop_ub; ub_loop++) {
      b_loop_ub = y->size[0];
      for (i19 = 0; i19 < b_loop_ub; i19++) {
        y->data[i19 + y->size[0] * ub_loop].re = 0.0;
        y->data[i19 + y->size[0] * ub_loop].im = 0.0;
      }
    }
  }

  ub_loop = (int)sx_idx_1 - 1;

#pragma omp parallel \
 num_threads(omp_get_max_threads()) \
 private(rwork,xoff,l)

  {
    emxInit_creal_T(&rwork, 1);

#pragma omp for nowait

    for (k = 0; k <= ub_loop; k++) {
      xoff = k * nrows;
      b_r2br_r2dit_trig_impl(x, xoff, n1_unsigned, costab, sintab, rwork);
      for (l = 0; l < n1; l++) {
        xoff = 1 + l;
        y->data[(xoff + y->size[0] * k) - 1] = rwork->data[xoff - 1];
      }
    }

    emxFree_creal_T(&rwork);
  }

  if (y->size[0] > 1) {
    b = 1.0 / (double)y->size[0];
    ub_loop = y->size[0] * y->size[1];
    i19 = y->size[0] * y->size[1];
    emxEnsureCapacity_creal_T(y, i19);
    loop_ub = ub_loop - 1;
    for (ub_loop = 0; ub_loop <= loop_ub; ub_loop++) {
      y->data[ub_loop].re *= b;
      y->data[ub_loop].im *= b;
    }
  }
}

//
// Arguments    : const emxArray_real_T *x
//                int N2
//                int n1
//                const emxArray_real_T *costab
//                const emxArray_real_T *sintab
//                const emxArray_real_T *sintabinv
//                emxArray_creal_T *y
// Return Type  : void
//
void dobluesteinfft(const emxArray_real_T *x, int N2, int n1, const
                    emxArray_real_T *costab, const emxArray_real_T *sintab,
                    const emxArray_real_T *sintabinv, emxArray_creal_T *y)
{
  unsigned int sx_idx_1;
  emxArray_creal_T *wwc;
  int nrows;
  int sz_idx_1;
  int i8;
  int k;
  emxArray_creal_T *fv;
  emxArray_creal_T *r1;
  int loop_ub;
  emxArray_creal_T *b_fv;
  int i9;
  emxArray_creal_T *rwork;
  int idx;
  int b_k;
  int xoff;
  int minNrowsNx;
  int i10;
  double a_re;
  double a_im;
  double fv_re;
  double fv_im;
  double wwc_re;
  double b_fv_im;
  double wwc_im;
  double b_fv_re;
  sx_idx_1 = (unsigned int)x->size[1];
  emxInit_creal_T(&wwc, 1);
  bluestein_setup_impl(n1, wwc);
  nrows = x->size[0];
  sz_idx_1 = x->size[1];
  i8 = y->size[0] * y->size[1];
  y->size[0] = n1;
  y->size[1] = sz_idx_1;
  emxEnsureCapacity_creal_T(y, i8);
  if (n1 > x->size[0]) {
    i8 = y->size[0] * y->size[1];
    emxEnsureCapacity_creal_T(y, i8);
    sz_idx_1 = y->size[1];
    for (i8 = 0; i8 < sz_idx_1; i8++) {
      loop_ub = y->size[0];
      for (i9 = 0; i9 < loop_ub; i9++) {
        y->data[i9 + y->size[0] * i8].re = 0.0;
        y->data[i9 + y->size[0] * i8].im = 0.0;
      }
    }
  }

  sz_idx_1 = (int)sx_idx_1 - 1;

#pragma omp parallel \
 num_threads(omp_get_max_threads()) \
 private(fv,r1,b_fv,rwork,idx,b_k,xoff,minNrowsNx,i10,a_re,a_im,fv_re,fv_im,wwc_re,b_fv_im,wwc_im,b_fv_re)

  {
    emxInit_creal_T(&fv, 1);
    emxInit_creal_T(&r1, 1);
    emxInit_creal_T(&b_fv, 1);
    emxInit_creal_T(&rwork, 1);

#pragma omp for nowait

    for (k = 0; k <= sz_idx_1; k++) {
      xoff = k * nrows;
      minNrowsNx = x->size[0];
      if (n1 < minNrowsNx) {
        minNrowsNx = n1;
      }

      i10 = rwork->size[0];
      rwork->size[0] = n1;
      emxEnsureCapacity_creal_T(rwork, i10);
      if (n1 > x->size[0]) {
        b_k = rwork->size[0];
        i10 = rwork->size[0];
        rwork->size[0] = b_k;
        emxEnsureCapacity_creal_T(rwork, i10);
        for (i10 = 0; i10 < b_k; i10++) {
          rwork->data[i10].re = 0.0;
          rwork->data[i10].im = 0.0;
        }
      }

      for (b_k = 0; b_k < minNrowsNx; b_k++) {
        idx = (n1 + b_k) - 1;
        a_re = wwc->data[idx].re;
        a_im = wwc->data[idx].im;
        rwork->data[b_k].re = a_re * x->data[xoff];
        rwork->data[b_k].im = a_im * -x->data[xoff];
        xoff++;
      }

      i10 = minNrowsNx + 1;
      for (b_k = i10; b_k <= n1; b_k++) {
        rwork->data[b_k - 1].re = 0.0;
        rwork->data[b_k - 1].im = 0.0;
      }

      r2br_r2dit_trig_impl(rwork, N2, costab, sintab, b_fv);
      r2br_r2dit_trig_impl(wwc, N2, costab, sintab, r1);
      i10 = fv->size[0];
      fv->size[0] = b_fv->size[0];
      emxEnsureCapacity_creal_T(fv, i10);
      b_k = b_fv->size[0];
      for (i10 = 0; i10 < b_k; i10++) {
        fv_re = b_fv->data[i10].re;
        fv_im = b_fv->data[i10].im;
        a_re = r1->data[i10].re;
        a_im = r1->data[i10].im;
        fv->data[i10].re = fv_re * a_re - fv_im * a_im;
        fv->data[i10].im = fv_re * a_im + fv_im * a_re;
      }

      b_r2br_r2dit_trig(fv, N2, costab, sintabinv, b_fv);
      idx = 0;
      i10 = wwc->size[0];
      for (b_k = n1; b_k <= i10; b_k++) {
        a_re = wwc->data[b_k - 1].re;
        fv_re = b_fv->data[b_k - 1].re;
        a_im = wwc->data[b_k - 1].im;
        fv_im = b_fv->data[b_k - 1].im;
        wwc_re = wwc->data[b_k - 1].re;
        b_fv_im = b_fv->data[b_k - 1].im;
        wwc_im = wwc->data[b_k - 1].im;
        b_fv_re = b_fv->data[b_k - 1].re;
        rwork->data[idx].re = a_re * fv_re + a_im * fv_im;
        rwork->data[idx].im = wwc_re * b_fv_im - wwc_im * b_fv_re;
        idx++;
      }

      for (b_k = 0; b_k < n1; b_k++) {
        idx = 1 + b_k;
        y->data[(idx + y->size[0] * k) - 1] = rwork->data[idx - 1];
      }
    }

    emxFree_creal_T(&rwork);
    emxFree_creal_T(&b_fv);
    emxFree_creal_T(&r1);
    emxFree_creal_T(&fv);
  }

  emxFree_creal_T(&wwc);
}

//
// Arguments    : int nRows
//                boolean_T useRadix2
//                emxArray_real_T *costab
//                emxArray_real_T *sintab
//                emxArray_real_T *sintabinv
// Return Type  : void
//
void generate_twiddle_tables(int nRows, boolean_T useRadix2, emxArray_real_T
  *costab, emxArray_real_T *sintab, emxArray_real_T *sintabinv)
{
  emxArray_real_T *costab1q;
  double e;
  int n;
  int i5;
  int nd2;
  int k;
  emxInit_real_T(&costab1q, 2);
  e = 6.2831853071795862 / (double)nRows;
  n = nRows / 2 / 2;
  i5 = costab1q->size[0] * costab1q->size[1];
  costab1q->size[0] = 1;
  costab1q->size[1] = n + 1;
  emxEnsureCapacity_real_T(costab1q, i5);
  costab1q->data[0] = 1.0;
  nd2 = n / 2 - 1;
  for (k = 0; k <= nd2; k++) {
    costab1q->data[1 + k] = std::cos(e * (1.0 + (double)k));
  }

  i5 = nd2 + 2;
  nd2 = n - 1;
  for (k = i5; k <= nd2; k++) {
    costab1q->data[k] = std::sin(e * (double)(n - k));
  }

  costab1q->data[n] = 0.0;
  if (!useRadix2) {
    n = costab1q->size[1] - 1;
    nd2 = (costab1q->size[1] - 1) << 1;
    i5 = costab->size[0] * costab->size[1];
    costab->size[0] = 1;
    costab->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(costab, i5);
    i5 = sintab->size[0] * sintab->size[1];
    sintab->size[0] = 1;
    sintab->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(sintab, i5);
    costab->data[0] = 1.0;
    sintab->data[0] = 0.0;
    i5 = sintabinv->size[0] * sintabinv->size[1];
    sintabinv->size[0] = 1;
    sintabinv->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(sintabinv, i5);
    for (k = 0; k < n; k++) {
      sintabinv->data[1 + k] = costab1q->data[(n - k) - 1];
    }

    i5 = costab1q->size[1];
    for (k = i5; k <= nd2; k++) {
      sintabinv->data[k] = costab1q->data[k - n];
    }

    for (k = 0; k < n; k++) {
      costab->data[1 + k] = costab1q->data[1 + k];
      sintab->data[1 + k] = -costab1q->data[(n - k) - 1];
    }

    i5 = costab1q->size[1];
    for (k = i5; k <= nd2; k++) {
      costab->data[k] = -costab1q->data[nd2 - k];
      sintab->data[k] = -costab1q->data[k - n];
    }
  } else {
    n = costab1q->size[1] - 1;
    nd2 = (costab1q->size[1] - 1) << 1;
    i5 = costab->size[0] * costab->size[1];
    costab->size[0] = 1;
    costab->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(costab, i5);
    i5 = sintab->size[0] * sintab->size[1];
    sintab->size[0] = 1;
    sintab->size[1] = nd2 + 1;
    emxEnsureCapacity_real_T(sintab, i5);
    costab->data[0] = 1.0;
    sintab->data[0] = 0.0;
    for (k = 0; k < n; k++) {
      costab->data[1 + k] = costab1q->data[1 + k];
      sintab->data[1 + k] = -costab1q->data[(n - k) - 1];
    }

    i5 = costab1q->size[1];
    for (k = i5; k <= nd2; k++) {
      costab->data[k] = -costab1q->data[nd2 - k];
      sintab->data[k] = -costab1q->data[k - n];
    }

    sintabinv->size[0] = 1;
    sintabinv->size[1] = 0;
  }

  emxFree_real_T(&costab1q);
}

//
// Arguments    : int n1
//                boolean_T useRadix2
//                int *N2blue
//                int *nRows
// Return Type  : void
//
void get_algo_sizes(int n1, boolean_T useRadix2, int *N2blue, int *nRows)
{
  int n;
  int pmax;
  int pmin;
  boolean_T exitg1;
  int p;
  int pow2p;
  *N2blue = 1;
  if (useRadix2) {
    *nRows = n1;
  } else {
    n = (n1 + n1) - 1;
    pmax = 31;
    if (n <= 1) {
      pmax = 0;
    } else {
      pmin = 0;
      exitg1 = false;
      while ((!exitg1) && (pmax - pmin > 1)) {
        p = (pmin + pmax) >> 1;
        pow2p = 1 << p;
        if (pow2p == n) {
          pmax = p;
          exitg1 = true;
        } else if (pow2p > n) {
          pmax = p;
        } else {
          pmin = p;
        }
      }
    }

    *N2blue = 1 << pmax;
    *nRows = *N2blue;
  }
}

//
// Arguments    : const emxArray_real_T *x
//                int n1_unsigned
//                const emxArray_real_T *costab
//                const emxArray_real_T *sintab
//                emxArray_creal_T *y
// Return Type  : void
//
void r2br_r2dit_trig(const emxArray_real_T *x, int n1_unsigned, const
                     emxArray_real_T *costab, const emxArray_real_T *sintab,
                     emxArray_creal_T *y)
{
  unsigned int sx_idx_1;
  int n1;
  int nrows;
  int sz_idx_1;
  int i6;
  int k;
  emxArray_creal_T *rwork;
  int loop_ub;
  int i7;
  int iy;
  int xoff;
  int ihi;
  int nRowsM2;
  int nRowsD2;
  int nRowsD4;
  int iDelta;
  int ju;
  int i;
  boolean_T tst;
  double temp_re;
  double temp_im;
  int temp_re_tmp;
  double twid_re;
  double twid_im;
  sx_idx_1 = (unsigned int)x->size[1];
  n1 = n1_unsigned;
  nrows = x->size[0];
  sz_idx_1 = x->size[1];
  i6 = y->size[0] * y->size[1];
  y->size[0] = n1_unsigned;
  y->size[1] = sz_idx_1;
  emxEnsureCapacity_creal_T(y, i6);
  if (n1_unsigned > x->size[0]) {
    i6 = y->size[0] * y->size[1];
    emxEnsureCapacity_creal_T(y, i6);
    sz_idx_1 = y->size[1];
    for (i6 = 0; i6 < sz_idx_1; i6++) {
      loop_ub = y->size[0];
      for (i7 = 0; i7 < loop_ub; i7++) {
        y->data[i7 + y->size[0] * i6].re = 0.0;
        y->data[i7 + y->size[0] * i6].im = 0.0;
      }
    }
  }

  sz_idx_1 = (int)sx_idx_1 - 1;

#pragma omp parallel \
 num_threads(omp_get_max_threads()) \
 private(rwork,iy,xoff,ihi,nRowsM2,nRowsD2,nRowsD4,iDelta,ju,i,tst,temp_re,temp_im,temp_re_tmp,twid_re,twid_im)

  {
    emxInit_creal_T(&rwork, 1);

#pragma omp for nowait

    for (k = 0; k <= sz_idx_1; k++) {
      xoff = k * nrows;
      iy = x->size[0];
      ihi = n1_unsigned;
      if (iy < n1_unsigned) {
        ihi = iy;
      }

      nRowsM2 = n1_unsigned - 2;
      nRowsD2 = n1_unsigned / 2;
      nRowsD4 = nRowsD2 / 2;
      iy = rwork->size[0];
      rwork->size[0] = n1_unsigned;
      emxEnsureCapacity_creal_T(rwork, iy);
      if (n1_unsigned > x->size[0]) {
        iDelta = rwork->size[0];
        iy = rwork->size[0];
        rwork->size[0] = iDelta;
        emxEnsureCapacity_creal_T(rwork, iy);
        for (iy = 0; iy < iDelta; iy++) {
          rwork->data[iy].re = 0.0;
          rwork->data[iy].im = 0.0;
        }
      }

      ju = 0;
      iy = 0;
      for (i = 0; i <= ihi - 2; i++) {
        rwork->data[iy].re = x->data[xoff];
        rwork->data[iy].im = 0.0;
        iDelta = n1_unsigned;
        tst = true;
        while (tst) {
          iDelta >>= 1;
          ju ^= iDelta;
          tst = ((ju & iDelta) == 0);
        }

        iy = ju;
        xoff++;
      }

      rwork->data[iy].re = x->data[xoff];
      rwork->data[iy].im = 0.0;
      if (n1_unsigned > 1) {
        for (i = 0; i <= nRowsM2; i += 2) {
          temp_re = rwork->data[i + 1].re;
          temp_im = rwork->data[i + 1].im;
          rwork->data[i + 1].re = rwork->data[i].re - rwork->data[i + 1].re;
          rwork->data[i + 1].im = rwork->data[i].im - rwork->data[i + 1].im;
          rwork->data[i].re += temp_re;
          rwork->data[i].im += temp_im;
        }
      }

      iDelta = 2;
      iy = 4;
      ju = 1 + ((nRowsD4 - 1) << 2);
      while (nRowsD4 > 0) {
        for (i = 0; i < ju; i += iy) {
          temp_re_tmp = i + iDelta;
          temp_re = rwork->data[temp_re_tmp].re;
          temp_im = rwork->data[temp_re_tmp].im;
          rwork->data[temp_re_tmp].re = rwork->data[i].re - temp_re;
          rwork->data[temp_re_tmp].im = rwork->data[i].im - temp_im;
          rwork->data[i].re += temp_re;
          rwork->data[i].im += temp_im;
        }

        nRowsM2 = 1;
        for (xoff = nRowsD4; xoff < nRowsD2; xoff += nRowsD4) {
          twid_re = costab->data[xoff];
          twid_im = sintab->data[xoff];
          i = nRowsM2;
          ihi = nRowsM2 + ju;
          while (i < ihi) {
            temp_re_tmp = i + iDelta;
            temp_re = twid_re * rwork->data[temp_re_tmp].re - twid_im *
              rwork->data[temp_re_tmp].im;
            temp_im = twid_re * rwork->data[temp_re_tmp].im + twid_im *
              rwork->data[temp_re_tmp].re;
            rwork->data[temp_re_tmp].re = rwork->data[i].re - temp_re;
            rwork->data[temp_re_tmp].im = rwork->data[i].im - temp_im;
            rwork->data[i].re += temp_re;
            rwork->data[i].im += temp_im;
            i += iy;
          }

          nRowsM2++;
        }

        nRowsD4 /= 2;
        iDelta = iy;
        iy += iy;
        ju -= iDelta;
      }

      for (iDelta = 0; iDelta < n1; iDelta++) {
        iy = 1 + iDelta;
        y->data[(iy + y->size[0] * k) - 1] = rwork->data[iy - 1];
      }
    }

    emxFree_creal_T(&rwork);
  }
}

//
// File trailer for fft1.cpp
//
// [EOF]
//
