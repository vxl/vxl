// This is core/vil2/vil2_image_resource_plugin.h
#ifndef vil2_image_resource_plugin_h_
#define vil2_image_resource_plugin_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Interface for loading new image formats
// This class provides an interface for loading images in new formats
// \author      Franck Bettinger
// \date        Sun Mar 17 22:57:00 2002

#include <vcl_string.h>
#include <vil2/vil2_image_view_base.h>
#include <vil2/vil2_image_resource.h>

//=======================================================================

//: A base class for a plugin for vil2 images loading
// This class provides an interface for loading images in new formats
class vil2_image_resource_plugin : vil2_image_resource
{
 public:

    //: Default constructor
  vil2_image_resource_plugin();

    //: Destructor
  virtual ~vil2_image_resource_plugin();

    //: Name of the class
  virtual vcl_string is_a() const;

  virtual vil2_pixel_format pixel_format() const 
      { return VIL2_PIXEL_FORMAT_UNKNOWN; };
 
  virtual unsigned ni() const { return 0; };
  virtual unsigned nj() const { return 0; };
  virtual unsigned nplanes() const { return 0; };
 
  virtual bool get_property (char const *tag, void *property_value=0) const { return false; };
  virtual vil2_image_view_base_sptr get_copy_view (unsigned i0, unsigned ni, unsigned j0, unsigned nj) const { return vil2_image_view_base_sptr(0); };
 
  virtual bool put_view (const vil2_image_view_base &im, unsigned i0, unsigned j0) { return false; }; 

    //: Attempt to load image from named file. 
    // \param filetype  String hinting at what image format is
    // \param colour    define whether to load images as colour or grey-scale.
    //        Options are '' (i.e. rely on image), 'Grey' or 'RGB'
    // \ret   true if successful
  virtual bool load_the_image (vil2_image_view_base_sptr& image, 
                             const vcl_string & path);

    //: Attempt to load image from named file. 
    // \param filetype  String hinting at what image format is
    // \param colour    define whether to load images as colour or grey-scale.
    //        Options are '' (i.e. rely on image), 'Grey' or 'RGB'
    // \ret   true if successful
  virtual bool load_the_image (vil2_image_view_base_sptr& image, 
                             const vcl_string & path,
                             const vcl_string & filetype, 
                             const vcl_string & colour);

    //: Register a vil2_image_resource_plugin to the list of plugins
  static void register_plugin(vil2_image_resource_plugin* plugin);

    //: Delete all registered plugins
  static void delete_all_plugins();

    //: Set the desired image size 
  virtual void set_size(int width, int height);

    //: Check whether a filename is a potential candidate i
    // for loading and if it is available.
  virtual bool can_be_loaded(const vcl_string& filename);

    //: Set the colour options
  void set_colour(const vcl_string& colour);

    //: Set the filetype options
  void set_filetype(const vcl_string& filetype);

protected:

    //: file type
  vcl_string filetype_;

    //: colour
  vcl_string colour_;
  
  vil2_pixel_format pixel_format_;
  unsigned int ni_;
  unsigned int nj_;
  unsigned int nplanes_;
  int width_;
  int height_;
};

#endif // vil2_image_resource_plugin_h_
