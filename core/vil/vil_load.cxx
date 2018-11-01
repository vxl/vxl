// This is core/vil/vil_load.cxx
//:
// \file

#include <iostream>
#include "vil_load.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_open.h>
#include <vil/vil_new.h>
#include <vil/vil_file_format.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_plugin.h>
#include <vil/vil_image_view.h>
#include <vil/vil_exception.h>

vil_image_resource_sptr vil_load_image_resource_raw(vil_stream *is,
                                                    bool verbose)
{
  std::list<vil_file_format*>& l = vil_file_format::all();
  for (auto & p : l) {
#if 0 // debugging
    std::cerr << __FILE__ " : trying \'" << (*p)->tag() << "\'\n";
#endif
    is->seek(0);
    vil_image_resource_sptr im = p->make_input_image(is);
    if (im)
      return im;
  }

  // failed.
  if (verbose) {
    std::cerr << __FILE__ ": Unable to load image;\ntried";
    for (auto & p : l)
      // 'flush' in case of segfault next time through loop. Else, we
      // will not see those printed tags still in the stream buffer.
      std::cerr << " \'" << p->tag() << "\'" << std::flush;
    std::cerr << std::endl;
  }

  return nullptr;
}

vil_image_resource_sptr vil_load_image_resource_raw(char const* filename,
                                                    bool verbose)
{
  vil_smart_ptr<vil_stream> is = vil_open(filename, "r");
  vil_image_resource_sptr isp = nullptr;
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

vil_image_resource_sptr vil_load_image_resource(char const* filename,
                                                bool verbose)
{
  vil_image_resource_sptr im = vil_load_image_resource_plugin(filename);
  if (!im)
    im=vil_load_image_resource_raw(filename, verbose);
  if (!im && verbose)
    std::cerr << __FILE__ ": Failed to load [" << filename << "]\n";
  return im;
}


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
  return vil_image_resource_sptr(nullptr);
}

vil_pyramid_image_resource_sptr
vil_load_pyramid_resource(char const* directory_or_file, bool verbose)
{
  std::list<vil_file_format*>& l = vil_file_format::all();
  for (auto & p : l) {
#if 0 // debugging
    std::cerr << __FILE__ " : trying \'" << (*p)->tag() << "\'\n";


    std::cerr << "make_input_pyramid_image(" << directory_or_file << ")\n";
#endif
    vil_pyramid_image_resource_sptr pir =
      p->make_input_pyramid_image(directory_or_file);
    if (pir)
      return pir;
  }
  // failed.
  if (verbose) {
    std::cerr << __FILE__ ": Unable to load pyramid image;\ntried";
    for (auto & p : l)
      // 'flush' in case of segfault next time through loop. Else, we
      // will not see those printed tags still in the stream buffer.
      std::cerr << " \'" << p->tag() << "\'" << std::flush;
    std::cerr << std::endl;
  }
  return nullptr;
}

//: Convenience function for loading an image into an image view.
vil_image_view_base_sptr vil_load(const char *file, bool verbose)
{
  vil_image_resource_sptr data = vil_load_image_resource(file, verbose);
  if (!data) return nullptr;
  return data -> get_view();
}


#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//  --------------------------------------------------------------------------------
//  Windows' wchar_t overloading version
//
//
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

vil_image_resource_sptr vil_load_image_resource(wchar_t const* filename, bool verbose)
{
  // do not support image resource plugin for the time being
  //vil_image_resource_sptr im = vil_load_image_resource_plugin(filename);
  //if (!im)
  //  im=vil_load_image_resource_raw(filename);
  vil_image_resource_sptr im = vil_load_image_resource_raw(filename);
  return im;
}

//: Convenience function for loading an image into an image view.
vil_image_view_base_sptr vil_load(const wchar_t *file, bool verbose)
{
  vil_image_resource_sptr data = vil_load_image_resource(file, verbose);
  if (!data) return 0;
  return data -> get_view();
}

#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T
