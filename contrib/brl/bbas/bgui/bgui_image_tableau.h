// This is brl/bbas/bgui/bgui_image_tableau.h
#ifndef bgui_image_tableau_h_
#define bgui_image_tableau_h_
//:
// \file
// \author  Marko Bacic (u97mb@robots.ox.ac.uk)
// \brief An image tableau with an associated region of interest.
//
// \verbatim
//  Modifications
//   Marko Bacic    AUG-2000  Initial version.
//   K.Y.McGaul  05-AUG-2002  Print mouse position on status bar.
//   K.Y.McGaul  06-AUG-2002  Print RGB value on status bar.
//   Joseph Mundy   DEC-2002  Copied to brl to serve as a basis for extended functions
//   Peter Vanroose AUG-2004  Moved bgui_roi_tableau_make_roi to separate file
//   Gamze Tunali   SEP-2007  Added class bgui_image_tableau_vp_sc_snapshot
// \endverbatim

#include "bgui_image_tableau_sptr.h"
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view_base.h>
#include <vgui/vgui_text_tableau.h>
#include <vil1/vil1_image.h>
#include <vgui/vgui_gl.h>

class bgui_image_tableau_vp_sc_snapshot
{
 public:
  GLint vp[4];
  GLint sc[4];
  bool sc_was_enabled;

  bgui_image_tableau_vp_sc_snapshot() {
    glGetIntegerv(GL_VIEWPORT, vp);

    //std::cout << "Saving    [" << vp[0] << ' ' << vp[1] << ' ' << vp[2] << ' ' << vp[3] << std::endl;
    glGetIntegerv(GL_SCISSOR_BOX, sc);
    sc_was_enabled = glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE;
  }

  ~bgui_image_tableau_vp_sc_snapshot() {
    // restore viewport :
    glViewport(vp[0], vp[1], vp[2], vp[3]);
    //std::cout << "restoring [" << vp[0] << ' ' << vp[1] << ' ' << vp[2] << ' ' << vp[3] << std::endl;
    // turn off the scissor test, if it wasn't already on, and
    // restore old scissor settings :
    if (sc_was_enabled)
      glEnable(GL_SCISSOR_TEST);
    else
      glDisable(GL_SCISSOR_TEST);
    glScissor(sc[0], sc[1], sc[2], sc[3]);
  }
};

class bgui_image_tableau : public vgui_image_tableau
{
 public:
  typedef vgui_image_tableau base;

  bgui_image_tableau();

  bgui_image_tableau(vil_image_resource_sptr const & img,
                     vgui_range_map_params_sptr const & rmp = nullptr);

  bgui_image_tableau( vil_image_view_base const & img,
                      vgui_range_map_params_sptr const & rmp = nullptr);

  bgui_image_tableau(vil1_image const & img,
                     vgui_range_map_params_sptr const & rmp = nullptr);

  bgui_image_tableau(char const* filename,
                     vgui_range_map_params_sptr const & rmp = nullptr);


  std::string type_name() const { return "vgui_image_tableau"; }

  //: Return the vil1_image
  vil1_image get_image() const;

  //: Return the vil_image_resource
  vil_image_resource_sptr get_image_resource() const;

  //: Extract a line of pixel values (force to grey scale)
  void image_line(const float col_start,
                  const float row_start,
                  const float col_end,
                  const float row_end,
                  std::vector<double>& line_pos,
                  std::vector<double>& vals);

  //: Extract a line of pixel values return color if available
  void image_line(const float col_start,
                  const float row_start,
                  const float col_end,
                  const float row_end,
                  std::vector<double>& line_pos,
                  std::vector<std::vector<double> >& vals);

  //: set handle_motion true
  void set_handle_motion() { handle_motion_ = true; }
  void unset_handle_motion() { handle_motion_ = false; }
  bool handle_motion() const { return handle_motion_; }

  //: display a message at the mouse pointer
  void set_capture_mouse(){capture_mouse_ = true;}
  void unset_capture_mouse(){capture_mouse_ = false;}
  void set_mouse_message() {mouse_message_ = true; capture_mouse_ = true;}
  void unset_mouse_message();
  bool mouse_message() const {return mouse_message_;}
  void set_mouse_message_text(std::string const& msg){mouse_message_text_ = msg;}
  std::string mouse_message_text() const{ return mouse_message_text_;}
  bool mouse_pos(float& mouse_u, float& mouse_v){mouse_u = mouse_pos_[0],
      mouse_v = mouse_pos_[1]; return mouse_message_;}

  // true means locked, false unlocked
  void lock_linenum(bool b) { locked_ = b; handle_motion_=!b; }
  void show_image_path(bool show){show_path_ = show;}
  vgui_text_tableau_sptr text_tab(){return tt_;}
 protected:
  //: Handle all events for this tableau.
  bool handle(vgui_event const &e);

  //: get the pixel value as a double. RGB converted to grey.
  double get_pixel_value(const unsigned c, const unsigned r);

//: get the pixel value as color
  std::vector<double> get_color_pixel_value(const unsigned c, const unsigned r);

  //: Get pixel info from the frame buffer)
  void get_pixel_info_from_frame_buffer(const int x, const int y,
                                        vgui_event const &e, char* msg);

  //: Get pixel info from the image (rather than frame buffer)
  void get_pixel_info_from_image(const int x, const int y,
                                 vgui_event const &e, char* msg);

  //: If false this tableau stops handling motion
  bool handle_motion_;

  bool locked_;
  bool show_path_;
  bool mouse_message_;
  bool capture_mouse_;
  std::string mouse_message_text_;
  float mouse_pos_[2];
  vgui_text_tableau_sptr tt_;
};

//: Creates a smart-pointer to a bgui_image_tableau.
struct bgui_image_tableau_new : public bgui_image_tableau_sptr {

  //: Constructor - creates an empty image tableau.
  bgui_image_tableau_new()
    : bgui_image_tableau_sptr( new bgui_image_tableau() )
  { }

  //:  Constructors - create a tableau displaying the given image.

  bgui_image_tableau_new(vil_image_resource_sptr const & img,
                         vgui_range_map_params_sptr const & rmp = nullptr)
    : bgui_image_tableau_sptr( new bgui_image_tableau(img, rmp) )
  { }

  bgui_image_tableau_new(vil_image_view_base const & img,
                         vgui_range_map_params_sptr const & rmp = nullptr)
    : bgui_image_tableau_sptr( new bgui_image_tableau(img, rmp) )
  { }

  bgui_image_tableau_new(vil1_image const & img,
                         vgui_range_map_params_sptr const & rmp = nullptr)
    : bgui_image_tableau_sptr( new bgui_image_tableau(img, rmp) )
  { }

  bgui_image_tableau_new(char const* filename,
                         vgui_range_map_params_sptr const & rmp = nullptr)
    : bgui_image_tableau_sptr( new bgui_image_tableau(filename, rmp) )
  { }

  //:to cast up to a vgui_image_tableau
  operator vgui_image_tableau_sptr () const
  { vgui_image_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // bgui_image_tableau_h_
