#ifndef vcl_iso_cstdlib_h_
#define vcl_iso_cstdlib_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <cstdlib>

// allow this for now.
using std::abort;
using std::abs;
using std::abs;
using std::atexit;
using std::atof;
using std::atoi;
using std::atol;
using std::bsearch;
using std::calloc;
//using std::div;
//using std::div_t;
using std::exit;
using std::free;
using std::getenv;
using std::labs;
//using std::ldiv;
//using std::ldiv_t;
using std::malloc;
using std::mblen;
using std::mbstowcs;
using std::mbtowc;
using std::qsort;
using std::rand;
using std::realloc;
using std::size_t;
using std::srand;
using std::strtod;
using std::strtol;
using std::strtoul;
using std::system;
using std::wcstombs;
using std::wctomb;


#ifndef vcl_abs
#define vcl_abs      std::abs
#endif
#define vcl_atexit   std::atexit
#define vcl_atof     std::atof
#define vcl_atoi     std::atoi
#define vcl_atol     std::atol
#define vcl_bsearch  std::bsearch
#define vcl_div      std::div
#define vcl_div_t    std::div_t
#define vcl_exit     std::exit
#define vcl_getenv   std::getenv
#define vcl_labs     std::labs
#define vcl_ldiv     std::ldiv
#define vcl_ldiv_t   std::ldiv_t
#define vcl_mblen    std::mblen
#define vcl_mbstowcs std::mbstowcs
#define vcl_mbtowc   std::mbtowc
#define vcl_qsort    std::qsort
#define vcl_rand     std::rand
#define vcl_size_t   std::size_t
#define vcl_strtod   std::strtod
#define vcl_strtol   std::strtol
#define vcl_strtoul  std::strtoul
#define vcl_system   std::system
#define vcl_abort    std::abort
#define vcl_calloc   std::calloc
#define vcl_free     std::free
#define vcl_malloc   std::malloc
#define vcl_realloc  std::realloc
#define vcl_srand    std::srand
#define vcl_wcstombs std::wcstombs
#define vcl_wctomb   std::wctomb

#endif
