// This is brl/bseg/sdet/sdet_third_order_edge_det.h
#ifndef sdet_third_order_edge_det_h
#define sdet_third_order_edge_det_h
//:
//\file
//\brief An edge detector that first computes edge locations and then optimizes the orientations using the third order operator
//\author Amir Tamrakar
//\date 09 Sept 2006
//
//\verbatim
//  Modifications
//\endverbatim

#include <vil/vil_image_view.h>
#include <vdgl/vdgl_edgel.h>
#include <brip/brip_gaussian_kernel.h>
#include <brip/brip_interp_kernel.h>
#include <sdet/sdet_nms.h>
#include <brip/brip_subpix_convolution.h>
#include <sdet/sdet_third_order_edge_det_params.h>
#include <vsol/vsol_line_2d_sptr.h>

//: function to compute generic edges
class sdet_third_order_edge_det : public sdet_third_order_edge_det_params
{
 public:
  sdet_third_order_edge_det()
    : sdet_third_order_edge_det_params() {}
  sdet_third_order_edge_det(sdet_third_order_edge_det_params const& params)
    : sdet_third_order_edge_det_params(params) {}
  void apply(vil_image_view<vxl_byte> const& image);
  bool apply_color(vil_image_view<vxl_byte> const& image);
  vcl_vector<vdgl_edgel>& edgels() { return edgels_; }
  void line_segs(vcl_vector<vsol_line_2d_sptr>& lines);

  //: save edgels in the edge map file FORMAT, output files have .edg extension
  static bool save_edg_ascii(const vcl_string& filename, unsigned ni, unsigned nj, const vcl_vector<vdgl_edgel>& edgels);

 private:
  vcl_vector<vdgl_edgel> edgels_;
};

#endif // sdet_third_order_edge_det_h
