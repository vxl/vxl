#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
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

#include "mvl2_image_format_plugin.h"
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h> // for vcl_atoi()
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
  for (vcl_map<vcl_string,mvl2_video_reader*>::iterator it=mvl2_list_.begin();
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

vcl_string mvl2_image_format_plugin::is_a() const
{
  return vcl_string("mvl2_image_format_plugin");
}

//=======================================================================

bool mvl2_image_format_plugin::get_frame_number_and_filename(
    vcl_string& filename, int& frame_number, const vcl_string& path)
{
  // find extension of the file

  vcl_string extension;
  extension=path.substr(path.length()-4);
  if (extension!=vcl_string(".seq") &&
      extension!=vcl_string(".avi"))
  {
    return false;
  }

  // find the frame number and file name

  int frame_sep_pos;
  frame_sep_pos=path.rfind('_');
  if (frame_sep_pos<1)
  {
    return false;
  }

  vcl_string frame_string=path.substr(frame_sep_pos+1,path.length()-5-frame_sep_pos);

  frame_number=vcl_atoi(frame_string.c_str());
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
    const vcl_string & path, const vcl_string & /*filetype*/,
    const vcl_string & colour)
{
  int frame_number;
  vcl_string filename;
  vcl_string extension;

  if (!get_frame_number_and_filename(filename, frame_number, path))
  {
    return false;
  }

  extension=vul_file::extension(filename.c_str());

  // opens the file and initialise the video

  vcl_map<vcl_string,mvl2_video_reader*>::iterator mvl2_list_iterator;
  mvl2_list_iterator=mvl2_list_.find(filename);
  if (mvl2_list_iterator==mvl2_list_.end())
  {
    // filename not found in the cache
    mvl2_video_reader *video_reader;
    video_reader=0;
    if (extension==vcl_string(".avi"))
        video_reader=new mvl2_video_from_avi();
    if (extension==vcl_string(".seq"))
        video_reader=new mvl2_video_from_sequence();
    if (video_reader==0)
    {
      vcl_cout <<"WARNING : cannot allocate memory for video class.\n";
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
      vcl_cout << "WARNING : unable to initialize avi file.\n";
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

bool mvl2_image_format_plugin::can_be_loaded(const vcl_string& filename)
{
  vcl_string real_filename;
  int frame_number;

  return get_frame_number_and_filename(real_filename,frame_number,filename);
}

