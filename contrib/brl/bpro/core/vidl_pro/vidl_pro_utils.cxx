#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "vidl_pro_utils.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

bool vidl_pro_utils::create_directory(std::string const& dir, bool force)
{
  if (vul_file::exists(dir)&&!vul_file::is_directory(dir)){
    std::cerr << "In vidl_pro_utils::create_directory() -"
             << " path exists but is not a directory\n" << dir
             << std::endl;
    return false;
  }

  if (!vul_file::exists(dir)&&force)
    if (!vul_file::make_directory(dir)){
      std::cerr << "In  vidl_pro_utils::create_directory() -"
               << " could not make directory\n" << dir << std::endl;
      return false;
    }
  if (!vul_file::exists(dir)){
      std::cerr << "In  vidl_pro_utils::create_directory() -"
               << " directory doesn't exist after all attempts\n" << dir << std::endl;
      return false;
    }
  return true;
}


std::string vidl_pro_utils::image_sequence_path(std::string const& dir_path,
                                                std::string const& sname,
                                                unsigned index,
                                                std::string const& format)
{
  std::stringstream str;
  str << std::setw(5) << std::setfill('0') << index;
  std::string path = dir_path + "\\" + sname +str.str();
  std::string save_path = path + '.' + format;
  return save_path;
}
