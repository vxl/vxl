// This is mul/mil/mil_byte_image_2d_io_plugin.h
#ifndef mil_byte_image_2d_io_plugin_h_
#define mil_byte_image_2d_io_plugin_h_
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
#include <mil/mil_image_2d_of.h>
#include <vxl_config.h>

//=======================================================================

//: A base class for mil_byte_image_2d_of<vxl_byte> loading
// This class provides an interface for loading images in new formats
class mil_byte_image_2d_io_plugin
{
 public:

  //: Default constructor
  mil_byte_image_2d_io_plugin();

  //: Destructor
  virtual ~mil_byte_image_2d_io_plugin();

  //: Name of the class
  virtual vcl_string is_a() const;

  //: True if this is (or is derived from) class named s
  virtual bool is_class(vcl_string const& s) const;

  //: Attempt to load image from named file.
  // \param filetype  String hinting at what image format is
  // \param colour    define whether to load images as colour or grey-scale.
  //        Options are '' (i.e. rely on image), 'Grey' or 'RGB'
  // \ret   true if successful
  virtual bool loadTheImage (mil_image_2d_of<vxl_byte>& image,
                             const vcl_string & path,
                             const vcl_string & filetype,
                             const vcl_string & colour);

  //: Register a mil_byte_image_2d_io_plugin to the list of plugins
  static void register_plugin(mil_byte_image_2d_io_plugin* plugin);

  //: Delete all registered plugins
  static void delete_all_plugins();

  //: Check whether a filename is a potential candidate for loading and if it is available.
  virtual bool can_be_loaded(const vcl_string& filename);
};

#endif // mil_byte_image_2d_io_plugin_h_
