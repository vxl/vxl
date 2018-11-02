#include <iostream>
#include "brip_rect_mask.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

std::map<brip_rect_mask::ang_id, std::string > brip_rect_mask::names_=std::map<brip_rect_mask::ang_id, std::string >();
std::map<brip_rect_mask::ang_id, float > brip_rect_mask::angles_=std::map<brip_rect_mask::ang_id, float >();
bool brip_rect_mask::is_init_ = false;

//        0 0 - 0 0
//        0 - + - 0
//        - + + + -
//        0 - + - 0
//        0 0 - 0 0
//
static int cr1_5x5 [] =
{
    0, 0,-1, 0, 0,
    0,-1, 1,-1, 0,
   -1, 1, 1, 1,-1 ,
    0,-1, 1,-1, 0,
    0, 0,-1, 0, 0
};

//        0 - - - 0
//        - + + + -
//        - + + + -
//        - + + + -
//        0 - - - 0
//
static int s1_5x5 [] =
{
    0,-1,-1,-1, 0,
   -1, 1, 1, 1,-1,
   -1, 1, 1, 1,-1,
   -1, 1, 1, 1,-1,
    0,-1,-1,-1, 0,
};

//        0 - - - 0
//        - + + + -
//        0 - - - 0
//
static int r10a0_3x5 [] =
{
    0,-1,-1,-1, 0,
    -1, 1, 1, 1,-1,
    0,-1,-1,-1, 0
};

//        0 0 0 - 0
//        0 0 - + -
//        0 - + - 0
//        - + - 0 0
//        0 - 0 0 0
//
static int r10a45_5x5 [] =
{
    0,0,0,-1,0,
    0,0,-1,1,-1,
    0,-1,1,-1,0,
    -1,1,-1,0,0,
    0,-1,0,0,0
};

//     0 - - - - - - - 0
//     - + + + + + + + -
//     0 - - - - - - - 0
//
static int r30a0_3x9 [] =
{
    0,-1,-1,-1,-1,-1,-1,-1,0,
    -1,1,1,1,1,1,1,1,-1,
    0,-1,-1,-1,-1,-1,-1,-1,0
};

//     0 0 0 0 0 0 - - 0
//     0 0 0 - - - + + -
//     0 - - + + + - - 0
//     - + + - - - 0 0 0
//     0 - - 0 0 0 0 0 0
//
static int r30a22_5x9 [] =
{
    0,0,0,0,0,0,-1,-1,0,
    0,0,0,-1,-1,-1,1,1,-1,
    0,-1,-1,1,1,1,-1,-1,0,
    -1,1,1,-1,-1,-1,0,0,0,
    0,-1,-1,0,0,0,0,0,0
};

//       0 0 0 0 0 - 0
//       0 0 0 0 - + -
//       0 0 0 - + - 0
//       0 0 - + - 0 0
//       0 - + - 0 0 0
//       - + - 0 0 0 0
//       0 - 0 0 0 0 0
//
static int r30a45_7x7 [] =
{
    0,0,0,0,0,-1,0,
    0,0,0,0,-1,1,-1,
    0,0,0,-1,1,-1,0,
    0,0,-1,1,-1,0,0,
    0,-1,1,-1,0,0,0,
    -1,1,-1,0,0,0,0,
    0,-1,0,0,0,0,0
};

//     0 - - - - - - - 0
//     - + + + + + + + -
//     - + + + + + + + -
//     - + + + + + + + -
//     0 - - - - - - - 0
//
static int r31a0_5x9 [] =
{
    0,-1,-1,-1,-1,-1,-1,-1,0,
    -1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,-1,
    0,-1,-1,-1,-1,-1,-1,-1,0
};

//     0 0 0 0 - - - - 0
//     0 - - - + + + + -
//     - + + + + + + + -
//     - + + + + - - - 0
//     0 - - - - 0 0 0 0
//
static int r31a22_5x9 [] =
{
    0,0,0,0,-1,-1,-1,-1,0,
    0,-1,-1,-1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,-1,-1,-1,0,
    0,-1,-1,-1,-1,0,0,0,0
};

//       0 0 0 0 - - 0
//       0 0 0 - + + -
//       0 0 - + + + -
//       0 - + + + - 0
//       - + + + - 0 0
//       - + + - 0 0 0
//       0 - - 0 0 0 0
//
static int r31a45_7x7 [] =
{
    0,0,0,0,-1,-1,0,
    0,0,0,-1,1,1,-1,
    0,0,-1,1,1,1,-1,
    0,-1,1,1,1,-1,0,
    -1,1,1,1,-1,0,0,
    -1,1,1,-1,0,0,0,
    0,-1,-1,0,0,0,0
};

//     0 - - - - - - - 0
//     - + + + + + + + -
//     - + + + + + + + -
//     - + + + + + + + -
//     - + + + + + + + -
//     - + + + + + + + -
//     0 - - - - - - - 0
//
static int r32a0_7x9 [] =
{
    0,-1,-1,-1,-1,-1,-1,-1,0,
    -1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,-1,
    0,-1,-1,-1,-1,-1,-1,-1,0
};

