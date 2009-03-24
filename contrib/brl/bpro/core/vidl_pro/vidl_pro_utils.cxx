#include "vidl_pro_utils.h"
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vul/vul_file.h>

bool vidl_pro_utils::create_directory(vcl_string const& dir, bool force)
{
  if (vul_file::exists(dir)&&!vul_file::is_directory(dir)){
    vcl_cerr << "In vidl_pro_utils::create_directory() -"
             << " path exists but is not a directory\n" << dir
             << vcl_endl;
    return false;
  }

  if (!vul_file::exists(dir)&&force)
    if (!vul_file::make_directory(dir)){
      vcl_cerr << "In  vidl_pro_utils::create_directory() -"
               << " could not make directory\n" << dir << vcl_endl;
      return false;
    }
  if (!vul_file::exists(dir)){
      vcl_cerr << "In  vidl_pro_utils::create_directory() -"
               << " directory doesn't exist after all attempts\n" << dir << vcl_endl;
      return false;
    }
  return true;
}


vcl_string vidl_pro_utils::image_sequence_path(vcl_string const& dir_path,
                                                vcl_string const& sname,
                                                unsigned index,
                                                vcl_string const& format)
{
  vcl_stringstream str;
  str << vcl_setw(5) << vcl_setfill('0') << index;
  vcl_string path = dir_path + "\\" + sname +str.str();
  vcl_string save_path = path + '.' + format;
  return save_path;
}


