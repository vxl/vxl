// This is brl/bbas/vidl2/vidl2_iidc1394_params.h
#ifndef vidl2_iidc1394_params_h_
#define vidl2_iidc1394_params_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A parameters struct for IIDC 1394 camera streams
//
// \author Matt Leotta 
// \date 10 Jan 2006
//
// These parameters are as described by the
// IIDC 1394-base Digital Camera Specification
// they should work for any istream that follows this
// specification.

#include <vcl_string.h>
#include <vcl_vector.h>
#include "vidl2_pixel_format.h"

//: A parameters struct for IIDC 1394 camera streams
//
// The setter functions are provided for convenience, so that the
// parameters can be set through an anonynous object.
// \code
//   ostream.open( filename,
//                 vidl2_iidc1394_params()
//                 .frame_rate( vidl2_iidc1394_params::FRAMERATE_7_5 )
//                 .video_mode( vidl2_iidc1394_params::MODE_1024x768_YUV422 ) );
// \endcode
struct vidl2_iidc1394_params
{
  //: Available data transfer speeds
  enum speed_t { ISO_SPEED_100= 0,
                 ISO_SPEED_200,
                 ISO_SPEED_400,
                 ISO_SPEED_800,
                 ISO_SPEED_1600,
                 ISO_SPEED_3200 };

  //: Return the speed value for a speed setting
  static unsigned int speed_val(speed_t s);

  //: Available frame rates
  enum frame_rate_t { FRAMERATE_1_875= 32,
                      FRAMERATE_3_75,
                      FRAMERATE_7_5,
                      FRAMERATE_15,
                      FRAMERATE_30,
                      FRAMERATE_60,
                      FRAMERATE_120,
                      FRAMERATE_240 };

  //: Return the frame rate as a floating point value
  static float frame_rate_val(frame_rate_t r);

  //: Available video modes (format and mode combined)
  enum video_mode_t { MODE_160x120_YUV444= 64,
                      MODE_320x240_YUV422,
                      MODE_640x480_YUV411,
                      MODE_640x480_YUV422,
                      MODE_640x480_RGB8,
                      MODE_640x480_MONO8,
                      MODE_640x480_MONO16,
                      MODE_800x600_YUV422= 96,
                      MODE_800x600_RGB8,
                      MODE_800x600_MONO8,
                      MODE_1024x768_YUV422,
                      MODE_1024x768_RGB8,
                      MODE_1024x768_MONO8,
                      MODE_800x600_MONO16,
                      MODE_1024x768_MONO16,
                      MODE_1280x960_YUV422= 128,
                      MODE_1280x960_RGB8,
                      MODE_1280x960_MONO8,
                      MODE_1600x1200_YUV422,
                      MODE_1600x1200_RGB8,
                      MODE_1600x1200_MONO8,
                      MODE_1280x960_MONO16,
                      MODE_1600x1200_MONO16,
                      MODE_EXIF= 256,
                      MODE_FORMAT7_0= 288,
                      MODE_FORMAT7_1,
                      MODE_FORMAT7_2,
                      MODE_FORMAT7_3,
                      MODE_FORMAT7_4,
                      MODE_FORMAT7_5,
                      MODE_FORMAT7_6,
                      MODE_FORMAT7_7 };

  //: Return string describing the mode
  static vcl_string video_mode_string(video_mode_t m);

  //: Return the format number from the video mode enumeration
  static unsigned int video_format_val(video_mode_t m);
  //: Return the mode number from the video mode enumeration
  static unsigned int video_mode_val(video_mode_t m);

  //: Return the pixel format of the video mode
  static vidl2_pixel_format pixel_format(video_mode_t m);

  //: Return the image resolution of the video mode
  static bool resolution(video_mode_t m, unsigned& ni, unsigned& nj);

  //: Return the video mode enumeration for a format and mode
  static video_mode_t video_mode(unsigned int format, unsigned int mode);

  //: Describes the valid options for the parameters
  struct valid_options;

  //-------------------------------------------------------

  //: The node number of the camera (default 0)
  int node_;

  //: The port number of the camera (default 0)
  unsigned int port_;

  //: The data transfer speed (default ISO_SPEED_400)
  speed_t speed_;

  //: The frame rate (default FRAMERATE_15)
  frame_rate_t frame_rate_;

  //: The video format and mode (default MODE_640x480_RGB8)
  video_mode_t video_mode_;

  //-------------------------------------------------------

  //: Construct to default values
  vidl2_iidc1394_params();

  //: Set the node
  vidl2_iidc1394_params& node( int n )
  { node_ = n; return *this; }

  //: Set the port
  vidl2_iidc1394_params& port( unsigned int p )
  { port_ = p; return *this; }

  //: Set the speed
  vidl2_iidc1394_params& speed( speed_t s )
  { speed_ = s; return *this; }

  //: Set the frame rate
  vidl2_iidc1394_params& frame_rate( frame_rate_t r )
  { frame_rate_ = r; return *this; }

  //: Set the video mode
  vidl2_iidc1394_params& video_mode(video_mode_t m)
  { video_mode_ = m; return *this; }
};

//=============================================================================

//: This structure holds the set of valid options for parameters
// The istream class should be able to produce this object by
// probing cameras on the 1394 bus
struct vidl2_iidc1394_params::valid_options
{
  //: A valid mode and its valid frame rates
  struct valid_mode
  {
    video_mode_t mode;
    vcl_vector<frame_rate_t> frame_rates;
  };

  //: A valid camera and its options
  struct camera
  {
    int node;
    unsigned int port;
    vcl_string vendor;
    vcl_string model;
    speed_t speed;
    vcl_vector<valid_mode> modes;
  };

  //: A vector of the valid cameras
  // one object for each camera detected on the bus
  vcl_vector<camera> cameras;
};

#endif // vidl2_iidc1394_params_h_
