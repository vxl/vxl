// This is brl/bseg/strk/strk_epipolar_grouper.h
#ifndef strk_epipolar_grouper_h_
#define strk_epipolar_grouper_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for grouping edgel chains for translational motion
//
//  The epipolar_grouper finds consistent translational motion groups
//  corresponding to intersections of a given epipolar line with
//  moving curves.
//
// \author
//  J.L. Mundy - December 23, 2003
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vil1/vil1_memory_image_of.h>
#include <vcl_vector.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <strk/strk_epipolar_grouper_params.h>
#include <strk/strk_epi_point_sptr.h>
#include <strk/strk_epi_seg_sptr.h>

class strk_epipolar_grouper : public strk_epipolar_grouper_params
{
 public:
  //Constructors/destructor
  strk_epipolar_grouper(strk_epipolar_grouper_params& tp);

  ~strk_epipolar_grouper();
  //Accessors
  void set_image(vil1_memory_image_of<float> const& image){image_=image;}
  void set_edges(int frame, vcl_vector<vtol_edge_2d_sptr> const & edges);
  vcl_vector<vsol_polyline_2d_sptr> display_segs(int frame);
  //Utility Methods
  void epi_coords(const double u, const double v,
                  double& alpha, double& s);
  //:initialize arrays
  void init(const int n_frames);
  //: the main process method
  bool group();
  //: debug methods
  void brute_force_match();
  vil1_memory_image_of<unsigned char> epi_region_image();
 protected:
  //protected methods
  float scan_interval(float xs, float ys, float xe, float ye, int& npix);
  bool scan_intensity_info(strk_epi_point_sptr const &pm1,
                           strk_epi_point_sptr p,
                           strk_epi_point_sptr const &pp1);
  bool inside_epipolar_wedge(vdgl_digital_curve_sptr const& dc);
  bool extract_alpha_segments(vdgl_digital_curve_sptr const & dc,
                              vcl_vector<strk_epi_seg_sptr>& epi_segs);
  bool compute_segments();
  bool intensity_candidates(const strk_epi_seg_sptr& seg,
                            vcl_vector<strk_epi_seg_sptr>& left_cand,
                            vcl_vector<strk_epi_seg_sptr>& right_cand);
  double radius(const double s);
  double find_left_s(const double a, const double s,
                     vcl_vector<strk_epi_seg_sptr> const& cand);
  double find_right_s(const double a, const double s,
                      vcl_vector<strk_epi_seg_sptr> const& cand);
  double ds(const double s);
  bool image_coords(const double a, const double s,
                    double& u, double& v);
  double scan_interval(const double a, const double sl, const double s);
  double scan_left(double a, double s,
                   vcl_vector<strk_epi_seg_sptr> const& left_cand,
                   double& ds);
  double scan_right(double a,double s,
                    vcl_vector<strk_epi_seg_sptr> const& right_cand,
                    double& ds);

  bool fill_intensity_values(strk_epi_seg_sptr& seg);
  bool set_intensity_info();
  //members
  double du_;
  double da_;
  double alpha_min_;
  double alpha_inv_;
  double smax_;
  int frame_; //the current frame index
  vgl_point_2d<double> epi_;//the epipole
  vnl_double_3 upper_wedge_line_;
  vnl_double_3 lower_wedge_line_;
  vil1_memory_image_of<float> image_; //the current frame
  vcl_vector<vtol_edge_2d_sptr> edges_;//edges for current frame
  // frames    segments
  vcl_vector<vcl_vector<strk_epi_seg_sptr> > min_epi_segs_;
  vcl_vector<vcl_vector<strk_epi_seg_sptr> > max_epi_segs_;
};

#endif // strk_epipolar_grouper_h_
