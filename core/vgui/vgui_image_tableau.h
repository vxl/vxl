// This is core/vgui/vgui_image_tableau.h
#ifndef vgui_image_tableau_h_
#define vgui_image_tableau_h_
//:
// \file
// \brief  Tableau which renders the given image using an image_renderer.
// \author fsm
//
//  Contains classes  vgui_image_tableau  vgui_image_tableau_new
//
// \verbatim
//  Modifications
//   15-AUG-2000 Marko Bacic,Oxford RRG -- Removed legacy ROI
//   09-AUG-2002 K.Y.McGaul - Added Doxygen style comments.
//   05-AUG-2003 Amitha Perera - Added new vil support
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>

class vgui_image_renderer;
class vil1_image;
#include <vil/vil_image_resource_sptr.h>

class vgui_vil_image_renderer;
#include <vil/vil_image_view_base.h>

struct vgui_image_tableau_new;

//: Tableau which renders the given image using an image_renderer.
class vgui_image_tableau
  : public vgui_tableau
{
  vcl_string name_;
  bool pixels_centered_;
  vgui_image_renderer* renderer_;
  vgui_vil_image_renderer* vil_renderer_;

 public:
  //: Returns the type of this tableau ('vgui_image_tableau').
  vcl_string type_name() const;

  //: Returns the filename of the loaded image.
  //  It will not know this if the tableau was constructed from
  //  a vil1_image, only if the filename was given to the constructor.
  vcl_string file_name() const;

  //: Set the filename of the loaded image
  //  It could be used to identify a specific image
  void set_file_name( const vcl_string & fn ) { name_ = fn; }

  //: Returns a nice version of the name, including details of the image file.
  vcl_string pretty_name() const;

  //: Return the image being rendered by this tableau.
  virtual vil1_image get_image() const;

  //: Return the image view being rendered by this tableau.
  virtual vil_image_view_base_sptr get_image_view() const;

  //: Return the image view being rendered by this tableau.
  virtual vil_image_resource_sptr get_image_resource() const;

  //: Make the given image, the image rendered by this tableau.
  virtual void set_image( vil1_image const &img );

  //: Make the given image view, the image rendered by this tableau.
  virtual void set_image_view( vil_image_view_base const& img );

  //: Make the given image resource, the image rendered by this tableau.
  virtual void set_image_resource( vil_image_resource_sptr const& img );

  //: Make image loaded from the given file, the image rendered by this tableau.
  //
  // The image will be stored as a vil_image_view_base, and thus can be
  // retrieved with get_image_view() but not with get_image().
  //
  virtual void set_image_view( char const* filename );

  //: Make image loaded from the given file, the image rendered by this tableau.
  //
  // The image will be stored as a vil1_image, and thus can be
  // retrieved with get_image() but not with get_image_view().
  //
  virtual void set_image( char const* filename );

  //: Reread the image from file.
  virtual void reread_image();

  //: Width of image (0 if none).
  virtual unsigned width() const;

  //: Height of image (0 if none).
  virtual unsigned height() const;

  //: Returns the box bounding the rendered image.
  virtual bool get_bounding_box( float low[3], float high[3] ) const;

  //: Sets coordinate to be in the middle or corner of the pixel.
  //  This method controls whether the coordinate (i, j) is the
  //  corner of pixel (i, j) or in the middle.
  void center_pixels( bool v = true ) { pixels_centered_ = v; }

  //: Handle all events sent to this tableau.
  //  In particular, use draw events to render the image contained in
  //  this tableau.
  virtual bool handle( vgui_event const& e );

 protected:
  friend struct vgui_image_tableau_new;

  //: Constructor - don't use this, use vgui_image_tableau_new.
  //  Creates an empty image tableau.
  vgui_image_tableau();

  //: Constructor - don't use this, use vgui_image_tableau_new.
  //  Creates a tableau displaying the given image.
  vgui_image_tableau( vil1_image const& img );

  //: Constructor - don't use this, use vgui_image_tableau_new.
  //  Creates a tableau displaying the given image view.
  vgui_image_tableau( vil_image_view_base const& img );

  //: Constructor - don't use this, use vgui_image_tableau_new.
  //  Creates a tableau displaying the given image resource.
  vgui_image_tableau( vil_image_resource_sptr const& img );

  //: Constructor - don't use this, use vgui_image_tableau_new.
  //  Creates a tableau which loads and displays an image from
  //  the given file. File loaded as a vil1_image.
  vgui_image_tableau( char const* filename );

 protected:
  //: Destructor - called by vgui_image_tableau_sptr.
  virtual ~vgui_image_tableau();
};

//: Creates a smart-pointer to a vgui_image_tableau.
struct vgui_image_tableau_new : public vgui_image_tableau_sptr {

  //: Constructor - creates an empty image tableau.
  vgui_image_tableau_new()
    : vgui_image_tableau_sptr( new vgui_image_tableau )
    { }

  //:  Constructor - creates a tableau displaying the given image.
  vgui_image_tableau_new( vil1_image const &t )
    : vgui_image_tableau_sptr( new vgui_image_tableau(t) )
    { }

  //:  Constructor - creates a tableau displaying the given image.
  vgui_image_tableau_new( vil_image_view_base const& t )
    : vgui_image_tableau_sptr( new vgui_image_tableau(t) )
    { }

  //:  Constructor - creates a tableau displaying the given image.
  vgui_image_tableau_new( vil_image_resource_sptr const& t )
    : vgui_image_tableau_sptr( new vgui_image_tableau(t) )
    { }

  //: Creates a tableau which loads and displays an image from the given file.
  //
  // The image will be stored as a vil1_image, and thus can be
  // retrieved with get_image() but not with get_image_view().
  //
  vgui_image_tableau_new(char const *f)
    : vgui_image_tableau_sptr( new vgui_image_tableau(f) )
    { }
};

#endif // vgui_image_tableau_h_
