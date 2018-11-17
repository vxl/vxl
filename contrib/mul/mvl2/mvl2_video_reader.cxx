//:
// \file
// \brief A base class for cameras/virtual cameras
// \author Louise Butcher

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "mvl2_video_reader.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

mvl2_video_reader::mvl2_video_reader() = default;

mvl2_video_reader::~mvl2_video_reader()
{
  if (is_initialized()) uninitialize();
}

std::string mvl2_video_reader::is_a() const
{
  return std::string("mvl2_video_reader");
}

std::vector<std::string> mvl2_video_reader::load_configs(const std::string& filename)
{
  config_names_.clear();
  config_sizes_.clear();
  config_strings_.clear();
  config_filenames_.clear();

  std::string config_name("Default");
  std::pair<int,int> config_size(320,240);
  std::string config_string("");
  std::string config_filename("");

  config_names_.push_back(config_name);
  config_sizes_.push_back(config_size);
  config_strings_.push_back(config_string);
  config_filenames_.push_back(config_filename);

  auto* config_file=new std::ifstream(filename.c_str());
  if (!(*config_file))
  {
    const char* val;
    if ((val=std::getenv("VIDL2RC"))==nullptr)
    {
      std::cerr << "VIDL2RC environment variable not defined.\n"
               << "Cannot find configuration file for video input.\n";
      return config_names_;
    }
    config_file=new std::ifstream(val);
    if (!(*config_file))
    {
      std::cerr << "Cannot find configuration file for video input.\n";
      return config_names_;
    }
  }

  while (!config_file->eof())
  {
    config_name=std::string("");

    *config_file >> config_name >> config_size.first >> config_size.second;
    *config_file >> config_string >> config_filename;

    if (config_name!=std::string(""))
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
  std::cout << std::endl
           << "Video configurations :\n"
           << "======================\n";
  for (unsigned int i=0; i<config_names_.size(); ++i)
  {
    std::cout << "Configuration " << i<< std::endl
             << "----------------\n"
             << "name = " <<   config_names_[i] << std::endl
             << "size = " <<   config_sizes_[i].first << 'x'
             << config_sizes_[i].second << std::endl
             << "options = " <<   config_strings_[i] << std::endl
             << "filename = " <<   config_filenames_[i] << std::endl
             << std::endl;
  }
}

bool mvl2_video_reader::use_config(const std::string& configname)
{
  std::size_t position=std::find(config_names_.begin(), config_names_.end(), configname) - config_names_.begin();
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
