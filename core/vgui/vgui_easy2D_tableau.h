// This is core/vgui/vgui_easy2D_tableau.h
#ifndef vgui_easy2D_tableau_h_
#define vgui_easy2D_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   24 Sep 1999
// \brief  Tableau to display two-dimensional geometric objects.
//
//  Contains classes: vgui_easy2D_tableau  vgui_easy2D_tableau_new
//
// \verbatim
// Modifications
//    24-SEP-1999  P.Pritchett - Initial version.
//    20-JUL-2000  Marko Bacic - Provided support for printing linestrips
//    25-APR-2002  domi at vision.ee.ethz.ch - make print_psfile work without
//                                             image tableau
//    26-APR-2002  K.Y.McGaul - Added some doxygen-style documentation.
//    01-OCT-2002  K.Y.McGaul - Moved vgui_easy2D to vgui_easy2D_tableau.
//                            - Added functions clear, remove, get_all.
// \endverbatim

#include <vgui/vgui_displaylist2D_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>

class vgui_soview2D;
class vgui_soview2D_point;
class vgui_soview2D_lineseg;
class vgui_soview2D_infinite_line;
class vgui_soview2D_circle;
class vgui_soview2D_ellipse;
class vgui_soview2D_linestrip;
class vgui_soview2D_polygon;
class vgui_soview2D_image;

//: Tableau to display two-dimensional geometric objects.
//
//  Two dimensional geometric objects (see vgui_soview2D) such as lines,
//  points, circles, etc can be added using add, or add_point, add_line,
//  add_circle, etc.
//
//  The constructor of this tableau can accept a child tableau such as an
//  image or clear tableau on top of which the geometric objects will be
//  displayed (so for example, lines can be drawn on an image).  To display
//  objects on a non-black, plain coloured background use a clear_tableau
//  and set its colour.
//
//  To remove objects call remove(vgui_soview*).  The vgui_soview* is returned
//  by add(), or you can get a list of all soviews using get_all().  To remove
//  all objects use clear().
//
//  The geometric objects on the easy2D (and any underlying image) can be saved
//  as PostScript by calling print_psfile. (If you just wish to save an image
//  of your easy2D you may want to use vgui_utils::dump_colour_buffer instead).
class vgui_easy2D_tableau : public vgui_displaylist2D_tableau
{
 public:
  //: Constructor - don't use this, use vgui_easy2D_tableau_new.
  //  Create an easy2D with the given name.
  vgui_easy2D_tableau(const char* n="unnamed");

  //: Constructor - don't use this, use vgui_easy2D_tableau_new.
  //  Creates an easy2D with the given image tableau as child.
  vgui_easy2D_tableau(vgui_image_tableau_sptr const&, const char* n="unnamed");

  //: Constructor - don't use this, use vgui_easy2D_tableau_new.
  //  Creates an easy2D with the given child tableau.
  vgui_easy2D_tableau(vgui_tableau_sptr const&, const char* n="unnamed");

  //: Handle all events sent to this tableau.
  //  In particular, use draw events to draw 2-dimensional geometric objects.
  bool handle(const vgui_event& e);

  //: Returns the name given to this tableau in the constructor.
  vcl_string name() const { return name_; };

  //: Returns the filename of the child image if there is one, else the name.
  vcl_string file_name() const;

  //: Returns a nice version of the type, including details of any image file.
  vcl_string pretty_name() const;

  //: Returns the type of this tableau ('vgui_easy2D_tableau').
  vcl_string type_name() const;

  //: Set the child tableau to be the given image_tableau.
  void set_image(vcl_string const& image);

  //: Set the child tableau to be the given tableau.
  void set_child(vgui_tableau_sptr const&);

  //: Set the colour of objects to the given red, green, blue values.
  void set_foreground(float r, float g, float b)
    { fg[0] = r; fg[1] = g; fg[2] = b; }

  //: Set the width of lines to the given width.
  void set_line_width(float w) { line_width = w; }

  //: Set the radius of points to the given radius.
  void set_point_radius(float r) { point_size = r; }

  //: Add the given two-dimensional object to the display.
  void add(vgui_soview2D*);

  //: Add a point at the given position to the display.
  vgui_soview2D_point* add_point(float x, float y);

  //: Add a finite line with the given start and end points to the display.
  //  Note that this will be added as a vgui_lineseg (not
  //  vgui_line - which doesn't exist).
  vgui_soview2D_lineseg* add_line(float x0, float y0, float x1, float y1);

