#ifndef brct_plane_sweeper_h_
#define brct_plane_sweeper_h_
//:
// \file
// \brief Sweep a x-y plane and compute correllation and point matches.
//
//  The goal is to compute 3-d points from a stereo pair. It is assumed
//  that homographies for two planes (z=0 and z=z0) are available.  These
//  are currently computed by brct_plane_calibrator. By interpolation, we have
//  Hl(z) and Hr(z) for the left(l) and right(r) cameras for
//  any x-y plane, Z=z. Then at each z there are methods for computing matches
//  of intensity through normalized cross correlation and by the distance
//  between Harris corners.  A number of approaches are available, but the
//  most successful is to map Harris corners from the right image to the world
//  plane, using Hr^-1 then map the corners to the left image using Hl.
//  The overall homography is Hl*Hr^-1. A match between a mapped corner and
//  original left image corner is defined by 1) they are within a distance
//  threshold and 2) the normalized cross-correlation at the match point is
//  above a specified threshold.
//
//  Earlier attempts involved mapping the points and images onto the world
//  plane and carrying out the matches there. It is probably better conditioned
//  to consider errors in the projected image space rather than on the world plane.
//
// \author J.L. Mundy
// \verbatim
//  Initial version Feb. 2004
// \endverbatim
//
//////////////////////////////////////////////////////////////////////

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <bsol/bsol_point_index_2d.h>
#include <brct/brct_plane_sweeper_params.h>
class brct_plane_sweeper : public brct_plane_sweeper_params
{
 public:
  brct_plane_sweeper(brct_plane_sweeper_params const& sp);
  ~brct_plane_sweeper();

  //:Accessors
  vil1_memory_image_of<unsigned char> z_corr_image(const int i);

  vcl_vector<vsol_point_2d_sptr> harris_corners(const int cam);

  //:Mutators
  bool set_image(const int cam, vil1_image const& image);

  // Utility functions

  //: Read the homographies for two parallel planes to each camera plane
  bool read_homographies(vcl_string const& homography_file);

  //: compute harris corners for each image
  bool compute_harris();

  //: Project the camera image to the world plane
  vil1_memory_image_of<unsigned char> project_image_to_plane(const int plane,
                                                             const int cam);

  //: Map the camera image (cam) to the other image plane
  bool map_image_to_image(const int from_cam, const double z,
                          vil1_memory_image_of<unsigned char>& mapped_to_image,
                          vil1_memory_image_of<unsigned char>& orig_to_image);

  //: Construct images corresponding to the overlap in the two cam projections
  bool overlapping_projections(const int plane,
                               vcl_vector<vil1_memory_image_of<float> >& imgs);

  //: Construct images corresponding to the overlap in the two cam projections
  bool overlapping_projections(const double z,
                               vcl_vector<vil1_memory_image_of<float> >& imgs);


  //: Construct images and corners corresponding to the overlap
  bool overlapping_projections(const double z,
                               vcl_vector<vil1_memory_image_of<float> >& imgs,
                               vcl_vector<vcl_vector<vsol_point_2d_sptr> >& corners);

  //: Compute the cross correlation between overlapping projections for plane
  vil1_memory_image_of<unsigned char>
    cross_correlate_projections(const int plane);

  //: Compute the cross correlation between overlapping projections for z pos
  vil1_memory_image_of<unsigned char>
    cross_correlate_projections(const double z);

  //: Compute the cross correlation at corresponding Harris corners
  bool
    cross_correlate_proj_corners(const double z,
                                 vil1_image& back,
                                 vcl_vector<vsol_point_2d_sptr>& matched_cnrs,
                                 vcl_vector<vsol_point_2d_sptr>& back_prj_cnrs,
                                 vcl_vector<vsol_point_2d_sptr>& orig_cnrs0,
                                 bool reset_flags=false);

  //: Compute a depth image based on max cross-correlation.
  bool depth_image(vil1_memory_image_of<unsigned char>& depth_out,
                   vil1_memory_image_of<unsigned char>& coor_out);

  //: Compute Harris point matches by plane sweeping
  bool harris_depth_match(vcl_vector<vsol_point_3d_sptr>& points_3d,
                          vcl_vector<vsol_point_2d_sptr>& proj_points);

  //: Correlation values by depth
  void corr_vals(const int col, const int row,
                 vcl_vector<float>& z, vcl_vector<float>& corr);

