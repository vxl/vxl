// This is core/vidl/vidl_v4l2_devices.cxx
#include <cstdio>
#include <cstring>
#include <iostream>
#include "vidl_v4l2_devices.h"
//:
// \file
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//   15 Apr 2008 Created (A. Garrido)
//\endverbatim

extern "C" { // revisar los .h

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
};

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vidl_pixel_format.h"


// ----------------- local functions ---------------
namespace
{
  inline bool is_directory(const char *dir)
  {
    struct stat s;
    return lstat(dir, &s) == 0 && S_ISDIR(s.st_mode);
  }
  inline bool is_video_device(const char *file)
  {
    struct stat s;
    bool isvd=(lstat(file, &s) == 0 &&
               S_ISCHR(s.st_mode) && // is character device
               ((int)((unsigned short)(s.st_rdev)>> 8) == 81) && // major number 81
               ((int)((unsigned short)(s.st_rdev) & 0xC0) ==0)   // minor in [0,63]
           );
#if 0
    std::cout << file << ": ";
    if (lstat(file, &s) == 0) std::cout << "lstat ok, ";
    if (S_ISCHR(s.st_mode)) std::cout << "ISCHR ok, ";
    std::cout <<  "rdev:" << (s.st_rdev)<< "Major: "<< ((unsigned short)(s.st_rdev)>> 8);
    if ((int)((unsigned short)(s.st_rdev)>> 8) == 81) std::cout <<" major number 81, ";
    if ((int)((unsigned short)(s.st_rdev) & 0xFF) >=0) std::cout << " minor >0, ";
    if ((int)((unsigned short)(s.st_rdev) & 0xFF) <=63) std::cout << " minor <64.";
    if (isvd) std::cout << " Is video device"; else std::cout << " discarded";
    std::cout << std::endl;
#endif // 0
    return isvd;
  }
}
// --------------- end local functions --------------------


vidl_v4l2_devices& vidl_v4l2_devices::all()
{
  static vidl_v4l2_devices instance;
  return instance;
}


void vidl_v4l2_devices::load_devices(const char *dirname)
{
  //std::cerr << "Directory: " << dirname << std::endl;
  DIR *dp;
  struct dirent *ep;
  dp = opendir(dirname);
  char filename[200];
  if (dp != NULL)
  {
    while ((ep = readdir(dp))) {
      std::strcpy(filename,dirname);
      std::strcat(filename,"/");
      std::strcat(filename,ep->d_name);
      if (is_directory(filename) && ep->d_name[0]!='.')
        load_devices(filename);
      else  if (is_video_device(filename)) {
        //std::cerr<< filename << "  is video device\n";
        vidl_v4l2_device_sptr aux= new vidl_v4l2_device(filename);
        //vecdev.push_back(aux);
        if (aux->n_inputs()>0) vecdev.push_back(aux);
        else std::cerr << "No inputs in device " << filename << std::endl;
      }
      //else puts("  is not video device");
    }
    closedir(dp);
  }
  else
    std::perror("Couldn't open the directory");
}


vidl_v4l2_devices::vidl_v4l2_devices()
{
  // /sys/class/video4linux kernel 2.6?
  // /proc/video/dev kernel 2.4?
  // /dev/video0 to /dev/video63 (conventionally) v4l2 specification
  const char *dir;
  //if (!is_directory(dir="/sys/class/video4linux"))// kernel 2.6
  //if (!is_directory(dir="/proc/video/dev")) // kernel 2.4
  if (!is_directory(dir= "/dev")) // v4l2 specification
    return;

  load_devices(dir);
}
