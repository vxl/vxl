// This is core/vgui/vgui_soview.h
#ifndef vgui_soview_h_
#define vgui_soview_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author  Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date    24 Mar 99
// \brief   Spatial object view (base class for 2 & 3D geometric objects).
//
//  Contains classes: vgui_soview
//  Notes: We use floats instead of doubles as size is a speed issue (sic.)
//

#include "dll.h"

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>

#include "vgui_style.h"
#include "vgui_style_sptr.h"

class vgui_observer;
class vgui_message;

//: Spatial object view (base class for 2 & 3D geometric objects).
//
//  This class is the base class for vgui_soview2D and vgui_soview3D
//  and contains the functionality they have in common.
class vgui_soview /*: public vgui_observable*/
{
 public:
  //: Constructor - create a default soview.
  vgui_soview();

  //: Destructor - delete this soview.
  virtual ~vgui_soview();

  //: Render this soview on the display.
  virtual void draw() const = 0;

  //: Render this soview for selection purposes.
  //
  // By default, this will call draw(). However, some objects take
  // time to draw, especially in GL_SELECT mode. The routine allows
  // such objects to render a simplified version for the selection
  // process. Note that during selection, the object is not rendered
  // on screen. The "rendering" is used by OpenGL internals to
  // determine if the object is in the selection region (e.g. area
  // around mouse pointer).
  //
  virtual void draw_select() const;

  //: Calls OpenGL function glLoadName with this soview's id.
  virtual void load_name() const;

  //: Prints the ID of this soview.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: This should never be called, derived classes should implement this.
  virtual vcl_string type_name() const;

  //: Set the style (colour, line width) of the soview.
  virtual void set_style(const vgui_style_sptr&);

  //: Return the style (colour, line width) of the soview.
  virtual vgui_style_sptr get_style() const;

  //: Set the colour of the soview.
  void set_colour(float r, float g, float b);

  //: Set the point radius of the soview.
  void set_point_size(float s);

  //: Set the line width of the soview.
  void set_line_width(float w);

  //: Attach given observer to this soview.
  void attach(vgui_observer*);

  //: Detach the given observer from this soview.
  void detach(vgui_observer*);

  //: Get a list of all observers attached to this soview.
  void get_observers(vcl_vector<vgui_observer*>&) const;

  //: Update all observers.
  void notify() const;

  //: Send message to all observers.
  void notify(vgui_message const &) const;

  // fsm. new old message model
  static vgui_DLLDATA const void * const msg_select;
  static vgui_DLLDATA const void * const msg_deselect;

  //: Returns the ID of this soview.
  virtual unsigned get_id() const {return id;}

  //: Returns a pointer to the vgui_soview, given the ID.
  static vgui_soview* id_to_object(unsigned id);

  //: Create a new ID.
  static unsigned create_id();

  //: Return true if it is possible to select this soview.
  bool get_selectable() const { return selectable; }

  //: Make this soview selectable/non-selectable.
  void set_selectable( bool s) { selectable= s; }

 protected:
  //: ID of this soview.
  unsigned id;

  //: Whether this soview is selectable.
  bool selectable;

  //: Style (colour, line width, etc) of this soview.
  vgui_style_sptr style;

 private:
  void add_id();
  static vgui_DLLDATA unsigned current_id;
};

inline vcl_ostream& operator<<(vcl_ostream& s, const vgui_soview& so)
{
  return so.print(s);
}

#endif // vgui_soview_h_
