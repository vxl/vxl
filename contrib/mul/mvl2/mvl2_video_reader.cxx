#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief A base class for cameras/virtual cameras
// \author Louise Butcher

#include "mvl2_video_reader.h"
#include <vcl_cstdlib.h>

mvl2_video_reader::mvl2_video_reader()
{
}

mvl2_video_reader::~mvl2_video_reader()
{
  if (is_initialized()) uninitialize();
}

vcl_string mvl2_video_reader::is_a() const
{
  return vcl_string("mvl2_video_reader");
}

vcl_vector<vcl_string> mvl2_video_reader::load_configs(vcl_string filename)
{
  config_names_.clear();
  config_sizes_.clear();
  config_strings_.clear();
  config_filenames_.clear();

  vcl_string config_name("Default");
  vcl_pair<int,int> config_size(320,240);
  vcl_string config_string("");
  vcl_string config_filename("");

  config_names_.push_back(config_name);
  config_sizes_.push_back(config_size);
  config_strings_.push_back(config_string);
  config_filenames_.push_back(config_filename);

  vcl_ifstream* config_file=new vcl_ifstream(filename.c_str());
  if (!(*config_file))
  {
    const char* val;
    if ((val=vcl_getenv("VIDL2RC"))==0)
    {
      vcl_cerr << "VIDL2RC environment variable not defined.\n"
               << "Cannot find configuration file for video input.\n";
      return config_names_;
    }
    config_file=new vcl_ifstream(val);
    if (!(*config_file))
    {
      vcl_cerr << "Cannot find configuration file for video input.\n";
      return config_names_;
    }
  }

  while (!config_file->eof())
  {
    config_name=vcl_string("");

    *config_file >> config_name >> config_size.first >> config_size.second;
    *config_file >> config_string >> config_filename;

    if (config_name!=vcl_string(""))
    {
      config_names_.push_back(config_name);
      config_sizes_.push_back(config_size);
      config_strings_.push_back(config_string);
      config_filenames_.push_back(config_filename);
    }
  }

  return config_names_;
}

void mvl2_video_reader::display_configs()
{
  vcl_cout << vcl_endl
           << "Video configurations :\n"
           << "======================\n";
  for (unsigned int i=0; i<config_names_.size(); ++i)
  {
    vcl_cout << "Configuration " << i<< vcl_endl
             << "----------------\n"
             << "name = " <<   config_names_[i] << vcl_endl
             << "size = " <<   config_sizes_[i].first << 'x'
             << config_sizes_[i].second << vcl_endl
             << "options = " <<   config_strings_[i] << vcl_endl
             << "filename = " <<   config_filenames_[i] << vcl_endl
             << vcl_endl;
  }
}

bool mvl2_video_reader::use_config(vcl_string configname)
{
  unsigned int position=vcl_find(config_names_.begin(), config_names_.end(), configname) - config_names_.begin();
  if (position<config_names_.size())
  {
    return initialize(config_sizes_[position].first,
                      config_sizes_[position].second,
                      config_strings_[position],config_filenames_[position]);
  }
  else
    return false;
}

int mvl2_video_reader::length()
{
  return -1;
}