  //: Add an infinite line (ax + by +c = 0) to the display.
  vgui_soview2D_infinite_line* add_infinite_line(float a, float b, float c);

  //: Add a circle with the given centre and radius to the display.
  vgui_soview2D_circle* add_circle(float x, float y, float r);

  //: Add an ellipse with a given center, width, height, and angle
  vgui_soview2D_ellipse* add_ellipse(float x, float y, float w,
    float h, float phi);

  //: Add a point with the given projective coordinates.
  vgui_soview2D_point* add_point_3dv(double const p[3]);

  //: Add a line with the given projective start and end points.
  vgui_soview2D_lineseg* add_line_3dv_3dv(double const p1[3],
                                          double const p2[3]);

  //: Add an infinite line with the given projective coordinates.
  vgui_soview2D_infinite_line* add_infinite_line_3dv(double const l[3]);

  //: Add a circle with the given centre (in projective coords) and radius.
  vgui_soview2D_circle* add_circle_3dv(double const point[3], float r);

  //: Add a linestrip with the given n vertices to the display.
  vgui_soview2D_linestrip* add_linestrip(unsigned n, float const *x,
    float const *y);

  //: Add  polygon with the given n vertices to the display.
  vgui_soview2D_polygon* add_polygon(unsigned n, float const *x,
    float const *y);

  //: Remove the given soview from the display.
  void remove(vgui_soview* vso) { vgui_displaybase_tableau::remove(vso); }

  //: Clear all soviews from the display.
  void clear() { vgui_displaybase_tableau::clear(); }

  //: Returns a list of all soviews on the display.
  vcl_vector<vgui_soview*> const &get_all() const
  { return vgui_displaybase_tableau::get_all(); }

  //: If the child tableau is an image_tableau, return this.
  vgui_image_tableau_sptr get_image_tableau() { return image_image; }

  //: Screen dump to postscript file.
  //  Specify the optional arguments in case this tableau does not contain
  //  an image tableau, or if you want a smaller part of the image printed.
  //  If wd or ht are 0, no image is printed at all.
  void print_psfile(vcl_string filename, int reduction_factor,
                    bool print_geom_objs, int wd=-1, int ht=-1);

  // deprecated. This used to work without a segfault if you were
  // lucky. The newer versions that take a image parameter will work
  // more reliably.
  vgui_soview2D_image* add_image(float x, float y, float w, float h, char *data, unsigned int format, unsigned int type);

  //: Add an image at the given position to the display.
  //
  // Alpha blending will be turned on iff the view has 4 planes.
  //
  vgui_soview2D_image* add_image( float x, float y, vil_image_view_base const& img );

  //: Add an image at the given position to the display.
  //
  // Alpha blending will be turned on iff the image has 4 components.
  //
  vgui_soview2D_image* add_image( float x, float y, vil1_image const& img );

 protected:
  //: Destructor - called by vgui_easy2D_tableau_sptr.
  ~vgui_easy2D_tableau() { }

  //: Child tableau if there is one.
  vgui_parent_child_link image_slot;

  //: Child image tableau, if there is one.
  vgui_image_tableau_sptr image_image;

  //: Name of this tableau.
  vcl_string name_;

  //: Foreground colour (colour of the objects).
  float fg[3];

  //: Line width for geometric objects added to the display.
  float line_width;

  //: Point radius for geometric objects added to the display.
  float point_size;
};

//: Create a smart-pointer to a vgui_easy2D_tableau tableau.
struct vgui_easy2D_tableau_new : public vgui_easy2D_tableau_sptr
{
  //: Constructor - create an easy2D with the given name.
  vgui_easy2D_tableau_new(char const *n="unnamed") :
    vgui_easy2D_tableau_sptr(new vgui_easy2D_tableau(n)) { }

  //: Constructor - create an easy2D with the given image tableau as child.
  vgui_easy2D_tableau_new(vgui_image_tableau_sptr const& i, char const* n="unnamed") :
    vgui_easy2D_tableau_sptr(new vgui_easy2D_tableau(i, n)) { }

  //: Constructor - create an easy2D with the given child tableau.
  vgui_easy2D_tableau_new(vgui_tableau_sptr const& i, char const* n="unnamed") :
    vgui_easy2D_tableau_sptr(new vgui_easy2D_tableau(i, n)) { }
};

#endif // vgui_easy2D_tableau_h_
