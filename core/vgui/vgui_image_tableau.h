#ifndef vgui_image_tableau_h_
#define vgui_image_tableau_h_
// .NAME vgui_image_tableau
// .INCLUDE vgui/vgui_image_tableau.h
// .FILE vgui_image_tableau.cxx
//
// .SECTION Description
// The image_tableau renders the given image using an image_renderer.
//
// @see image_renderer
// @author fsm@robots.ox.ac.uk
// @bug 15-AUG-2000 Marko Bacic,Oxford RRG -- Removed legacy ROI

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>

class vgui_image_renderer;
class vil_image;

class vgui_image_tableau : public vgui_tableau {
public:
  vgui_image_tableau();
  vgui_image_tableau(vil_image const &);
  vgui_image_tableau(char const *);

  vcl_string type_name() const;
  vcl_string file_name() const;
  vcl_string pretty_name() const;

  vil_image get_image() const;
  void set_image(vil_image const &);
  void set_image(char const *);
  void reread_image();

  // size of image (0 if none).
  unsigned width() const;
  unsigned height() const;

  bool get_bounding_box(float low[3], float high[3]) const;
  // This method controls whether the coordinate (i, j) is the
  // corner of pixel (i, j) or in the middle.
  void center_pixels(bool v = true) { pixels_centered = v; }

  bool handle(vgui_event const &e);

protected:
  ~vgui_image_tableau();

private:
  vcl_string name_;
  bool pixels_centered;
  vgui_image_renderer *renderer;
};

struct vgui_image_tableau_new : public vgui_image_tableau_sptr {
  vgui_image_tableau_new() : vgui_image_tableau_sptr(new vgui_image_tableau) { }
  vgui_image_tableau_new(vil_image const &t) : vgui_image_tableau_sptr(new vgui_image_tableau(t)) { }
  vgui_image_tableau_new(char const *f) : vgui_image_tableau_sptr(new vgui_image_tableau(f)) { }
};

#endif // vgui_image_tableau_h_
