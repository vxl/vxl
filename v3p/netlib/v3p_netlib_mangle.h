#ifndef v3p_netlib_mangle_h
#define v3p_netlib_mangle_h

/*

This header file mangles all symbols exported from the v3p_netlib
library.  It is included in all files while building the library.

The following command was used to obtain the symbol list:

nm libv3p_netlib.a |grep " [TR] "

*/

/* Functions */
#define caxpy_ v3p_netlib_caxpy_
#define cdotc_ v3p_netlib_cdotc_
#define cscal_ v3p_netlib_cscal_
#define csrot_ v3p_netlib_csrot_
#define csvdc_ v3p_netlib_csvdc_
#define cswap_ v3p_netlib_cswap_
#define daxpy_ v3p_netlib_daxpy_
#define dcabs1_ v3p_netlib_dcabs1_
#define ddot_ v3p_netlib_ddot_
#define dnrm2_ v3p_netlib_dnrm2_
#define drot_ v3p_netlib_drot_
#define drotg_ v3p_netlib_drotg_
#define dscal_ v3p_netlib_dscal_
#define dsvdc_ v3p_netlib_dsvdc_
#define dswap_ v3p_netlib_dswap_
#define dznrm2_ v3p_netlib_dznrm2_
#define saxpy_ v3p_netlib_saxpy_
#define scnrm2_ v3p_netlib_scnrm2_
#define sdot_ v3p_netlib_sdot_
#define snrm2_ v3p_netlib_snrm2_
#define srot_ v3p_netlib_srot_
#define srotg_ v3p_netlib_srotg_
#define sscal_ v3p_netlib_sscal_
#define ssvdc_ v3p_netlib_ssvdc_
#define sswap_ v3p_netlib_sswap_
#define zaxpy_ v3p_netlib_zaxpy_
#define zdotc_ v3p_netlib_zdotc_
#define zdrot_ v3p_netlib_zdrot_
#define zscal_ v3p_netlib_zscal_
#define zsvdc_ v3p_netlib_zsvdc_
#define zswap_ v3p_netlib_zswap_

/* Types */
#define C_f v3p_netlib_C_f
#define C_fp v3p_netlib_C_fp
#define E_f v3p_netlib_E_f
#define E_fp v3p_netlib_E_fp
#define H_f v3p_netlib_H_f
#define H_fp v3p_netlib_H_fp
#define I_fp v3p_netlib_I_fp
#define J_fp v3p_netlib_J_fp
#define K_fp v3p_netlib_K_fp
#define L_fp v3p_netlib_L_fp
#define Multitype v3p_netlib_Multitype
#define Namelist v3p_netlib_Namelist
#define R_fp v3p_netlib_R_fp
#define S_fp v3p_netlib_S_fp
#define U_fp v3p_netlib_U_fp
#define Vardesc v3p_netlib_Vardesc
#define Z_f v3p_netlib_Z_f
#define Z_fp v3p_netlib_Z_fp
#define address v3p_netlib_address
#define alist v3p_netlib_alist
#define cilist v3p_netlib_cilist
#define cllist v3p_netlib_cllist
#define complex v3p_netlib_complex
#define doublecomplex v3p_netlib_doublecomplex
#define doublereal v3p_netlib_doublereal
#define flag v3p_netlib_flag
#define ftnint v3p_netlib_ftnint
#define ftnlen v3p_netlib_ftnlen
#define icilist v3p_netlib_icilist
#define inlist v3p_netlib_inlist
#define integer v3p_netlib_integer
#define integer1 v3p_netlib_integer1
#define logical v3p_netlib_logical
#define logical1 v3p_netlib_logical1
#define longint v3p_netlib_longint
#define olist v3p_netlib_olist
#define real v3p_netlib_real
#define shortint v3p_netlib_shortint
#define shortlogical v3p_netlib_shortlogical
#define uinteger v3p_netlib_uinteger
#define ulongint v3p_netlib_ulongint

#endif
