#ifndef mvl2_image_format_plugin_h_
#define mvl2_image_format_plugin_h_
//:
// \file
// \brief Interface for loading avi frames as image formats
// \author Franck Bettinger
// \date   Sun Mar 17 22:57:00 2002
// This class implements the plugin to load avi frames as a new formats

#include <string>
#include <iostream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_resource_plugin.h>
#include <mvl2/mvl2_video_reader.h>

//=======================================================================

//: A base class for vil_image_view_base_sptr loading
// This class provides an interface for loading images in new formats
class mvl2_image_format_plugin : public vil_image_resource_plugin
{
 public:

  //: Default constructor
  mvl2_image_format_plugin();

  //: Destructor
  virtual ~mvl2_image_format_plugin();

  //: Name of the class
  virtual std::string is_a() const;

  //: Attempt to load image from named file.
  // \param filetype  String hinting at what image format is (currently unused)
  // \param colour define whether to load images as colour or grey-scale
  //        Options are '' (ie rely on image), 'Grey' or 'RGB'
  // \return true if successful
  virtual bool load_the_image(vil_image_view_base_sptr& image,
                              std::string const& path,
                              std::string const& filetype,
                              std::string const& colour="");

  virtual bool load_the_image(vil_image_view_base_sptr& image,
                              std::string const& path)
  { return load_the_image(image, path, "", ""); }

  //: Check whether a filename is a potential candidate for loading and if it is available.
  virtual bool can_be_loaded(const std::string& filename);

 protected:  // add user-defined methods after this line (do not remove)

  std::map<std::string,mvl2_video_reader*> mvl2_list_;

  //: Compute the real file name and extract the frame number from the path.
  //  The result is true if the file that will be used exists.
  bool get_frame_number_and_filename(
    std::string& filename, int& frame_number, const std::string& path);
};

#endif // mvl2_image_format_plugin_h_
