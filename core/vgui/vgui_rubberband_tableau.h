// This is core/vgui/vgui_rubberband_tableau.h
#ifndef vgui_rubberband_tableau_h_
#define vgui_rubberband_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author  K.Y.McGaul
// \date    31-MAR-2000
// \brief   Tableau to rubberband circles, lines etc.
//
//  Contains classes: vgui_rubberband_tableau  vgui_rubberband_tableau_new
//                    vgui_rubberband_client  vgui_rubberband_easy2D_client
//
// \verbatim
//  Modifications
//   31-MAR-2000 K.Y.McGaul - Initial version.
//   31-MAR-2000 F.S. de M - Minor cosmetic changes.
//   07-JUL-2000 Marko Bacic - Added support for linestrip
//   19-JUL-2000 Marko Bacic - Now supports vgui_rubberband_client
//   14-AUG-2000 FSM - Fixed so that it works with Windows
//   17-SEP-2002 K.Y.McGaul - Added doxygen style comments.
//   01-OCT-2002 K.Y.McGaul - Moved vgui_rubberbander to vgui_rubberband_tableau
//   26-JUN-2003 Mark Johnson - Added clear_highlight method to client
// \endverbatim


#include <vgui/vgui_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_event_condition.h>

//-----------------------------------------------------------------------------
//: Receives the parameters captured by vgui_rubberband_tableau.
class vgui_rubberband_client
{
 public:
  //: Destructor - delete this vgui_rubberband_client.
  virtual ~vgui_rubberband_client() {}

  //: Called by vgui_rubberband_tableau when the user has selected a point.
  virtual void add_point(float, float);

  //: Called by vgui_rubberband_tableau when the user has selected a finite line.
  virtual void add_line(float,float,float,float);

  //: Called by vgui_rubberband_tableau when user has selected an infinite line.
  virtual void add_infinite_line(float,float,float);

  //: Called by vgui_rubberband_tableau when the user has selected a circle.
  virtual void add_circle(float,float,float);

  //: Called by vgui_rubberband_tableau when the user has selectd a linestrip.
  virtual void add_linestrip(int n,float const *,float const *);

  //: Called by vgui_rubberband_tableau when the user has selected a polygon.
  virtual void add_polygon(int n,float const*,float const*);

  //: Called by vgui_rubberband_tableau when user has selected a rectangular box
  virtual void add_box(float,float,float,float);

  //: Called by vgui_rubberband_tableau whenever mouse motion is captured
  virtual void clear_highlight();
};

//-----------------------------------------------------------------------------
//: Rubberbanding onto a vgui_easy2D_tableau.
//
//  Special case of vgui_rubberband_client for cases where we just want to
//  draw rubberbanded objects straight onto a vgui_easy2D_tableau.
class vgui_rubberband_easy2D_client : public vgui_rubberband_client
{
 public:
  //: Pointer to the vgui_easy2D_tableau object this client writes to.
  vgui_easy2D_tableau_sptr easy;

  //: Constructor - takes a pointer to a vgui_easy2D_tableau.
  vgui_rubberband_easy2D_client(vgui_easy2D_tableau_sptr const& e) : easy(e) {}

  //: Called by vgui_rubberband_tableau when the user has selected a point.
  //  Adds a point to the vgui_easy2D_tableau.
  void add_point(float x, float y);

  //: Called by vgui_rubberband_tableau when the user has selected a finite line.
  //  Adds a finite line to the vgui_easy2D_tableau.
  void add_line(float x0, float y0, float x1, float y1);

  //: Called by vgui_rubberband_tableau when user has selected an infinite line.
  //  Adds an infinite line to the vgui_easy2D_tableau.
  void add_infinite_line(float a, float b, float c);

  //: Called by vgui_rubberband_tableau when the user has selected a circle.
  //  Adds a circle to the vgui_easy2D_tableau.
  void add_circle(float x, float y, float r);

  //: Called by vgui_rubberband_tableau when the user has selectd a linestrip.
  //  Adds a linestrip to the vgui_easy2D_tableau.
  void add_linestrip(int n, float const* x, float const* y);

  //: Called by vgui_rubberband_tableau when the user has selected a polygon.
  //  Adds a polygon to the vgui_easy2D_tableau.
  void add_polygon(int n, float const* x, float const* y);

  //: Called by vgui_rubberband_tableau when the user has selected a box.
  //  Since vgui_easy2D_tableau doesn't have a rectangular box object, this
  //  add an equivalent 4 point polygon instead.
  void add_box(float x0, float y0, float x1, float y1);

