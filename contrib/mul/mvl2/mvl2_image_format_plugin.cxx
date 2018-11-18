//:
// \file
// \brief Interface for loading avi frames as image formats
// \author Franck Bettinger
// \date   Mon Mar 18 06:57:49 2002
// This class implements the plugin to load avi frames as a new formats
//
// \verbatim
//  Modifications
// \endverbatim

#include <iostream>
#include <string>
#include <cstdlib>
#include "mvl2_image_format_plugin.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <mvl2/mvl2_video_from_avi.h>
#include <mvl2/mvl2_video_from_sequence.h>
#include <vil/vil_copy.h>
#include <vil/vil_image_view.h>

//=======================================================================

mvl2_image_format_plugin::mvl2_image_format_plugin()
{
}

//=======================================================================

mvl2_image_format_plugin::~mvl2_image_format_plugin()
{
  for (std::map<std::string,mvl2_video_reader*>::iterator it=mvl2_list_.begin();
       it!=mvl2_list_.end(); ++it)
  {
    if ((*it).second!=0)
    {
      (*it).second->uninitialize();
      delete (*it).second;
    }
    (*it).second = 0;
    mvl2_list_.erase(it);
  }
}

//=======================================================================

std::string mvl2_image_format_plugin::is_a() const
{
  return std::string("mvl2_image_format_plugin");
}

//=======================================================================

bool mvl2_image_format_plugin::get_frame_number_and_filename(
    std::string& filename, int& frame_number, const std::string& path)
{
  // find extension of the file

  std::string extension;
  extension=path.substr(path.length()-4);
  if (extension!=std::string(".seq") &&
      extension!=std::string(".avi"))
  {
    return false;
  }

  // find the frame number and file name

  std::size_t frame_sep_pos=path.rfind('_');
  if (frame_sep_pos<1)
  {
    return false;
  }

  std::string frame_string=path.substr(frame_sep_pos+1,path.length()-5-frame_sep_pos);

  frame_number=std::atoi(frame_string.c_str());
  filename=path.substr(0,frame_sep_pos)+extension;

  if (!vul_file::exists(filename))
  {
    return false;
  }

  return true;
}

//=======================================================================

bool mvl2_image_format_plugin::load_the_image (
    vil_image_view_base_sptr& image,
    const std::string & path, const std::string & /*filetype*/,
    const std::string & colour)
{
  int frame_number;
  std::string filename;
  std::string extension;

  if (!get_frame_number_and_filename(filename, frame_number, path))
  {
    return false;
  }

  extension=vul_file::extension(filename.c_str());

  // opens the file and initialise the video

  std::map<std::string,mvl2_video_reader*>::iterator mvl2_list_iterator;
  mvl2_list_iterator=mvl2_list_.find(filename);
  if (mvl2_list_iterator==mvl2_list_.end())
  {
    // filename not found in the cache
    mvl2_video_reader *video_reader;
    video_reader=0;
    if (extension==std::string(".avi"))
        video_reader=new mvl2_video_from_avi();
    if (extension==std::string(".seq"))
        video_reader=new mvl2_video_from_sequence();
    if (video_reader==0)
    {
      std::cout <<"WARNING : cannot allocate memory for video class.\n";
      return false;
    }
    if (!image)
    {
      return false;
    }
    if (width_==-1)
    {
      width_=image->ni();
    }
    if (height_==-1)
    {
      height_=image->nj();
    }
    if (!video_reader->initialize(width_,height_,colour,filename))
    {
      std::cout << "WARNING : unable to initialize avi file.\n";
      return false;
    }
    mvl2_list_[filename]=video_reader;
    mvl2_list_iterator=mvl2_list_.find(filename);
    if (mvl2_list_iterator==mvl2_list_.end())
    {
      return false;
    }
  }

  (*mvl2_list_iterator).second->seek(frame_number);
  vil_image_view<vxl_byte> the_image;
  bool ok=(*mvl2_list_iterator).second->get_frame(the_image);

  image->set_size((*mvl2_list_iterator).second->get_width(),
                  (*mvl2_list_iterator).second->get_height(),
                  the_image.nplanes());
  vil_copy_deep(the_image,(vil_image_view<vxl_byte>&)*image);

  return ok;
}

//=======================================================================

bool mvl2_image_format_plugin::can_be_loaded(const std::string& filename)
{
  std::string real_filename;
  int frame_number;

  return get_frame_number_and_filename(real_filename,frame_number,filename);
}
