#ifndef mil_byte_image_2d_io_plugin_h_
#define mil_byte_image_2d_io_plugin_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \brief Interface for loading new image formats
// This class provides an interface for loading images in new formats
// \author      Franck Bettinger
// \date        Sun Mar 17 22:57:00 2002

#include <vcl_string.h>
#include <mil/mil_image_2d_of.h>
#include <vil/vil_byte.h>

//=======================================================================

//: A base class for mil_byte_image_2d_of<vil_byte> loading
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

  //: Attempt to load image from named file. 
  // \param filetype  String hinting at what image format is
  // \param colour define whether to load images as colour or grey-scale
  //        Options are '' (ie rely on image), 'Grey' or 'RGB'
  //!ret: true if successful
  virtual bool loadTheImage (mil_image_2d_of<vil_byte>& image, 
      const vcl_string & path, const vcl_string & filetype, 
      const vcl_string & colour);

  //: Register a mil_byte_image_2d_io_plugin to the list of plugins
  static void register_plugin(mil_byte_image_2d_io_plugin* plugin);

  //: Delete all registered plugins
  static void delete_all_plugins();

  //: Check wether a filename is a potential candidate for loading and if it is available.
  virtual bool can_be_loaded(const vcl_string& filename);
};

#endif // mil_byte_image_2d_io_plugin_h_
