//:
// \file
// \brief A class for reading video files on windows platform
// \author Louise Butcher

#include "mvl2_video_from_sequence.h"
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>

mvl2_video_from_sequence::mvl2_video_from_sequence()
{
  use_seq_file_=false;
  is_initialized_=false;
  current_frame_=0;
  frame_rate_=25;
  width_=0;
  height_=0;
}

mvl2_video_from_sequence::~mvl2_video_from_sequence()
{
}

vcl_string mvl2_video_from_sequence::is_a() const
{
  return vcl_string("mvl2_video_from_sequence");
}

mvl2_video_reader* mvl2_video_from_sequence::clone() const
{
  return new mvl2_video_from_sequence(*this);
}

// possible options : Grey
bool mvl2_video_from_sequence::initialize( int /* width */, int /* height */,
                                           vcl_string format, vcl_string file_name)
{
  use_colour_=true;
  if (!format.find(vcl_string("Grey"))) use_colour_=false;
  if (!vul_file::exists(file_name) || vul_file::is_directory(file_name))
  {
    vcl_cerr<<"File "<<file_name<<" does not exist\n";
    is_initialized_=false;
    return false;
  }

  if (vul_file::extension(file_name.c_str())==vcl_string(".seq"))
  {
    char buffer[201];
    vcl_ifstream ifile(file_name.c_str());
    use_seq_file_=true;

    while (!ifile.eof())
    {
      ifile.getline(buffer,200);
      vcl_string filename(buffer);
      if (filename.length()>0) list_files_.push_back(filename);
    }
    current_frame_=0;
    is_initialized_=true;
  }

  //knock off the extension
  int name_length=file_name.length();
  int dot_pos = file_name.find_last_of(".");
  file_name.erase(dot_pos, name_length);

  //Extract the largest possible number off the end

  no_digits_=0;
  offset_=0;
  name_length=file_name.length();
  for (int i=1;i<9;++i)
  {
    vcl_string last_i;
    last_i=file_name.substr(name_length-i,i);
    int curr_no=atoi(last_i.c_str());
    if (curr_no>0)
    {
      no_digits_=i;
      offset_=curr_no;
      file_stem_=file_name.substr(0,name_length-i);
    }
    else break;
  }

  current_frame_=0;
  is_initialized_=true;
  vil_image_view<vxl_byte> loc_img;
  get_frame(loc_img);
  height_=loc_img.nj();
  width_=loc_img.ni();
  return true;
}

void mvl2_video_from_sequence::uninitialize()
{
}

int mvl2_video_from_sequence::next_frame()
{
  if (!is_initialized_) return -1;

  return ++current_frame_;
}

bool mvl2_video_from_sequence::get_frame(vil_image_view<vxl_byte>& image)
{
  vcl_string curr_file;

  if (use_seq_file_)
  {
    if (current_frame_<list_files_.size())
    {
      curr_file=list_files_[current_frame_];
    }
    else
    {
      return false;
    }
  }
  else
  {
    int currno=current_frame_+offset_;
    curr_file=vul_sprintf("%s%d.jpg",file_stem_.c_str(),currno);
  }

  vil_image_view_base_sptr image_view_sptr;
  if (!(image_view_sptr=vil_load(curr_file.c_str())))
  {
    vcl_cout<<"Unable to load : " << curr_file<<vcl_endl;
    return false;
  }
  if (use_colour_)
  {
    image.deep_copy(image_view_sptr);
  }
  else
  {
    image.deep_copy(vil_convert_to_grey_using_rgb_weighting(image_view_sptr));
  }
  height_=image.ni();
  width_=image.nj();
  return true;
}

void mvl2_video_from_sequence::reset_frame()
{
  current_frame_=0;
}

void mvl2_video_from_sequence::set_frame_rate(double /*frame_rate*/)
{
  vcl_cerr << "mvl2_video_from_sequence::set_frame_rate() NYI\n";
}

double mvl2_video_from_sequence::get_frame_rate()
{
  return frame_rate_;
}

int mvl2_video_from_sequence::get_width()
{
  return width_;
}

int mvl2_video_from_sequence::get_height()
{
  return height_;
}

void mvl2_video_from_sequence::set_capture_size(int /*width*/,int /*height*/)
{
  vcl_cerr << "mvl2_video_from_sequence::set_capture_size() NYI\n";
}

int mvl2_video_from_sequence::length()
{
  if (!is_initialized_) return -1;
  return list_files_.size();
}

int mvl2_video_from_sequence::seek(int frame_number)
{
  assert (frame_number >= 0);
  current_frame_=frame_number;
  return current_frame_;
}
