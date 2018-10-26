#ifndef bwm_video_registration_h
#define bwm_video_registration_h
//-----------------------------------------------------------------------------
//:
// \file
// \brief bwm_video_registration - Utilities for registering video streams
// \author  J.L. Mundy January 04, 2007
//
//
// \verbatim
// Modifications:
//   None
// \endverbatim
//-----------------------------------------------------------------------------
#include <vsol/vsol_box_2d_sptr.h>
#include <vgl/vgl_plane_3d.h>
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_ostream_sptr.h>
#include <bwm/video/bwm_video_cam_istream_sptr.h>

class bwm_video_registration
{
  // PUBLIC INTERFACE----------------------------------------------------------

  // Utility Methods

 public:
  //: Determine output image bounds containing all projections.
  // Given a set of perspective cameras and a world_plane
  // determine the bounds of the output image that will contain all
  // projections of the input stream.
  //
  // World sample distance is the average output pixel size in world units
  // over all homographies from the image plane onto the world plane.
  static bool
    output_frame_bounds_planar(bwm_video_cam_istream_sptr& cam_stream,
                               vgl_plane_3d<double> const& world_plane,
                               unsigned input_ni, unsigned input_nj,
                               vsol_box_2d_sptr& bounds,
                               double& world_sample_distance,
                               unsigned skip_frames = 1
                              );

  //: Map input stream onto world plane.
  // Given a set of input images, perspective cameras and world plane:
  // map the input video stream onto the specified world plane.
  // The resulting mapped images are written to the output stream.
  // World sample distance is output pixel size in world units.
  // This parameter determines the scale of the output image stream.
  static bool
  register_image_stream_planar(vidl_istream_sptr& in_stream,
                               bwm_video_cam_istream_sptr& cam_istream,
                               vgl_plane_3d<double> const& world_plane,
                               vsol_box_2d_sptr const& bounds,
                               double world_sample_distance,
                               vidl_ostream_sptr& out_stream,
                               unsigned skip_frames = 0,
                               bool preserve_float = true
                              );

  //: Map input stream onto world plane and write out the homographies
  static
bool register_planar_homographies(bwm_video_cam_istream_sptr& cam_istream,
                                  vgl_plane_3d<double> const& world_plane,
                                  vsol_box_2d_sptr const& bounds,
                                  double world_sample_distance,
                                  std::string const& homg_out_dir,
                                  unsigned skip_frames = 0
                                  );
 private:
  // only static methods
  bwm_video_registration() = delete;
};

#endif
