#ifndef mvl2_image_format_plugin_h_
#define mvl2_image_format_plugin_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Interface for loading avi frames as image formats
// \author Franck Bettinger
// \date   Sun Mar 17 22:57:00 2002
// This class implements the plugin to load avi frames as a new formats

#include <vcl_string.h>
#include <vcl_map.h>
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
  virtual vcl_string is_a() const;

  //: Attempt to load image from named file.
  // \param filetype  String hinting at what image format is (currently unused)
  // \param colour define whether to load images as colour or grey-scale
  //        Options are '' (ie rely on image), 'Grey' or 'RGB'
  //!ret: true if successful
  virtual bool load_the_image (vil_image_view_base_sptr& image,
                               vcl_string const& path,
                               vcl_string const& filetype="",
                               vcl_string const& colour="");

  //: Check whether a filename is a potential candidate for loading and if it is available.
  virtual bool can_be_loaded(const vcl_string& filename);

 protected:  // add user-defined methods after this line (do not remove)

  vcl_map<vcl_string,mvl2_video_reader*> mvl2_list_;

  //: Compute the real file name and extract the frame number from the path.
  //  The result is true if the file that will be used exists.
  bool get_frame_number_and_filename(
    vcl_string& filename, int& frame_number, const vcl_string& path);
};

#endif // mvl2_image_format_plugin_h_
