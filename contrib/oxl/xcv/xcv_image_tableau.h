#ifndef xcv_image_tableau_h_
#define xcv_image_tableau_h_
/*
  u97mb@robots.ox.ac.uk
*/

// .NAME	xcv_image_tableau - An image tableau with an associated region of interest
// .INCLUDE	xcv/xcv_image_tableau.h
// .FILE	xcv_image_tableau.cxx

#include "xcv_image_tableau_sptr.h"
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_rubberbander.h>

class xcv_image_tableau : public vgui_image_tableau
{
public:
  typedef vgui_image_tableau base;

  xcv_image_tableau();
  xcv_image_tableau(vil_image const &);
  xcv_image_tableau(char const *);

  vcl_string type_name() const;

  // get
  vil_image get_image() const;

  // set
  void set_image(vil_image const &);
  void set_image(char const *);          // <- convenience

  // size of image (0 if none).
  unsigned width() const;
  unsigned height() const;

  bool get_bounding_box(float low[3], float high[3]) const;

  // Define region of interest
  void set_roi(float,float,float,float);

  // Undefine region of interest
  void unset_roi();

protected:
  bool handle(vgui_event const &e);

private:
//  vcl_string name_;
//  vgui_image_renderer *please;
  struct ROI {
    float x;
    float y;
    float width;
    float height;
  };
  ROI roi_;
  bool defined_;
};

struct xcv_image_tableau_new : public xcv_image_tableau_sptr {
  typedef xcv_image_tableau_sptr base;
  xcv_image_tableau_new() : base(new xcv_image_tableau()) { }
  xcv_image_tableau_new(vil_image const &i) : base(new xcv_image_tableau(i)) { }
  xcv_image_tableau_new(char const *n) : base(new xcv_image_tableau(n)) { }
  operator vgui_image_tableau_sptr () const { vgui_image_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

class vgui_roi_tableau_make_roi : public vgui_rubberbander_client {
public:
  vgui_roi_tableau_make_roi(xcv_image_tableau_sptr const&);
  void add_box(float,float,float,float);
  void add_point(float,float) {}
  void add_line(float,float,float,float) {}
  void add_circle(float,float,float) {}
  void add_linestrip(int,float const *,float const *) {}
  void add_infinite_line(float,float,float) {}
  bool is_done()
  {return done_;}
private:
  bool done_;
  xcv_image_tableau_sptr image_tableau_;
};

#endif // xcv_image_tableau_h_
