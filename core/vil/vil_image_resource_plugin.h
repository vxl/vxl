// This is core/vil/vil_image_resource_plugin.h
#ifndef vil_image_resource_plugin_h_
#define vil_image_resource_plugin_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Interface for loading new image formats
//
// This class provides an interface for loading images in new formats
// \author      Franck Bettinger
// \date        Sun Mar 17 22:57:00 2002

#include <vcl_string.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_resource.h>

//=======================================================================

//: A base class for a plugin for vil images loading
// This class provides an interface for loading images in new formats

class vil_image_resource_plugin : public vil_image_resource
{
 public:

  //: Default constructor
  vil_image_resource_plugin() : filetype_(""), colour_(""), width_(-1), height_(-1) {}

  //: Destructor
  virtual ~vil_image_resource_plugin() {}

  //: Name of the class
  virtual vcl_string is_a() const { return "vil_image_resource_plugin"; }

  virtual vil_pixel_format pixel_format() const { return VIL_PIXEL_FORMAT_UNKNOWN; }

  virtual unsigned ni() const { return 0; }
  virtual unsigned nj() const { return 0; }
  virtual unsigned nplanes() const { return 0; }

  virtual bool get_property(char const * /*tag*/, void * /*property_value*/=0) const { return false; }
  virtual vil_image_view_base_sptr get_copy_view(unsigned /*i0*/, unsigned /*ni*/, unsigned /*j0*/, unsigned /*nj*/) const
  { return vil_image_view_base_sptr(0); }

  virtual bool put_view(vil_image_view_base const& /*im*/, unsigned /*i0*/, unsigned /*j0*/) { return false; }

  //: Attempt to load image from named file.
  // \return  true if successful
  virtual bool load_the_image(vil_image_view_base_sptr& image, const vcl_string & path)
  { return load_the_image(image,path,filetype_,colour_); }

  //: Attempt to load image from named file.
  // \param filetype  String hinting at what image format is
  // \param colour    define whether to load images as colour or grey-scale.
  //        Options are '' (i.e. rely on image), 'Grey' or 'RGB'
  // \return  true if successful
  virtual bool load_the_image(vil_image_view_base_sptr& image,
                              const vcl_string & path,
                              const vcl_string & filetype,
                              const vcl_string & colour);

  //: Register a vil_image_resource_plugin to the list of plugins
  static void register_plugin(vil_image_resource_plugin* plugin);

  //: Delete all registered plugins
  static void delete_all_plugins();

  //: Set the desired image size
  virtual void set_size(int width, int height) { width_=width; height_=height; }

  //: Check whether a filename is a potential candidate for loading and if it is available.
  virtual bool can_be_loaded(const vcl_string& filename);

  //: Set the colour options
  void set_colour(const vcl_string& colour) { colour_=colour; }

  //: Set the filetype options
  void set_filetype(const vcl_string& filetype) { filetype_=filetype; }

 protected:

  //: file type
  vcl_string filetype_;

  //: colour
  vcl_string colour_;

  vil_pixel_format pixel_format_;
  unsigned int ni_;
  unsigned int nj_;
  unsigned int nplanes_;
  int width_;
  int height_;
};

#endif // vil_image_resource_plugin_h_
