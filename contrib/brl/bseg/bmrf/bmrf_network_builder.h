// This is brl/bseg/bmrf/bmrf_network_builder.h
#ifndef bmrf_network_builder_h_
#define bmrf_network_builder_h_
//:
// \file
// \brief A builder class that generates the MRF network
// \author J. L. Mundy
// \date 1/18/04
//
// The builder generates bmrf_epi_seg(s) from a series of frames and adds
// them to the network.
//
// \verbatim
//  Modifications
// \endverbatim
#include <vil/vil_image_view.h>
//#include <vil1/vil1_memory_image_of.h>
#include <vcl_vector.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_point_2d.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <bmrf/bmrf_network_builder_params.h>
#include <bmrf/bmrf_epi_point_sptr.h>
#include <bmrf/bmrf_epi_seg_sptr.h>
#include <bmrf/bmrf_node_sptr.h>
#include <bmrf/bmrf_network_sptr.h>

class bmrf_network_builder : public bmrf_network_builder_params
{
 public:
  //Constructors/destructor
  bmrf_network_builder(bmrf_network_builder_params& tp);

  ~bmrf_network_builder();

  //Accessors

  //: get the completed network
  bmrf_network_sptr network();

  //Mutators

  //: specify the image for the current frame
  void set_image(vil_image_view<float> const& image);

  //: set the edge segmentation for the current frame
  void set_edges(int frame, vcl_vector<vtol_edge_2d_sptr> const & edges);

  //Utility Methods

  //:convert image coordinates to epipolar coordinates
  void epi_coords(const double u, const double v,
                  double& alpha, double& s);

  //:convert epipolar coordinates to image coordinates
  // \note if \param u or \param v are outside the image return false.
  bool image_coords(const double a, const double s,
                    double& u, double& v);

  //:initialize arrays
  void init(const int n_frames);

  //: the main process method
  bool build();


 protected:
  float scan_interval(float xs, float ys, float xe, float ye, int& npix);
  bool scan_intensity_info(bmrf_epi_point_sptr const &pm1,
                           bmrf_epi_point_sptr p,
                           bmrf_epi_point_sptr const &pp1);
  bool inside_epipolar_wedge(vdgl_digital_curve_sptr const& dc);
  bool extract_alpha_segments(vdgl_digital_curve_sptr const & dc,
                              vcl_vector<bmrf_epi_seg_sptr>& epi_segs);
  bool compute_segments();
  bool intensity_candidates(const bmrf_epi_seg_sptr& seg,
                            vcl_vector<bmrf_epi_seg_sptr>& left_cand,
                            vcl_vector<bmrf_epi_seg_sptr>& right_cand);
  double radius(const double s);
  double find_left_s(const double a, const double s,
                     vcl_vector<bmrf_epi_seg_sptr> const& cand);
  double find_right_s(const double a, const double s,
                      vcl_vector<bmrf_epi_seg_sptr> const& cand);
  double ds(const double s);

  double scan_interval(const double a, const double sl, const double s);
  double scan_left(double a, double s,
                   vcl_vector<bmrf_epi_seg_sptr> const& left_cand,
                   double& ds);
  double scan_right(double a,double s,
                    vcl_vector<bmrf_epi_seg_sptr> const& right_cand,
                    double& ds);

  bool fill_intensity_values(bmrf_epi_seg_sptr& seg);
  bool set_intensity_info();
  bool add_frame_nodes();
  bool time_neighbors(bmrf_node_sptr const& node,
                      vcl_vector<bmrf_node_sptr>& neighbors);
  bool assign_neighbors();
  bool build_network();
  //members
  //: network building status flags
  bool network_valid_;

  //:the current frame index
  int frame_;

  //:the increment in image column dimension
  double du_;

  //:the increment in epipolar angle
  double da_;

  //:the minimum alpha bound
  double alpha_min_;

  //:the alpha scale factor to map alpha onto [0, 1]
  double alpha_inv_;

  //:the maximum distance from the epipole in the image
  double smax_;

  //:the epipole
  vgl_point_2d<double> epi_;

  //:the homogeneous coordinates of the upper epipolar wedge limit
  vnl_double_3 upper_wedge_line_;

  //:the homogeneous coordinates of the lower epipolar wedge limit
  vnl_double_3 lower_wedge_line_;

  //:the image for the current frame
  vil_image_view<float> image_;

  //:the edges for current frame
  vcl_vector<vtol_edge_2d_sptr> edges_;

  //:the network under construction
  bmrf_network_sptr network_;

  //:temporary arrays for building the intensity information
  vcl_vector<bmrf_epi_seg_sptr> min_epi_segs_;
  vcl_vector<bmrf_epi_seg_sptr> max_epi_segs_;
};

#endif // bmrf_network_builder_h_