//     0 0 0 - - - - - 0
//     0 0 - + + + + + -
//     0 - + + + + + + -
//     - + + + + + + + -
//     - + + + + + + - 0
//     - + + + + + - 0 0
//     0 - - - - - 0 0 0
//
static int r32a22_7x9 [] =
{
    0,0,0,-1,-1,-1,-1,-1,0,
    0,0,-1,1,1,1,1,1,-1,
    0,-1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,-1,0,
    -1,1,1,1,1,1,-1,0,0,
    0,-1,-1,-1,-1,-1,0,0,0
};

//     0 0 0 0 0 - 0 0 0
//     0 0 0 0 - + - 0 0
//     0 0 0 - + + + - 0
//     0 0 - + + + + + -
//     0 - + + + + + - 0
//     - + + + + + - 0 0
//     0 - + + + - 0 0 0
//     0 0 - + - 0 0 0 0
//     0 0 0 - 0 0 0 0 0
//
static int r32a45_9x9 [] =
{
    0,0,0,0,0,-1,0,0,0,
    0,0,0,0,-1,1,-1,0,0,
    0,0,0,-1,1,1,1,-1,0,
    0,0,-1,1,1,1,1,1,-1,
    0,-1,1,1,1,1,1,-1,0,
    -1,1,1,1,1,1,-1,0,0,
    0,-1,1,1,1,-1,0,0,0,
    0,0,-1,1,-1,0,0,0,0,
    0,0,0,-1,0,0,0,0,0
};

//  0 - - - - - - - - - - - 0
//  - + + + + + + + + + + + -
//  - + + + + + + + + + + + -
//  - + + + + + + + + + + + -
//  - - - - - - - - - - - - 0
//
static int r51a0_5x13[] =
{
    0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,
    -1,1,1,1,1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,1,1,1,1,-1,
    0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0
};

//  0 0 0 0 0 0 0 0 0 - - - 0
//  0 0 0 0 0 0 0 0 - + + + -
//  0 0 0 0 0 - - - + + + + -
//  0 0 0 - - + + + + + + + -
//  0 - - + + + + + + + - - 0
//  - + + + + + + + - - 0 0 0
//  - + + + + - - - 0 0 0 0 0
//  - + + + - 0 0 0 0 0 0 0 0
//  0 - - - 0 0 0 0 0 0 0 0 0
//
static int r51a22_9x13[] =
{
    0,0,0,0,0,0,0,0,0,-1,-1,-1,0,
    0,0,0,0,0,0,0,0,-1,1,1,1,-1,
    0,0,0,0,0,-1,-1,-1,1,1,1,1,-1,
    0,0,0,-1,-1,1,1,1,1,1,1,1,-1,
    0,-1,-1,1,1,1,1,1,1,1,-1,-1,0,
    -1,1,1,1,1,1,1,1,-1,-1,0,0,0,
    -1,1,1,1,1,-1,-1,-1,0,0,0,0,0,
    -1,1,1,1,-1,0,0,0,0,0,0,0,0,
    0,-1,-1,-1,0,0,0,0,0,0,0,0,0
};

//   0 0 0 0 0 0 0 0 - - 0
//   0 0 0 0 0 0 0 - + + -
//   0 0 0 0 0 0 - + + + -
//   0 0 0 0 0 - + + + - 0
//   0 0 0 0 - + + + - 0 0
//   0 0 0 - + + + - 0 0 0
//   0 0 - + + + - 0 0 0 0
//   0 - + + + - 0 0 0 0 0
//   - + + + - 0 0 0 0 0 0
//   - + + - 0 0 0 0 0 0 0
//   0 - - 0 0 0 0 0 0 0 0
//
static int r51a45_11x11[] =
{
    0,0,0,0,0,0,0,0,-1,-1,0,
    0,0,0,0,0,0,0,-1,1,1,-1,
    0,0,0,0,0,0,-1,1,1,1,-1,
    0,0,0,0,0,-1,1,1,1,-1,0,
    0,0,0,0,-1,1,1,1,-1,0,0,
    0,0,0,-1,1,1,1,-1,0,0,0,
    0,0,-1,1,1,1,-1,0,0,0,0,
    0,-1,1,1,1,-1,0,0,0,0,0,
    -1,1,1,1,-1,0,0,0,0,0,0,
    -1,1,1,-1,0,0,0,0,0,0,0,
    0,-1,-1,0,0,0,0,0,0,0,0
};

