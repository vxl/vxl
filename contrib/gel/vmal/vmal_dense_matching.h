#ifndef vmal_dense_matching_h_
#define vmal_dense_matching_h_

//------------------------------------------------------------------------------
// .NAME        vmal_dense_matching
// .INCLUDE     vmal/vmal_dense_matching.h
// .FILE        vmal_dense_matching.cxx
// .SECTION Description:
//
// .SECTION Author
//   L. Guichard
// .SECTION Modifications:
//------------------------------------------------------------------------------


#include <vmal/vmal_multi_view_data_edge_sptr.h>
#include <vnl/vnl_double_3x3.h>

double vmal_round(double a);
int vmal_round_int(double a);

class vmal_dense_matching
{
 public:

  vmal_dense_matching(const vnl_double_3x3 & H0,
                      const vnl_double_3x3 & H1);

  ~vmal_dense_matching();

  void set_fmatrix(const vnl_double_3x3 & F);

  void set_hmatrix(const vnl_double_3x3 & H);


  // Between two set of lines in 2 images that are matched, it compute
  //the best lines using the fundamental constraint.
  void refine_lines_using_F(const vmal_multi_view_data_edge_sptr& mvd_edge,
                            const vmal_multi_view_data_edge_sptr& res);
  // Between two set of lines in 2 images that are matched, it compute
  //the best lines using the homography.
  void refine_lines_using_H(const vmal_multi_view_data_edge_sptr& mvd_edge,
                            const vmal_multi_view_data_edge_sptr& res);

  void disparity_map(const vmal_multi_view_data_edge_sptr& mvd_edge,
                     int h, int w);

 private:
  vnl_double_3x3 H0_,H1_,F_;
  vnl_double_3x3 H_;
  int type_; //1 for Fundamental matrix, 2 for Homography matrix
};

#endif