  //: Called by vgui_rubberband_tableau whenever mouse motion is captured.
  //  This turns off the highlighting of geometry objects to eliminate
  //  flickering highlights while drawing temporary objects.
  void clear_highlight();
};

//-----------------------------------------------------------------------------
//: Tableau to rubberband circles, lines etc.
//
//  The values captured (eg. two points defining a line) are passed to the
//  appropriate function (eg. add_line) in the client passed in to the
//  constructor.  This client is derived from vgui_rubberband_client.
//
//  In more detail:
//
//  The user draws the object in the rendering area using the defined
//  'gestures'.  For example, to draw a line, the first gesture is a mouse
//  down event defining the start point.  A line is shown on the display
//  between the start point and the mouse pointer until the second gesture
//  is performed (releasing the mouse button) and this defines the end point.
//
//  These values are passed to add_line in the client.  These values could
//  be used to draw the line (like vgui_rubberband_easy2D_client) or
//  you can create your own client derived from vgui_rubberband_client to
//  do something different.
//
//  See xcv to see a rubberbander in action (used to add geometric objects).
//
//  If you want to get values from a user and you want your code to wait
//  until the values have been collected then rubberbander won't do this.
//  You need to write something like ./oxl/xcv/xcv_picker_tableau that
//  grabs the event loop.
class vgui_rubberband_tableau : public vgui_tableau
{
 public:
  //: Whether to use overlays or not.
  bool use_overlays;  // capes@robots - default is true

  //: Initialise vgui_rubberband_tableau.
  void init (vgui_rubberband_client* client);

  //: Constructor - don't use this, use vgui_rubberband_tableau_new.
  //  Takes the vgui_rubberband_client as a parameter.
  vgui_rubberband_tableau(vgui_rubberband_client* client) { init(client); }

//vgui_rubberband_tableau(vgui_easy2D_tableau_sptr const& e) { init(new vgui_rubberband_tableau_easy2D_client(e)); }

  //: Return the type of this tableau ('vgui_rubberband_tableau').
  vcl_string type_name() const { return "vgui_rubberband_tableau"; }

  // These describe what the user has to do to use the rubberbanding gesture.
  vgui_event_condition gesture0;
  vgui_event_condition gesture1;
  vgui_event_condition gesture2;

  void rubberband_point();
  void rubberband_line();
  void rubberband_infinite_line();
  void rubberband_circle();
  void rubberband_polygon();
  void rubberband_linestrip(); // u97mb
  void rubberband_box();
  void rubberband_none();

  vgui_rubberband_client* get_client() { return client_; }
  void set_client(vgui_rubberband_client *client) { client_ = client; }

  void draw_point(float x0, float y0);
  void draw_line(float x0, float y0, float x1, float y1);
  void draw_infinite_line(float a, float b, float c); // ax + by + c = 0
  void draw_circle(float x0, float y0, float r);
  void draw_linestrip(float x,float y); // u97mb
  void draw_polygon(float x, float y);
  void draw_box(float x0,float y0, float x1,float y1);

 protected:
  ~vgui_rubberband_tableau() {}
  bool handle_point(vgui_event const&, float, float);
  bool handle_line(vgui_event const&, float, float);
  bool handle_linestrip(vgui_event const&,float , float ); // u97mb
  bool handle_infinite_line(vgui_event const&, float, float);
  bool handle_circle(vgui_event const&, float, float);
  bool handle_polygon(vgui_event const&, float, float);
  bool handle_box(vgui_event const&, float, float);
  bool handle(vgui_event const&);

 private:
  vgui_rubberband_client *client_;
  enum object_type {none_enum, point_enum, line_enum, infinite_line_enum,
                    circle_enum, polygon_enum, linestrip_enum,box_enum};
  bool active;
  static object_type obj_type;
  float lastx, lasty;   // position where mouse was last seen.
  vcl_vector<float>x_coords, y_coords;
};

//-----------------------------------------------------------------------------
typedef vgui_tableau_sptr_t<vgui_rubberband_tableau> vgui_rubberband_tableau_sptr;

//: Creates a smart-pointer to a vgui_rubberband_tableau tableau.
struct vgui_rubberband_tableau_new : public vgui_rubberband_tableau_sptr
{
  vgui_rubberband_tableau_new(vgui_rubberband_client* client)
    : vgui_rubberband_tableau_sptr(new vgui_rubberband_tableau(client)) {}
};

#endif // vgui_rubberband_tableau_h_
