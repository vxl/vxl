#ifndef vgui_soview_h_
#define vgui_soview_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \brief Undocumented class FIXME
//
// Notes:
//   We use floats instead of doubles as size is a speed issue (sic.)
//
// \author
//              Philip C. Pritchett, 24 Mar 99
//              Robotics Research Group, University of Oxford
//-----------------------------------------------------------------------------

#include "dll.h"

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>

class vgui_observer;
class vgui_message;
class vgui_style;

class vgui_soview /*: public vgui_observable*/ {
public:
  vgui_soview();
  virtual ~vgui_soview() {}

  virtual void draw() = 0;
  virtual void load_name();
  virtual vcl_ostream& print(vcl_ostream&) const;

  virtual vcl_string type_name() const;

  // style
  virtual void set_style(vgui_style*);
  virtual vgui_style* get_style();

  void set_colour(float r, float g, float b);
  void set_point_size(float s);
  void set_line_width(float w);

  // observers
  void attach(vgui_observer*);
  void detach(vgui_observer*);
  void get_observers(vcl_vector<vgui_observer*>&) const;
  void notify() const;
  void notify(vgui_message const &) const;

  // fsm. new old message model
  static vgui_DLLDATA const void * const msg_select;
  static vgui_DLLDATA const void * const msg_deselect;

  // id management
  virtual unsigned get_id() {return id;}
  static vgui_soview* id_to_object(unsigned id);

  static unsigned create_id();

  // decide if it is possible to select this object
  bool get_selectable() const { return selectable; }
  void set_selectable( bool s) { selectable= s; }

protected:
  unsigned id;
  bool selectable;
  vgui_style* style;

private:
  void add_id();
  static vgui_DLLDATA unsigned current_id;
};

inline vcl_ostream& operator<<(vcl_ostream& s, const vgui_soview& so)
{
  return so.print(s);
}

#endif // vgui_soview_h_
