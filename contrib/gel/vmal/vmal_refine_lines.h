#ifndef vmal_refine_lines_h_
#define vmal_refine_lines_h_
//--------------------------------------------------------------------------------
//:
//  \file
//
//   Refine two lines using different methods:
//   - with this homography between the two images
//       * refine at the minimum: keep only the common part of the lines (intersection)
//       * refine at the maximum: extend the lines (union)
//   - with the fundamental matrix
//       Problems have been encountered because when the lines are almost parallel
//       to the epipolar lines, big errors occurred.
//
// \author
//   L. Guichard
//--------------------------------------------------------------------------------
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>

class vmal_refine_lines
{
 public:
  //---------------------------------------------------------------------------
  //: Constructors.
  //---------------------------------------------------------------------------
  vmal_refine_lines();

  //---------------------------------------------------------------------------
  //: Destructor.
  //---------------------------------------------------------------------------
  ~vmal_refine_lines();

  //---------------------------------------------------------------------------
  //: Refine the two lines using the fundamental matrix (To improve!).
  //---------------------------------------------------------------------------
  void refine_lines_f(vnl_double_3 &line0p, vnl_double_3 &line0q,
                      vnl_double_3 &line1p, vnl_double_3 &line1q,
                      const vnl_double_3x3 & F,
                      vnl_double_3 &r_line0p, vnl_double_3 &r_line0q,
                      vnl_double_3 &r_line1p, vnl_double_3 &r_line1q);

  //---------------------------------------------------------------------------
  //: Refine two lines using the homography matrix and keeping the intersection
  //---------------------------------------------------------------------------
  void refine_lines_max_h(vnl_double_3 &line0p, vnl_double_3 &line0q,
                          vnl_double_3 &line1p, vnl_double_3 &line1q,
                          const vnl_double_3x3 & H,
                          vnl_double_3 &r_line0p, vnl_double_3 &r_line0q,
                          vnl_double_3 &r_line1p, vnl_double_3 &r_line1q);

  //---------------------------------------------------------------------------
  //: Refine two lines using the homography matrix and extending to the union
  //---------------------------------------------------------------------------
  void refine_lines_min_h(vnl_double_3 &line0p, vnl_double_3 &line0q,
                          vnl_double_3 &line1p, vnl_double_3 &line1q,
                          const vnl_double_3x3 &H,
                          vnl_double_3 &r_line0p, vnl_double_3 &r_line0q,
                          vnl_double_3 &r_line1p, vnl_double_3 &r_line1q);
};

#endif