  //: Correlation values by depth
  vsol_point_2d_sptr map_point(vsol_point_2d_sptr const& p, const int cam,
                               const double z);

  //: Map harris corners from one image to the other via plane at z
  bool
    map_harris_corners(const int from_cam, const double z,
                       vcl_vector<vsol_point_2d_sptr>& mapped_to_points,
                       vcl_vector<vsol_point_2d_sptr>& orig_to_points);

  //:init harris matcher
  void init_harris_match(const int from_cam);

  //: Map and match harris corners from one image to the other via plane at z
  bool
    match_harris_corners(const int from_cam, const double z,
                         vcl_vector<vsol_point_2d_sptr>& matched_points,
                         vcl_vector<vsol_point_2d_sptr>& orig_to_points);

  bool harris_sweep(const int from_cam);

  vcl_vector<vsol_point_2d_sptr> matched_points_at_z_index(int z_index);

  vcl_vector<vsol_point_3d_sptr> proj_points_3d();

  vcl_vector<vsol_point_3d_sptr> world_points_3d();

  bool save_world_points(vcl_string const& out_file);

 protected:
  //local utility functions

  bool overlapping_box(vcl_vector<vgl_h_matrix_2d<double> > const& homgs,
                       vsol_box_2d_sptr& box);
  //: tx and ty shift the projections to the (0,0) image origin
  bool
    overlapping_projections(vcl_vector<vgl_h_matrix_2d<double> > const& homgs,
                            vcl_vector<vil1_memory_image_of<float> >& imgs,
                            double& tx, double& ty);
  bool
    overlapping_projections(vcl_vector<vgl_h_matrix_2d<double> > const& homgs,
                            vcl_vector<vil1_memory_image_of<float> >& imgs,
                            vcl_vector<vcl_vector<vsol_point_2d_sptr> >& corners,
                            double& tx, double& ty);

  void homographies_at_z(double z,
                         vcl_vector<vgl_h_matrix_2d<double> >& homgs);
  vsol_box_2d_sptr
    depth_image_box(const double zmin, const double zmax);

  vcl_vector<vsol_point_2d_sptr> project_corners(vgl_h_matrix_2d<double> const & H,
                                                 vcl_vector<vsol_point_2d_sptr> const& corners);

  bool
    correlate_corners(vcl_vector<vil1_memory_image_of<float> > const& imgs,
                      vcl_vector<vcl_vector<vsol_point_2d_sptr> > const& cnrs,
                      vcl_vector<vsol_point_2d_sptr>& matched_corners);

  vil1_memory_image_of<vil1_rgb<unsigned char> >
    overlay_matches(vcl_vector<vsol_point_2d_sptr>,
                    vil1_memory_image_of<float> const& back);

  vsol_point_2d_sptr map_point(vsol_point_2d_sptr const& p,
                               vgl_h_matrix_2d<double> const& Hcomp);

  bool map_points(const int from_cam, const double z,
                  vcl_vector<vsol_point_2d_sptr> const& from_points,
                  vcl_vector<vsol_point_2d_sptr>& to_points);

  bool intersecting_bounding_box(vgl_h_matrix_2d<double> const& Hcomp,
                                 vsol_box_2d_sptr const& from_box,
                                 vsol_box_2d_sptr const& to_box,
                                 vsol_box_2d_sptr & box);

  bool map_image(const int from_cam, const double z,
                 vil1_memory_image_of<float>& mapped_image);

  //flags
  bool harris_valid_;
  bool homographies_valid_;

  //members
  int n_planes_;
  int n_cams_;
  float del_;
  int to_cam_;
  //world plane
  vcl_vector<double> z_;
  //world plane camera
  vcl_vector< vcl_vector<vgl_h_matrix_2d<double> > > homographies_;
  //cam
  vcl_vector<vil1_image> images_;
  vcl_vector<vil1_memory_image_of<float> > smooth_images_;
  //z
  vcl_vector<vil1_memory_image_of<float> > z_corr_images_;
  //cam       corners
  vcl_vector<vcl_vector<vsol_point_2d_sptr> > harris_corners_;
  //z                  matched corners
  vcl_vector<vcl_vector<vsol_point_2d_sptr> > matched_corners_;
  bsol_point_index_2d pindx_;
};

#endif // brct_plane_sweeper_h_
