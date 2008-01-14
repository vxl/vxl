#ifndef bwm_video_corr_processor_h_
#define bwm_video_corr_processor_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Various processes to compute cameras and refine correspondences
//
// \verbatim
//  Modifications
//   Original December 30, 2007
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_cost_function.h>
#include <vil/vil_image_view.h>
#include <bwm/video/bwm_video_corr.h>
#include <vidl2/vidl2_istream_sptr.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bwm/video/bwm_video_cam_istream_sptr.h>
#include <bwm/video/bwm_video_cam_ostream_sptr.h>

//: A least squares cost function for registering correspondences
// by minimizing square difference in intensities
class bwm_video_corr_lsqr_cost_func : public vnl_least_squares_function
{
public:
  //: Constructor
  // the base image is the between frame with missing correspondences
  // corr_image_a and corr_image_b are the images with known correspondences
  // corrs_a and corrs_b are the known correspondences in frames a and b
  // it is necessary that corrs_a and corrs_b are matching correspondences
  // proj_wld_pts are the projection of the 3-d correspondence world points
  // into the base image.
  
  bwm_video_corr_lsqr_cost_func(vil_image_view<float> const& base_image,
                                unsigned match_radius,
                                vcl_vector<float> corr_window_ab
                                );

  void set_base_image(vil_image_view<float> const& base){base_image_ = base;}

  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);
 protected:
  vil_image_view<float> base_image_;
  unsigned match_radius_;
  vcl_vector<float> corr_window_ab_;
};

class bwm_video_corr_cost_function: public vnl_cost_function
{
 public:
  //: Constructor - rcam pointer is not deleted by this class
  bwm_video_corr_cost_function(vil_image_view<float> const& base_image,
                                unsigned match_radius,
                                vcl_vector<float> corr_window_ab);
  ~bwm_video_corr_cost_function(){}

  //: The cost function. x is a 2-element vector holding the corr position
  virtual double f(vnl_vector<double> const& x);

  void set_base_image(vil_image_view<float> const& base){base_image_ = base;}
 protected:
  vil_image_view<float> base_image_;
  unsigned match_radius_;
  vcl_vector<float> corr_window_ab_;
};

class bwm_video_corr_processor 
{
  // PUBLIC INTERFACE----------------------------------------------------------
  // the type of solver
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  //: Constructor - default
  bwm_video_corr_processor() : verbose_(false), site_name_(""),
    video_path_(""), camera_path_(""), video_istr_(0), cam_istr_(0), 
    world_pts_valid_(false){}

  //: Destructor
  ~bwm_video_corr_processor() {}

  //: Accessors 
  void set_site_name(vcl_string const& site_name) {site_name_=site_name;}
  void set_video_path(vcl_string const& video_path) {video_path_=video_path;}
  void set_camera_path(vcl_string const& cam_path) {camera_path_=cam_path;}

  void set_verbose(bool verbose){verbose_ = verbose;}
  void set_correspondences(vcl_vector<bwm_video_corr_sptr> const& corrs);
  vcl_vector<bwm_video_corr_sptr> correspondences(){return corrs_;}

  vcl_string site_name() {return site_name_;}
  vcl_string video_path() {return video_path_;}
  vcl_string camera_path() {return camera_path_;}
  //: Data input Methods
  bool open_video_site(vcl_string const& site_path, bool cameras_exist = true);
  bool open_video_stream(vcl_string const& video_path);
  bool open_camera_istream(vcl_string const& camera_path);
  bool open_camera_ostream(vcl_string const& camera_path);

  //: Data output methods
  bool write_video_site(vcl_string const& site_path);

  // Process directives
  //: Find an initial set of world points and interpolated cameras
  // Assumptions: 1) a partial set of correspondences, e.g. every 5th frame
  //              2) no cameras available on any frame
  //              3) no world points are known
  //              4) initial_depth is a guess at camera standoff distance
  bool 
    initialize_world_pts_and_cameras(vpgl_calibration_matrix<double> const& K,
                                    double initial_depth);   
  //:get the resulting cameras
  vcl_vector<vpgl_perspective_camera<double> >cameras(){return cameras_;}

