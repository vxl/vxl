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
#include <sdet/sdet_edgemap_sptr.h>

//: function to compute generic edges
class sdet_third_order_edge_det : public sdet_third_order_edge_det_params
{
 public:
  sdet_third_order_edge_det()
    : sdet_third_order_edge_det_params(), view_i(0), view_j(0) {}
  sdet_third_order_edge_det(sdet_third_order_edge_det_params const& params)
    : sdet_third_order_edge_det_params(params), view_i(0), view_j(0) {}
  void apply(vil_image_view<vxl_byte> const& image);
  bool apply_color(vil_image_view<vxl_byte> const& image);
  std::vector<vdgl_edgel>& edgels()  { return edgels_; }
  void line_segs(std::vector<vsol_line_2d_sptr>& lines);
  sdet_edgemap_sptr edgemap();

  //: save edgels in the edge map file FORMAT, output files have .edg extension
  static bool save_edg_ascii(const std::string& filename, unsigned ni, unsigned nj, const std::vector<vdgl_edgel>& edgels);

 private:
  std::vector<vdgl_edgel> edgels_;
  unsigned int view_i, view_j;
};

#endif // sdet_third_order_edge_det_h
