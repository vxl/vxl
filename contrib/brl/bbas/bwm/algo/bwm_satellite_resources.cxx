#include "bwm_satellite_resources.h"

#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>

void add_directories(vcl_string root, vcl_vector<vcl_string>& directories) {
  if (vul_file::is_directory(root))
    directories.push_back(root);
  
  vcl_string glob = root + "/*"; // get everything directory or not
  vul_file_iterator file_it(glob.c_str());
  while (file_it) {
    vcl_string name(file_it());
    if (vul_file::is_directory(name))
      add_directories(name, directories);
    ++file_it;
  }

}

bwm_satellite_resources::bwm_satellite_resources(vcl_string path, 
                                                 double lower_left_lat, 
                                                 double lower_left_lon, 
                                                 double upper_right_lat, 
                                                 double upper_right_lon)
{
  vcl_vector<vcl_string> directories;
  add_directories(path, directories);
  if (!directories.size())
    return;
  vcl_cout << "found " << directories.size() << " directories!\n";  
}