//  - - - - - - - - - - - - -
//  - + + + + + + + + + + + -
//  - + + + + + + + + + + + -
//  - + + + + + + + + + + + -
//  - + + + + + + + + + + + -
//  - + + + + + + + + + + + -
//  - - - - - - - - - - - - -
//
static int r52a0_7x13[] =
{
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,1,1,1,1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,1,1,1,1,-1,
    -1,1,1,1,1,1,1,1,1,1,1,1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

#if 0 // not used
//  0 0 0 0 0 0 0 0 - - - - 0
//  0 0 0 0 0 - - + + + + + -
//  0 0 0 0 - + + + + + + + -
//  0 0 0 - + + + + + + + - -
//  0 - - + + + + + + + - - 0
//  - - + + + + + + + - 0 0 0
//  - + + + + + + + - 0 0 0 0
//  - + + + + + - - 0 0 0 0 0
//  0 - - - - 0 0 0 0 0 0 0 0
//
static int r52a22_9x13[] =
{
    0,0,0,0,0,0,0,0,0,-1,-1,-1,0,
    0,0,0,0,0,-1,-1,1,1,1,1,1,-1,
    0,0,0,0,-1,1,1,1,1,1,1,1,-1,
    0,0,0,-1,1,1,1,1,1,1,1,-1,-1,
    0,-1,-1,1,1,1,1,1,1,1,-1,-1,0,
    -1,-1,1,1,1,1,1,1,1,-1,0,0,0,
    -1,1,1,1,1,1,1,1,-1,0,0,0,0,
    -1,1,1,1,1,1,-1,-1,0,0,0,0,0,
    0,-1,-1,-1,-1,0,0,0,0,0,0,0,0
};
#endif // 0

//  0 0 0 0 0 0 0 0 0 - 0 0 0
//  0 0 0 0 0 0 0 0 - + - 0 0
//  0 0 0 0 0 0 0 - + + + - 0
//  0 0 0 0 0 0 - + + + + + -
//  0 0 0 0 0 - + + + + + - 0
//  0 0 0 0 - + + + + + - 0 0
//  0 0 0 - + + + + + - 0 0 0
//  0 0 - + + + + + - 0 0 0 0
//  0 - + + + + + - 0 0 0 0 0
//  - + + + + + - 0 0 0 0 0 0
//  0 - + + + - 0 0 0 0 0 0 0
//  0 0 - + - 0 0 0 0 0 0 0 0
//  0 0 0 - 0 0 0 0 0 0 0 0 0
//
static int r52a45_13x13[] =
{
   0,0,0,0,0,0,0,0,0,-1,0,0,0,
   0,0,0,0,0,0,0,0,-1,1,-1,0,0,
   0,0,0,0,0,0,0,0,-1,1,1,1,-1,
   0,0,0,0,0,0,-1,1,1,1,1,1,-1,
   0,0,0,0,0,-1,1,1,1,1,1,-1,0,
   0,0,0,0,-1,1,1,1,1,1,-1,0,0,
   0,0,0,-1,1,1,1,1,1,-1,0,0,0,
   0,0,-1,1,1,1,1,1,-1,0,0,0,0,
   0,-1,1,1,1,1,1,-1,0,0,0,0,0,
   -1,1,1,1,1,1,-1,0,0,0,0,0,0,
   0,-1,1,1,1,-1,0,0,0,0,0,0,0,
   0,0,-1,1,-1,0,0,0,0,0,0,0,0,
   0,0,0,-1,0,0,0,0,0,0,0,0,0
};

//  0 0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 - - - - 0
//  0 0 0 0 0 - + + + + -
//  0 0 0 0 - + + + + + -
//  0 0 0 - + + + + + + -
//  0 0 - + + + + + + + -
//  0 0 - + + + + + + - 0
//  0 - + + + + + + - 0 0
//  0 - + + + + + + - 0 0
//  0 0 - + + + + - 0 0 0
//  0 0 0 - - - - 0 0 0 0
//
static int b0_11x11[] =
{
    0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,-1,-1,-1,-1,0,
    0,0,0,0,0,-1,1,1,1,1,-1,
    0,0,0,0,-1,1,1,1,1,1,-1,
    0,0,0,-1,1,1,1,1,1,1,-1,
    0,0,-1,1,1,1,1,1,1,1,-1,
    0,0,-1,1,1,1,1,1,1,-1,0,
    0,-1,1,1,1,1,1,1,-1,0,0,
    0,0,-1,1,1,1,1,-1,0,0,0,
    0,0,0,-1,-1,-1,-1,0,0,0,0
};

//    0 0 0 0 0 0 0 0 0
//    0 - - - - - 0 0 0
//    - + + + + + - 0 0
//    - + + + + + - 0 0
//    - + + + + + + - 0
//    0 - + + + + - 0 0
//    0 0 - + + + + - 0
//    0 0 0 - + + + - 0
//    0 0 0 0 - - - 0 0
//
static int b1_9x9[] =
{
    0,0,0,0,0,0,0,0,0,
    0,-1,-1,-1,-1,-1,0,0,0,
    -1,1,1,1,1,1,-1,0,0,
    -1,1,1,1,1,1,-1,0,0,
    -1,1,1,1,1,1,1,-1,0,
    0,-1,1,1,1,1,-1,0,0,
    0,0,-1,1,1,1,1,-1,0,
    0,0,0,-1,1,1,1,-1,0,
    0,0,0,0,-1,-1,-1,0,0
};

//  0 0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 - - - - - - 0
//  0 0 0 - + + + + + + -
//  0 0 0 - + + + + + + -
//  0 0 - + + + + + + - 0
//  0 0 - + + + + + + - 0
//  0 - + + + + + + - 0 0
//  0 - + + + + + + - 0 0
//  - + + + + + + - 0 0 0
//  - + + + + + + - 0 0 0
//  0 - - - - - - 0 0 0 0
//
static int b0s_11x11[] =
{
    0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,-1,-1,-1,-1,-1,-1,0,
    0,0,0,-1,1,1,1,1,1,1,-1,
    0,0,0,-1,1,1,1,1,1,1,-1,
    0,0,-1,1,1,1,1,1,1,-1,0,
    0,0,-1,1,1,1,1,1,1,-1,0,
    0,-1,1,1,1,1,1,1,-1,0,0,
    0,-1,1,1,1,1,1,1,-1,0,0,
    -1,1,1,1,1,1,1,-1,0,0,0,
    -1,1,1,1,1,1,1,-1,0,0,0,
    0,-1,-1,-1,-1,-1,-1,0,0,0,0
};

//    0 0 0 0 0 0 0 0 0
//    0 - - - - - 0 0 0
//    - + + + + + - 0 0
//    - + + + + + - 0 0
//    - + + + + + + - 0
//    0 - + + + + + + -
//    0 0 - + + + + + -
//    0 0 - + + + + + -
//    0 0 - - - - - - 0
//
static int b1s_9x9[] =
{
    0,0,0,0,0,0,0,0,0,
    0,-1,-1,-1,-1,-1,0,0,0,
    -1,1,1,1,1,1,-1,0,0,
    -1,1,1,1,1,1,-1,0,0,
    -1,1,1,1,1,1,1,-1,0,
    0,-1,1,1,1,1,1,1,-1,
    0,0,-1,1,1,1,1,1,-1,
    0,0,-1,1,1,1,1,1,-1,
    0,0,-1,-1,-1,-1,-1,0,0
};

// ====================   Corners ===========================

//    - - - - -
//    - + + + +
//    - + + + 0
//    - + + 0 0
//    - + 0 0 0
static int c4_90_0_5x5[] =
{
    -1,-1,-1,-1,-1,
    -1,1,1,1,1,
    -1,1,1,1,0,
    -1,1,1,0,0,
    -1,1,0,0,0
};

//   0 0 0 0 - - -
//   0 0 0 - - - -
//   0 0 - + + + +
//   0 - + + + + 0
//   - + + + + 0 0
//
static int c4_45_0_5x7[] =
{
    0,0,0,0,-1,-1,-1,
    0,0,0,-1,-1,-1,-1,
    0,0,-1,1,1,1,1,
    0,-1,1,1,1,1,0,
    -1,1,1,1,1,0,0
};

//    0 0 0 0 0 0 0
//    0 0 0 - 0 0 0
//    0 0 - + - 0 0
//    0 - + + + - 0
//    - + + + + + -
//
static int c4_45_45_5x7[] =
{
     0, 0, 0, 0, 0, 0, 0,
     0, 0, 0,-1, 0, 0, 0,
     0, 0,-1, 1,-1, 0, 0,
     0,-1, 1, 1, 1,-1, 0,
    -1, 1, 1, 1, 1, 1,-1
};
//====================== extrema ==========================
//   0 - 0
//   - + -
//   0 - 0
static int m1a0_3x3[] =
{
     0, -1,  0,
    -1,  1, -1,
     0, -1,  0
};

//   - + -
//   + + +
//   - + -
static int m4a0_3x3[] =
{
    -1, 1,-1,
     1, 1, 1,
    -1, 1,-1
};

//  0 0 - 0 0
//  0 - + - 0
//  - + + + -
//  0 - + - 0
//  0 0 - 0 0
static int m4sa0_5x5[] =
{
     0, 0,-1, 0, 0,
     0,-1, 1,-1, 0,
    -1, 1, 1, 1,-1,
     0,-1, 1,-1, 0,
     0, 0,-1, 0, 0};

// 0 0 0 - 0 0 0
// 0 0 - + - 0 0
// 0 - + + + - 0
// - + + + + + -
// 0 - + + + - 0
// 0 0 - + - 0 0
// 0 0 0 - 0 0 0
static int m7a0_7x7[] =
{
     0, 0, 0,-1, 0, 0, 0,
     0, 0,-1, 1,-1, 0, 0,
     0,-1, 1, 1, 1,-1, 0,
    -1, 1, 1, 1, 1, 1,-1,
     0,-1, 1, 1, 1,-1, 0,
     0, 0,-1, 1,-1, 0, 0,
     0, 0, 0,-1, 0, 0, 0
};

//=======================   edges =========================

//   0 - 0
//   0 + 0
//   0 0 0
static int e2a0_3x3[] =
{
    0,-1, 0,
    0, 1, 0,
    0, 0, 0
};

//    - 0 0
//    0 + 0
//    0 0 0
static int e2a45_3x3[] =
{
    -1, 0, 0,
     0, 1, 0,
     0, 0, 0
};

//        - - -
//        + + +
//        + + +

//
static int e3a0_3x3[] =
{
    -1,-1,-1,
     1, 1, 1,
     1, 1, 1
};

//  0 0 - + +
//  0 - + + 0
//  - + + 0 0
static int e3a45_3x5[] =
{
     0, 0,-1, 1, 1,
     0,-1, 1, 1, 0,
    -1, 1, 1, 0, 0
};

//       - - - - -
//       - - - - -
//       + + + + +
//       + + + + +
//       + + + + +
//
static int e5a0_5x5[] =
{
    -1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1
};

#if 0 // not used
//   0 0 - - -
//   - - - - -
//   - - + + +
//   + + + + +
//   + + + 0 0
//
static int e5a22_5x5[] =
{
    0,0,-1,-1,-1,
    -1,-1,-1,-1,-1,
    -1,-1,1,1,1,
    1,1,1,1,1,
    1,1,1,0,0
};
#endif

//  0 - - - +
//  - - - + +
//  - - + + +
//  - + + + 0
//  + + + 0 0
//
static int e5a45_5x5[] =
{
    0,-1,-1,-1,1,
    -1,-1,-1,1,1,
    -1,-1,1,1,1,
    -1,1,1,1,0,
    1,1,1,0,0
};

//  - - - - - - - - - - -
//  - - - - - - - - - - -
//  + + + + + + + + + + +
//  + + + + + + + + + + +
//  + + + + + + + + + + +
//
static int e11a0_5x11[] =
{
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1
};
//======================== region types ======================
//
//  all foreground
//   + + + + +
//   + + + + +
//   + + + + +
//   + + + + +
//   + + + + +
static int f5a0_5x5[] =
{
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1
};

//    0 + 0
//    + + +
//    0 + 0
static int f3a0_3x3[] =
{
   0,1,0,
   1,1,1,
   0,1,0
};

//  all background
//   - - - - -
//   - - - - -
//   - - - - -
//   - - - - -
//   - - - - -
static int b5a0_5x5[] =
{
    -1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1
};

//  all background
//  0 0 0 - 0 0 0
//  0 0 - - - 0 0
//  0 - - - - - 0
//  - - - - - - -
//  0 - - - - - 0
//  0 0 - - - 0 0
//  0 0 0 - 0 0 0
static int b7a0_7x7[] =
{
     0, 0, 0,-1, 0, 0, 0,
     0, 0,-1,-1,-1, 0, 0,
     0,-1,-1,-1,-1,-1, 0,
    -1,-1,-1,-1,-1,-1,-1,
     0,-1,-1,-1,-1,-1, 0,
     0, 0,-1,-1,-1, 0, 0,
     0, 0, 0,-1, 0, 0, 0,
};

// 0 0 0 - 0 0 0
// 0 0 - + - 0 0
// 0 - + + + - 0
// - + + + + + -
// 0 - + + + - 0
// 0 0 - + - 0 0
// 0 0 0 - 0 0 0
static int b7a1_7x7[] =
{
     0, 0, 0,-1, 0, 0, 0,
     0, 0,-1, 1,-1, 0, 0,
     0,-1, 1, 1, 1,-1, 0,
    -1, 1, 1, 1, 1, 1,-1,
     0,-1, 1, 1, 1,-1, 0,
     0, 0,-1, 1,-1, 0, 0,
     0, 0, 0,-1, 0, 0, 0,
};

//    0 - 0
//    - - -
//    0 - 0
static int b3a0_3x3[] =
{
    0,-1, 0,
   -1,-1,-1,
    0,-1, 0
};

vnl_matrix<int> reflect_about_vert(vnl_matrix<int> const& m)
{
  unsigned nc = m.cols(), nr = m.rows();
  vnl_matrix<int> ref(nr, nc);
  int rc = (nc-1)/2;
  for (unsigned r = 0; r<nr; ++r)
    for (int c = -rc; c<=rc; ++c)
      ref[r][rc+c] = m[r][rc-c];
  return ref;
}

#if 0
vnl_matrix<int> reflect_about_vert_even(vnl_matrix<int> const& m)
{
  unsigned nc = m.cols(), nr = m.rows();
  vnl_matrix<int> ref(nr, nc);
  int rc = nc/2;
  for (unsigned r = 0; r<nr; ++r)
    for (int c = -rc; c<=rc; ++c)
      ref[r][rc+c] = m[r][rc-c];
  return ref;
}
#endif

vnl_matrix<int> reflect_about_horz(vnl_matrix<int> const& m)
{
  unsigned nc = m.cols(), nr = m.rows();
  vnl_matrix<int> ref(nr, nc);
  int rr = (nr-1)/2;
  for (unsigned c = 0; c<nc; ++c)
    for (int r = -rr; r<=rr; ++r)
      ref[rr+r][c] = m[rr-r][c];
  return ref;
}

#if 0
vnl_matrix<int> reflect_about_horz_even(vnl_matrix<int> const& m)
{
  unsigned nc = m.cols(), nr = m.rows();
  vnl_matrix<int> ref(nr, nc);
  int rr = nr-1/2;
  for (unsigned c = 0; c<nc; ++c)
    for (int r = -rr; r<=rr; ++r)
      ref[rr+r][c] = m[rr-r][c];
  return ref;
}
#endif

brip_rect_mask::brip_rect_mask(mask_id mid)
{
  switch (mid)
    {
    case cr1:{
      vnl_matrix<int> cr1a0(cr1_5x5, 5, 5);
      masks_[a0] =    cr1a0;
      break;
    }
    case s1:{
      vnl_matrix<int> s1a0(s1_5x5, 5, 5);
      masks_[a0] =    s1a0;
      break;
    }
    case b0:{
      vnl_matrix<int> b0a0(b0_11x11, 11, 11);
      masks_[a0] =    b0a0;
      break;
    }
    case b1:{
      vnl_matrix<int> b1a0(b1_9x9, 9, 9);
      masks_[a0] =    b1a0;
      break;
    }
    case b0s:{
      vnl_matrix<int> b0sa0(b0s_11x11, 11, 11);
      masks_[a0] =    b0sa0;
      break;
    }
    case b1s:{
      vnl_matrix<int> b1sa0(b1s_9x9, 9, 9);
      masks_[a0] =    b1sa0;
      break;
    }
    case r10:{
      vnl_matrix<int> r10a0(r10a0_3x5, 3, 5), r10a45(r10a45_5x5, 5, 5);
      masks_[a0] =    r10a0;
      masks_[a45]  =  r10a45;
      masks_[a90]  =  r10a0.transpose();
      masks_[a135] =  reflect_about_vert(r10a45);
      break;
    }
    case r30: {
      vnl_matrix<int> r30a0(r30a0_3x9, 3, 9), r30a22(r30a22_5x9, 5, 9),
        r30a45(r30a45_7x7, 7, 7);
      masks_[a0] =    r30a0;
      masks_[a22] =   r30a22;
      masks_[a45] =   r30a45;
      masks_[a67]  =  r30a22.transpose();
      masks_[a90]  =  r30a0.transpose();
      masks_[a112] =  reflect_about_vert(r30a22.transpose());
      masks_[a135] =  reflect_about_vert(r30a45);
      masks_[a157] =  reflect_about_horz(r30a22);
      break;
    }
    case r31: {
      vnl_matrix<int> r31a0(r31a0_5x9, 5, 9), r31a22(r31a22_5x9, 5, 9),
        r31a45(r31a45_7x7, 7, 7);
      masks_[a0] =    r31a0;
      masks_[a22] =   r31a22;
      masks_[a45] =   r31a45;
      masks_[a67]  =  r31a22.transpose();
      masks_[a90]  =  r31a0.transpose();
      masks_[a112] =  reflect_about_vert(r31a22.transpose());
      masks_[a135] =  reflect_about_vert(r31a45);
      masks_[a157] =  reflect_about_horz(r31a22);
      break;
    }
    case r32: {
      vnl_matrix<int> r32a0(r32a0_7x9, 7, 9), r32a22(r32a22_7x9, 7, 9),
        r32a45(r32a45_9x9, 9, 9);
      masks_[a0] =    r32a0;
      masks_[a22] =   r32a22;
      masks_[a45] =   r32a45;
      masks_[a67]  =  r32a22.transpose();
      masks_[a90]  =  r32a0.transpose();
      masks_[a112] =  reflect_about_vert(r32a22.transpose());
      masks_[a135] =  reflect_about_vert(r32a45);
      masks_[a157] =  reflect_about_horz(r32a22);
      break;
    }
    case r51: {
      vnl_matrix<int> r51a0(r51a0_5x13, 5, 13),  r51a22(r51a22_9x13, 9, 13),
        r51a45(r51a45_11x11, 11, 11);
      masks_[a0] =    r51a0;
      masks_[a22] =   r51a22;
      masks_[a45] =   r51a45;
      masks_[a67]  =  r51a22.transpose();
      masks_[a90]  =  r51a0.transpose();
      masks_[a112] =  reflect_about_vert(r51a22.transpose());
      masks_[a135] =  reflect_about_vert(r51a45);
      masks_[a157] =  reflect_about_horz(r51a22);
      break;
    }
    case r52: {
      vnl_matrix<int> r52a0(r52a0_7x13, 7, 13),  r52a22(r51a22_9x13, 9, 13),
        r52a45(r52a45_13x13, 13, 13);
      masks_[a0] =    r52a0;
      masks_[a22] =   r52a22;
      masks_[a45] =   r52a45;
      masks_[a67]  =  r52a22.transpose();
      masks_[a90]  =  r52a0.transpose();
      masks_[a112] =  reflect_about_vert(r52a22.transpose());
      masks_[a135] =  reflect_about_vert(r52a45);
      masks_[a157] =  reflect_about_horz(r52a22);
      break;
    }
    case c4_90_0: {
      vnl_matrix<int> c4_90_0a0(c4_90_0_5x5, 5, 5);
      masks_[a0] =    c4_90_0a0;
      masks_[a90] =   reflect_about_vert(c4_90_0a0);
      masks_[a180] =   reflect_about_horz(c4_90_0a0);
      masks_[a270] =   reflect_about_horz(reflect_about_vert(c4_90_0a0));
      break;
    }
    case c4_45_0: {
      vnl_matrix<int> c4_45_0a0(c4_45_0_5x7, 5, 7);
      masks_[a0] =    c4_45_0a0;
      masks_[a45]=    c4_45_0a0.transpose();
      masks_[a90] =   reflect_about_vert(c4_45_0a0);
      masks_[a135] =  reflect_about_vert(c4_45_0a0.transpose());
      masks_[a180] =   reflect_about_horz(c4_45_0a0);
      masks_[a225]= reflect_about_horz(reflect_about_vert(c4_45_0a0.transpose()));
      masks_[a270] =  reflect_about_horz(reflect_about_vert(c4_45_0a0));
      break;
    }
    case c4_45_45: {
      vnl_matrix<int> c4_45_45a0(c4_45_45_5x7, 5, 7);
      masks_[a0] =    c4_45_45a0;
      masks_[a90] =   c4_45_45a0.transpose();
      masks_[a180] =  reflect_about_horz(c4_45_45a0);
      masks_[a270] =  reflect_about_horz(c4_45_45a0).transpose();
      break;
    }
    case e2:{
      vnl_matrix<int> e2a0(e2a0_3x3, 3, 3), e2a45(e2a45_3x3, 3, 3);
      masks_[a0] =    e2a0;
      masks_[a45] =   e2a45;
      masks_[a90]  =  e2a0.transpose();
      masks_[a135]  =  reflect_about_horz(e2a45);
      masks_[a180]  =  reflect_about_horz(e2a0);
      masks_[a225] =  reflect_about_vert(reflect_about_horz(e2a45));
      masks_[a270]  =  reflect_about_vert(e2a0.transpose());
      masks_[a315]  =  reflect_about_vert(e2a45.transpose());
      break;
    }
    case e3:{
      vnl_matrix<int> e3a0(e3a0_3x3, 3, 3), e3a45(e3a45_3x5, 3, 5);
      masks_[a0] =    e3a0;
      masks_[a45] =   e3a45;
      masks_[a90]  =  e3a0.transpose();
      masks_[a135]  =  reflect_about_horz(e3a45);
      masks_[a180]  =  reflect_about_horz(e3a0);
      masks_[a225] =  reflect_about_vert(reflect_about_horz(e3a45));
      masks_[a270]  =  reflect_about_vert(e3a0.transpose());
      masks_[a315]  =  reflect_about_vert(e3a45.transpose());
      break;
    }
    case e5:{
      vnl_matrix<int> e5a0(e5a0_5x5, 5, 5), e5a45(e5a45_5x5, 5, 5);
      masks_[a0] =    e5a0;
      masks_[a45] =   e5a45;
      masks_[a90]  =  e5a0.transpose();
      masks_[a135]  =  reflect_about_horz(e5a45);
      masks_[a180]  =  reflect_about_horz(e5a0);
      masks_[a225] =  reflect_about_vert(reflect_about_horz(e5a45));
      masks_[a270]  =  reflect_about_vert(e5a0.transpose());
      masks_[a315]  =  reflect_about_vert(e5a45.transpose());
      break;
    }
    case e11:{
      vnl_matrix<int> e11a0(e11a0_5x11, 5, 11);
      masks_[a0] =    e11a0;
      masks_[a90]  =  e11a0.transpose();
      masks_[a180]  =  reflect_about_horz(e11a0);
      masks_[a270]  =  reflect_about_vert(e11a0.transpose());
      break;
    }
    case m1:{
      vnl_matrix<int> m1a0(m1a0_3x3, 3, 3);
      masks_[a0] =    m1a0;
      break;
    }
    case m4:{
      vnl_matrix<int> m4a0(m4a0_3x3, 3, 3);
      masks_[a0] =    m4a0;
      break;
    }
    case m4s:{
      vnl_matrix<int> m4sa0(m4sa0_5x5, 5, 5);
      masks_[a0] =    m4sa0;
      break;
    }
    case m7:{
      vnl_matrix<int> m7a0(m7a0_7x7, 7, 7);
      masks_[a0] =    m7a0;
      break;
    }
    case f3:{
      vnl_matrix<int> f3a0(f3a0_3x3, 3, 3);
      masks_[a0] =    f3a0;
      break;
    }
    case b3:{
      vnl_matrix<int> b3a0(b3a0_3x3, 3, 3);
      masks_[a0] =    b3a0;
      break;
    }
    case f5:{
      vnl_matrix<int> f5a0(f5a0_5x5, 5, 5);
      masks_[a0] =    f5a0;
      break;
    }
    case b5:{
      vnl_matrix<int> b5a0(b5a0_5x5, 5, 5);
      masks_[a0] =    b5a0;
      break;
    }
    case b7:{
      vnl_matrix<int> b7a0(b7a0_7x7, 7, 7);
      vnl_matrix<int> b7a1(b7a1_7x7, 7, 7);
      masks_[a0] =    b7a1; // unused
      masks_[a0] =    b7a0;
      break;
    }

    default:
      std::cout << "specified mask does not exist\n";
    };
}

bool brip_rect_mask::
find_ait(ang_id aid,
         std::map<ang_id, vnl_matrix<int> >::const_iterator& ait) const
{
  ait = masks_.find(aid);
  if (ait == masks_.end())
    return false;
  return true;
}

brip_rect_mask::ang_id brip_rect_mask::angle_id(unsigned angle_index) const
{
  auto ait = masks_.begin();
  for (unsigned i = 0; i<angle_index; ++i)
    ++ait;
  return (*ait).first;
}

bool brip_rect_mask::set_angle_id(ang_id aid)
{
  std::map<ang_id, vnl_matrix<int> >::const_iterator ait;
  bool found = find_ait(aid, ait);
  if (!found) {
    ru_ = 0; rv_ = 0;
    return false;
  }
  current_mask_ = (*ait).second;
  ru_ = (current_mask_.cols()-1)/2;
  rv_ = (current_mask_.rows()-1)/2;
  return true;
}

void brip_rect_mask::init()
{
  if (!is_init_) {
    names_[a0]="0 deg";       angles_[a0]=0.0f;
    names_[a22]="22.5 deg";   angles_[a22]=22.5f;
    names_[a45]="45 deg";     angles_[a45]=45.0f;
    names_[a67]="67.5 deg";   angles_[a67]=67.5f;
    names_[a90]="90 deg";     angles_[a90]=90.0f;
    names_[a112]="112.5 deg"; angles_[a112]=112.5f;
    names_[a135]="135 deg";   angles_[a135]=135.0f;
    names_[a157]="157.5 deg"; angles_[a157]=157.5f;
    names_[a180]="180 deg";   angles_[a180]=180.0f;
    names_[a202]="202.5 deg"; angles_[a202]=202.5f;
    names_[a225]="225 deg";   angles_[a225]=225.0f;
    names_[a247]="247.5 deg"; angles_[a247]=247.5f;
    names_[a270]="270 deg";   angles_[a270]=270.0f;
    names_[a292]="292.5 deg"; angles_[a292]=292.5f;
    names_[a315]="315 deg";   angles_[a315]=315.0f;
    names_[a337]="337.5 deg"; angles_[a337]=337.5f;
    is_init_ = true;
  }
}

std::string brip_rect_mask::name(ang_id aid)
{
  init();
  std::map<ang_id, std::string >::const_iterator ait;
  ait = names_.find(aid);
  if (ait == names_.end())
    return "null";
  return (*ait).second;
 }

float brip_rect_mask::angle(ang_id aid)
{
  init();
  std::map<ang_id, float >::const_iterator ait;
  ait = angles_.find(aid);
  if (ait == angles_.end())
    return 0.0f;
  return (*ait).second;
 }

unsigned brip_rect_mask::nplus() const
{
  unsigned np = 0;
  vnl_matrix<int> const& weights = current_mask_;
  for (int weight : weights)
    if (weight>0) ++np;
  return np;
}

unsigned brip_rect_mask::nminus() const
{
  unsigned np = 0;
  vnl_matrix<int> const& weights = current_mask_;
  for (int weight : weights)
    if (weight<0) ++np;
  return np;
}

float brip_rect_mask::ang_diff(ang_id aid0,ang_id aid1)
{
  float diff = brip_rect_mask::angle(aid1)-brip_rect_mask::angle(aid0);
  if (diff>180.0f) diff-=360.0f;
  if (diff<-180.0f) diff+=360.0f;
  return diff;
}

bool brip_rect_mask::intersect(mask_id mid,
                               ang_id aid0,
                               unsigned short i0, unsigned short j0,
                               ang_id aid1,
                               unsigned short i1, unsigned short j1)
{
  brip_rect_mask m0(mid), m1(mid);
  m0.set_angle_id(aid0);
  m1.set_angle_id(aid1);
  unsigned ni0 = m0.ni(), nj0 = m0.nj(), ni1 = m1.ni(), nj1 = m1.nj();
  int  ru0 = (ni0-1)/2, rv0 = (nj0-1)/2;
  int ru1 = (ni1-1)/2, rv1 = (nj1-1)/2;
  for (int jr0 = -rv0; jr0<=rv0; ++jr0)
    for (int ir0 = -ru0; ir0<=ru0; ++ir0)
      if (m0(ir0,jr0)>0)
          for (int jr1 = -rv1; jr1<=rv1; ++jr1)
            for (int ir1 = -ru1; ir1<=ru1; ++ir1)
              if (m1(ir1,jr1)>0)
                if ((i0+ir0)==(i1+ir1)&&(j0+jr0)==(j1+jr1))
                  return true;
  return false;
}

bool brip_rect_mask::intersect_domain(mask_id mid,
                                      ang_id aid0,
                                      unsigned short i0, unsigned short j0,
                                      ang_id aid1,
                                      unsigned short i1, unsigned short j1)
{
  brip_rect_mask m0(mid), m1(mid);
  m0.set_angle_id(aid0);
  m1.set_angle_id(aid1);
  unsigned ni0 = m0.ni(), nj0 = m0.nj(), ni1 = m1.ni(), nj1 = m1.nj();
  int  ru0 = (ni0-1)/2, rv0 = (nj0-1)/2;
  int ru1 = (ni1-1)/2, rv1 = (nj1-1)/2;
  for (int jr0 = -rv0; jr0<=rv0; ++jr0)
    for (int ir0 = -ru0; ir0<=ru0; ++ir0)
      for (int jr1 = -rv1; jr1<=rv1; ++jr1)
        for (int ir1 = -ru1; ir1<=ru1; ++ir1)
          if ((i0+ir0)==(i1+ir1)&&(j0+jr0)==(j1+jr1))
            return true;
  return false;
}

void brip_rect_mask::print(ang_id aid)
{
  std::map<ang_id, vnl_matrix<int> >::const_iterator ait;
  bool found = find_ait(aid, ait);
  if (!found) {
    std::cout<< "no such angle\n";
    return;
  }
  vnl_matrix<int> msk = (*ait).second;
  int ri = (msk.cols()-1)/2, rj = (msk.rows()-1)/2;
  for (int jr = -rj; jr<=rj ; ++jr) {
    for (int ir = -ri; ir<= ri; ++ir)
    {
      int m = msk(jr+rj, ir+ri);
      if (m >0)
        std::cout << '+' << ' ';
      else if (m<0)
        std::cout << '-' << ' ';
      else
        std::cout << '0' << ' ';
    }
    std::cout << '\n';
  }
  std::cout<< '\n';
}

std::ostream& operator<<(std::ostream& s, brip_rect_mask const& msk)
{
  s << "masks\n";
  auto& msk_nc = const_cast<brip_rect_mask&>(msk);
  unsigned nangles = msk.n_angles();
  for (unsigned a = 0; a<nangles; ++a) {
    brip_rect_mask::ang_id aid = msk.angle_id(a);
    msk_nc.set_angle_id(aid);
    unsigned ni =msk_nc.ni(), nj = msk_nc.nj();
    int ri = (ni-1)/2, rj = (nj-1)/2;
    for (int jr = -rj; jr<=rj ; ++jr) {
      for (int ir = -ri; ir<= ri; ++ir)
      {
        int m = msk_nc(ir,jr);
        if (m >0)
          s << '+' << ' ';
        else if (m<0)
          s << '-' << ' ';
        else
          s << '0' << ' ';
      }
      s << '\n';
    }
    s<< '\n';
  }
  return s;
}
