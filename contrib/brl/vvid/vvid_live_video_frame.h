// This is brl/vvid/vvid_live_video_frame.h
#ifndef vvid_live_video_frame_h_
#define vvid_live_video_frame_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vvid_live_video_frame
//
//  A special tableau that has a live camera as an image generator. The
//  basic image tableau is wrapped in a easy2D_tableau so that overlays
//  can be generated if desired
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy September 14, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vvid/vvid_live_video_frame_sptr.h>
#include <vvid/cmu_1394_camera.h>

class vvid_live_video_frame : public vbl_ref_count
{
 public:
  vvid_live_video_frame(int node, int pixel_sample_interval,
                        const cmu_1394_camera_params& cp);
  ~vvid_live_video_frame();
  //camera manipulation
  void set_camera_params(const cmu_1394_camera_params& cp);
  cmu_1394_camera_params get_camera_params() const { return (const cmu_1394_camera_params)cam_; }
  bool attach_live_video();
  void start_live_video();
  void update_frame();
  void stop_live_video();

  //: Frame state, i.e. is live and capturing frames
  bool get_frame_live() const { return live_; }
  //: Access to next live camera frames. Causes the camera to take a new frame
  void get_camera_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im,
                            int pix_sample_interval = 1);
  void get_camera_mono_image(vil_memory_image_of<unsigned char>& im,
                             int pix_sample_interval = 1);

  //: Access to current cached camera frames
  // Both styles of image access are supported

  //: New pointer to the image
  vil_memory_image_of< vil_rgb<unsigned char> > get_current_rgb_image(int pix_sample_interval);

  //: User supplies the pointer to the image
  bool get_current_rgb_image(int pix_sample_interval,
                             vil_memory_image_of< vil_rgb<unsigned char> >& im);

  vil_memory_image_of<unsigned char> get_current_mono_image(int pix_sample_interval);

  bool get_current_mono_image(int pix_sample_interval,
                              vil_memory_image_of<unsigned char>& im);

  //: Handy pointers to the frame tableaux
  vgui_image_tableau_sptr get_image_tableau(){return itab_;}
  vgui_easy2D_tableau_sptr get_easy2D_tableau(){return e2d_;}
  vgui_viewer2D_tableau_sptr get_viewer2D_tableau(){return v2d_;}

 private:
  //status flags
  bool live_;//video is live
  //the live camera
  cmu_1394_camera cam_;
  int pixel_sample_interval_;//default pixel sample spacing
  //:the tableau hierarchy
  // Local pointers to the camera image in the image_tableau
  vil_memory_image_of< vil_rgb<unsigned char> > rgb_frame_;
  vil_memory_image_of< unsigned char > mono_frame_;
  vgui_image_tableau_sptr itab_;
  vgui_easy2D_tableau_sptr e2d_;
  vgui_viewer2D_tableau_sptr v2d_;
};

#endif // vvid_live_video_frame_h_
