// This is brl/bbas/bhdfs/bhdfs_vil_load.cxx
//:
// \file

#include <vil/vil_load.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_plugin.h>
#include <vil/vil_image_view.h>
#include <vil/vil_exception.h>

#include "bhdfs_vil_load.h"
#include "bhdfs_vil_stream.h"

vil_image_resource_sptr bhdfs_vil_load_image_resource_raw(char const* filename,
                                                          bool verbose)
{
  vil_smart_ptr<vil_stream> is = new bhdfs_vil_stream(filename, "r");
  vil_image_resource_sptr isp = 0;
  if (is)
  {
    try
    {
      isp = vil_load_image_resource_raw(is.as_pointer(), verbose);
    }
    catch (const vil_exception_corrupt_image_file &e)
    {
      throw vil_exception_corrupt_image_file(e.function_name, e.file_type, filename, e.details);
    }
  }

  if (!isp && verbose)
    std::cerr << __FILE__ ": Failed to load [" << filename << "]\n";
  return isp;
}

vil_image_resource_sptr bhdfs_vil_load_image_resource(char const* filename,
                                                      bool verbose)
{
  //vil_image_resource_sptr im = bhdfs_vil_load_image_resource_plugin(filename);
  //if (!im)
  //  im=bhdfs_vil_load_image_resource_raw(filename, verbose);
  vil_image_resource_sptr im = bhdfs_vil_load_image_resource_raw(filename, verbose);
  if (!im && verbose)
    std::cerr << __FILE__ ": Failed to load [" << filename << "]\n";
  return im;
}

#if 0 // function commented out
vil_image_resource_sptr vil_load_image_resource_plugin(char const* filename)
{
  vil_image_resource_plugin im_resource_plugin;
  if (im_resource_plugin.can_be_loaded(filename))
  {
    vil_image_view_base* img=new vil_image_view<vxl_byte>(640,480,3);
    vil_image_resource_sptr im;
    vil_image_view_base_sptr im_view(img);
    if (im_resource_plugin.load_the_image(im_view,filename))
    {
      im=vil_new_image_resource(im_view->ni(),im_view->nj(),
                                im_view->nplanes(),im_view->pixel_format());
      if (im->put_view((const vil_image_view_base&)*im_view,0,0))
        return im;
    }
  }
  return vil_image_resource_sptr(0);
}
#endif // 0

#if 0 // function commented out
vil_pyramid_image_resource_sptr
vil_load_pyramid_resource(char const* directory_or_file, bool verbose)
{
  for (vil_file_format** p = vil_file_format::all(); *p; ++p) {
#if 0 // debugging
    std::cerr << __FILE__ " : trying \'" << (*p)->tag() << "\'\n";


    std::cerr << "make_input_pyramid_image(" << directory_or_file << ")\n";
#endif
    vil_pyramid_image_resource_sptr pir =
      (*p)->make_input_pyramid_image(directory_or_file);
    if (pir)
      return pir;
  }
  // failed.
  if (verbose) {
    std::cerr << __FILE__ ": Unable to load pyramid image;\ntried";
    for (vil_file_format** p = vil_file_format::all(); *p; ++p)
      // 'flush' in case of segfault next time through loop. Else, we
      // will not see those printed tags still in the stream buffer.
      std::cerr << " \'" << (*p)->tag() << "\'" << std::flush;
    std::cerr << std::endl;
  }
  return 0;
}
#endif // 0

//: Convenience function for loading an image into an image view.
vil_image_view_base_sptr bhdfs_vil_load(const char *file, bool verbose)
{
  //vil_image_resource_sptr data = vil_load_image_resource(file, verbose);
  vil_image_resource_sptr data = bhdfs_vil_load_image_resource_raw(file, verbose);
  if (!data) return 0;
  return data -> get_view();
}

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//  --------------------------------------------------------------------------------
//  Windows' wchar_t overloading version
//
//
#if 0 // function commented out
vil_image_resource_sptr vil_load_image_resource_raw(wchar_t const* filename, bool verbose)
{
  vil_smart_ptr<vil_stream> is = vil_open(filename, "r");
  vil_image_resource_sptr isp = 0;
  if (is)
    isp = vil_load_image_resource_raw(is.as_pointer(), verbose);
  if (!isp && verbose)
    std::wcerr << __FILE__ << L": Failed to load [" << filename << L"]\n";
  return isp;
}
#endif

#if 0 // function commented out
vil_image_resource_sptr vil_load_image_resource(wchar_t const* filename, bool verbose)
{
  // do not support image resource plugin for the time being
  //vil_image_resource_sptr im = vil_load_image_resource_plugin(filename);
  //if (!im)
  //  im=vil_load_image_resource_raw(filename);
  vil_image_resource_sptr im = vil_load_image_resource_raw(filename);
  return im;
}
#endif // 0

#if 0 // function commented out
//: Convenience function for loading an image into an image view.
vil_image_view_base_sptr vil_load(const wchar_t *file, bool verbose)
{
  vil_image_resource_sptr data = vil_load_image_resource(file, verbose);
  if (!data) return 0;
  return data -> get_view();
}
#endif // 0

#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T
