// This is core/vgui/vgui_vil_image_tableau.h
#ifndef vgui_vil_image_tableau_h_
#define vgui_vil_image_tableau_h_

//:
// \file
// \author Amitha Perera
// \brief  Tableau which renders the given vil_image_view using an image_renderer.
//
//  Mostly cut-n-paste from vgui_image_tableau.
//

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_vil_image_tableau_sptr.h>

class vgui_vil_image_renderer;

#include <vil/vil_fwd.h>
#include <vil/vil_image_view.h>

//: Captures the non-templated functionality of vgui_vil_image_tableau.
//
// \deprecated Use vgui_image_tableau instead. It now supports
// vil_image_view objects.
//
// This is useful to deal with the tableau without worrying about the
// type of image displayed by the tableau. Typically, you will search
// the tableau hierarchy for "vgui_vil_image_tableau" and vertical
// cast the resulting pointer to a "vgui_vil_image_tableau_base".
//
class vgui_vil_image_tableau_base : public vgui_tableau
{
 public:
  //: Returns the type of this tableau ('vgui_vil_image_tableau').
  virtual vcl_string type_name() const = 0;

  //: Returns a nice version of the name, including details of the image type.
  virtual vcl_string pretty_name() const = 0;

  //: Width of image (0 if none).
  virtual unsigned width() const = 0;

  //: Height of image (0 if none).
  virtual unsigned height() const = 0;

  //: Returns the box bounding the rendered image.
  virtual bool get_bounding_box( float low[3], float high[3] ) const = 0;

  //: Sets coordinate to be in the middle or corner of the pixel.
  //  This method controls whether the coordinate (i, j) is the
  //  corner of pixel (i, j) or in the middle.
  virtual void center_pixels( bool v = true ) = 0;

  //: Handle all events sent to this tableau.
  //  In particular, use draw events to render the image contained in
  //  this tableau.
  virtual bool handle( vgui_event const &e ) = 0;

  virtual ~vgui_vil_image_tableau_base() { };
};


template <class T>
struct vgui_vil_image_tableau_new;

//: Tableau which renders the given image view using an image_renderer.
//
// \deprecated Use vgui_image_tableau instead. It now supports
// vil_image_view objects.
//
// Use vgui_vil_image_tableau_new to construct objects of this class.
template <class T>
class vgui_vil_image_tableau : public vgui_vil_image_tableau_base
{
 public:
  //: Returns the type of this tableau ('vgui_vil_image_tableau').
  //
  // The image type name is *not* encoded, because encoding it means
  // that you will generally be unable to find this tableau in the
  // hierarchy.
  //
  vcl_string type_name() const;

  //: Returns a nice version of the name, including details of the image type.
  vcl_string pretty_name() const;

  //: Return the image being rendered by this tableau.
  vil_image_view<T> get_image() const;

  //: Make the given image, the image rendered by this tableau.
  void set_image( vil_image_view<T> const &img );

  //: Width of image (0 if none).
  unsigned width() const;

  //: Height of image (0 if none).
  unsigned height() const;

  //: Returns the box bounding the rendered image.
  bool get_bounding_box( float low[3], float high[3] ) const;

  //: Sets coordinate to be in the middle or corner of the pixel.
  //  This method controls whether the coordinate (i, j) is the
  //  corner of pixel (i, j) or in the middle.
  void center_pixels( bool v = true ) { pixels_centered_ = v; }

  //: Handle all events sent to this tableau.
  //  In particular, use draw events to render the image contained in
  //  this tableau.
  bool handle( vgui_event const &e );

 protected:
  //: Destructor - called by vgui_vil_image_tableau_sptr.
  ~vgui_vil_image_tableau();

 private:
  vgui_vil_image_tableau();

  vgui_vil_image_tableau( vil_image_view<T> const &img );

  friend struct vgui_vil_image_tableau_new<T>;

 private:
  bool pixels_centered_;
  vgui_vil_image_renderer* renderer_;
};

//: Creates a smart-pointer to a vgui_vil_image_tableau.
//
// \deprecated Use vgui_image_tableau_new instead. It now supports
// vil_image_view objects.
template <class T>
struct vgui_vil_image_tableau_new : public vgui_vil_image_tableau_sptr<T> {

  //: Constructor - creates an empty image tableau.
  vgui_vil_image_tableau_new()
    : vgui_vil_image_tableau_sptr<T>( new vgui_vil_image_tableau<T> ) { }

  //: Constructor - creates a tableau displaying the given image.
  vgui_vil_image_tableau_new( vil_image_view<T> const &t )
    : vgui_vil_image_tableau_sptr<T>( new vgui_vil_image_tableau<T>(t) ) { }
};

#endif // vgui_vil_image_tableau_h_
