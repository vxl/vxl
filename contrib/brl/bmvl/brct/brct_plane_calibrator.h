#ifndef brct_plane_calibrator_h_
#define brct_plane_calibrator_h_
//:
// \file
// \brief A class to compute homographies from a world plane to a camera plane
//
// \author J.L. Mundy
// \verbatim
//  Initial version Feb. 2004
// \endverbatim
//
//////////////////////////////////////////////////////////////////////

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <brct/brct_corr_sptr.h>

class brct_plane_calibrator
{
 public:
  enum z_pos {Z_BACK=0, Z_FRONT};
  enum cam_no {LEFT=0, RIGHT};
  brct_plane_calibrator();
  ~brct_plane_calibrator();

  //Accessors

  int n_points(int plane){return pts_3d_[plane].size();}
  vcl_vector<vgl_homg_point_2d<double> >
    points_3d(int plane){return pts_3d_[plane];}
  vgl_homg_point_2d<double> p3d(const int plane, const int i)
    {return pts_3d_[plane][i];}
  brct_corr_sptr corr(const int plane, const int i){return corrs_[plane][i];}

  //Mutators

  //: read the visible marker points on a set of 3-d world planes
  void read_data(vcl_string const& point3d_file);
  //: set the image size for a given view
  bool set_image_size(const int cam, const int width, const int height);

  // Utility functions

  bool compute_initial_homographies();
  bool compute_homographies();
  bool write_corrs(vcl_string const& corrs_file);
  bool read_corrs(vcl_string const& corrs_file);
  bool write_homographies(vcl_string const& homography_file);
  vcl_vector<vgl_point_2d<double> >
    projected_3d_points_initial(const int plane, const int cam);
  vcl_vector<vgl_point_2d<double> >
    projected_3d_points(const int plane, const int cam);
 protected:
  //local utility functions

  void init_corrs();

  //members

  //:camera info
  vcl_vector<int> cam_width_;
  vcl_vector<int> cam_height_;

  //world plane      points
  vcl_vector<vcl_vector<vgl_homg_point_2d<double> > > pts_3d_;//points on 3-d planes

  //: correspondence data
  //world plane  image correspondences
  vcl_vector<vcl_vector<brct_corr_sptr> > corrs_;

  //: 3-d loaded flag
  bool points_3d_valid_;
  //: initial_homographies flag
  bool initial_homographies_valid_;

  //: debug flag
  bool debug_;
  double z_back_;
  double z_front_;
  //world plane       camera
  vcl_vector< vcl_vector<vgl_h_matrix_2d<double> > > initial_homographies_;
  vcl_vector< vcl_vector<vgl_h_matrix_2d<double> > > current_homographies_;
};

#endif // brct_plane_calibrator_h_
