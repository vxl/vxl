// This is core/vgui/vgui_viewer3D_tableau.h
#ifndef vgui_viewer3D_tableau_h_
#define vgui_viewer3D_tableau_h_
//:
// \file
// \brief  Tableau with functions to view 3D objects (eg, rotate, zoom, etc).
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   14-SEP-1999
//
//  Contains classes  vgui_viewer3D_tableau  vgui_viewer3D_tableau_new
//
// \verbatim
//  Modifications
//   14-SEP-1999 P.Pritchett - Initial version.
//   07-AUG-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
//   01-OCT-2002 K.Y.McGaul - Moved vgui_viewer3D to vgui_viewer3D_tableau.
// \endverbatim

#include "vgui_viewer3D_tableau_sptr.h"
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_drag_mixin.h>
#include <vgui/vgui_event_condition.h>
#include "dll.h"

struct vgui_viewer3D_tableau_spin;

//:  Tableau with functions to view 3D objects (eg, rotate, zoom, etc).
class vgui_viewer3D_tableau : public vgui_wrapper_tableau, public vgui_drag_mixin
{
 public:
  //: Constructor - don't use this, use vgui_viewer3D_tableau_new.
  vgui_viewer3D_tableau(vgui_tableau_sptr const&);

  // vgui_tableau methods
  void draw_before_child();
  virtual bool handle(const vgui_event& event);
  std::string type_name() const;

  // Drag mixin methods
  bool key_press(int, int, vgui_key, vgui_modifier);
  bool help();
  bool mouse_up(int x, int y, vgui_button button, vgui_modifier modifier);
  bool mouse_drag(int x, int y, vgui_button button, vgui_modifier modifier);
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

  // Key/mouse codes that initiate various actions
  vgui_event_condition c_mouse_rotate;
  vgui_event_condition c_mouse_translate;
  vgui_event_condition c_mouse_zoom;
  vgui_event_condition c_lock_dolly;
  vgui_event_condition c_lock_zoom;
  vgui_event_condition c_lighting;
  vgui_event_condition c_shading;
  vgui_event_condition c_spinning;
  vgui_event_condition c_render_mode;
  vgui_event_condition c_niceness;
  vgui_event_condition c_headlight;
  vgui_event_condition c_save_home;
  vgui_event_condition c_restore_home;

  // Implementation
  void setup_gl_matrices();

  //: Data on the current state of vgui_viewer3D_tableau (eg, the amount we are zoomed).
  struct token_t
  {
    float quat[4];     // quaternion
    float scale;
    float trans[3];
    float fov;
    token_t() {
      quat[0] = quat[1] = quat[2] = quat[3] = 0.0f;
      scale = 0.0;
      trans[0] = trans[1] = trans[2] = 0.0;
      fov = 45;
    }
  };

  token_t token;
  token_t home;

  static vgui_DLLDATA const void * const SPIN_EVENT;


  enum {wireframe, shaded, textured} gl_mode;

  bool spinning;
  bool allow_spinning;
  bool lighting;
  bool smooth_shading;
  bool high_quality;
  bool headlight;

 protected:
 ~vgui_viewer3D_tableau();

 private:
  vgui_event event;
  vgui_event last;

  vgui_viewer3D_tableau *viewer;
  float beginx;
  float beginy;
  token_t lastpos;
  float prevx;
  float prevy;

  bool lock_dolly;
  bool lock_zoom;

  vgui_viewer3D_tableau_spin *spin_data;
};

//: Create a smart pointer to a vgui_viewer3D_tableau tableau.
struct vgui_viewer3D_tableau_new : public vgui_viewer3D_tableau_sptr
{
  typedef vgui_viewer3D_tableau_sptr base;
  vgui_viewer3D_tableau_new(vgui_tableau_sptr const& a) : base(new vgui_viewer3D_tableau(a)) { }
};

#endif // vgui_viewer3D_tableau_h_