  //: save cameras to output stream
  bool write_cameras_to_stream();

  //: Find the missing correspondences by correlating with respect to 
  //  the bounding adjacent frames with correspondences
  // Assumptions: 1) Cameras are defined for each frame
  //              2) World points are available 
  // (if use_lmq == false then use amoeba)
  bool find_missing_correspondences(unsigned win_radius,
                                    unsigned search_radius, bool use_lmq = true);
  //: Recompute cameras and world points on the current set of correspondences 
  // Assumptions: 1) Cameras are defined for each frame
  //              2) Correspondences are defined for each frame
  bool refine_world_pts_and_cameras();

  void close(); //close all streams and clear data
  void print_frame_alignment_quality(unsigned start_frame, unsigned end_frame);
 protected:

  // INTERNALS-----------------------------------------------------------------
  //: compute the boolean mask defining which frames have coorespondences
  void mask(unsigned& min_frame, unsigned& max_frame, 
            vcl_vector<vcl_vector<bool> >& mask);

  //: get a float view of the frame at the specified index
  bool frame_at_index(unsigned frame_index, vil_image_view<float>& view);

  //: extract the float pixel windows for start and end frames used to
  //  find correspondences
  void compute_ab_corr_windows(unsigned match_radius,
                               vcl_vector<bool> const& mask_a,
                               vcl_vector<bool> const& mask_b);



  //: find correpondences on frame_index_x, between two frames,a and b
  // it must be true that index_a < index_x < index_b
  // if use_lmq = true levenberg_marquardt is used to refine the correspondence
  // position otherwise the amoeba algorithm is used
  bool find_missing_corrs(unsigned frame_index_a, vcl_vector<bool> mask_a,
                          unsigned frame_index_b, vcl_vector<bool> mask_b,
                          unsigned frame_index_x, unsigned win_radius,
                          unsigned search_radius, bool use_lmq);

  //: iterpolate between the set of known cameras (kcams). The vector,
  // unknown, specifies which cameras are unknown. The number of elements
  // in icams is the same as in unknown. The number of false entries in
  // unknown must be the same as the size of kcams.
  bool interpolate_cameras(vcl_vector<vpgl_perspective_camera<double> > kcams,
                           vcl_vector<bool> unknown, 
                           vcl_vector<vpgl_perspective_camera<double> >& icams);
//: search each pixel location in a square around the specified position
// the result is set back onto the position argument. RMS intensity difference
// at the start and end of the search is reported
void exhaustive_init(vnl_vector<double>& unknowns,
                     unsigned win_radius,
                     unsigned search_radius,
                     vil_image_view<float> const& base,
                     vcl_vector<float> corr_win,
                     double& start_error,
                     double& end_error);
  // Data Members--------------------------------------------------------------
 private:
  //: verbosity flag
  bool verbose_;

  //: video_site name
  vcl_string site_name_;

  //: video_path name
  vcl_string video_path_;

   //: video_path name
  vcl_string camera_path_;

 //: the list of corrs
  vcl_vector<bwm_video_corr_sptr> corrs_;
   //: the video input stream - currently only image list is supported
  vidl2_istream_sptr video_istr_;
  //: the camera input stream - currently only camera list is supported
  bwm_video_cam_istream_sptr cam_istr_;
  //: the camera output stream - currently only camera list is supported
  bwm_video_cam_ostream_sptr cam_ostr_;

  //: the current set of cameras
  vcl_vector<vpgl_perspective_camera<double> > cameras_;  
  //: the start index for correspondence processing
  unsigned frame_index_a_;
  //: the end index for correspondence processing
  unsigned frame_index_b_;
  //: the start frame for correspondence processing
  vil_image_view<float> image_a_;
  //: the end frame for correspondence processing
  vil_image_view<float> image_b_;
  //: world points have been computed
  bool world_pts_valid_;
  //: the world points for each correspondence
  vcl_vector<vgl_point_3d<double> > world_pts_;
  //: the float vectors for each correspondence match window on start frame
  vcl_vector<vcl_vector<float> >corr_windows_a_;
  //: the float vectors for each correspondence match window on end frame
  vcl_vector<vcl_vector<float> >corr_windows_b_;
};

#endif
