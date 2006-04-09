#include <vil/vil_image_list.h>
#include <sys/stat.h>
#include <vcl_cstdlib.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_load.h>

//#define IL_DEBUG
#if defined(como4301) && defined(__linux__)
# ifndef S_IFMT
#  define S_IFMT 0170000
# endif
# ifndef S_IFDIR
#  define S_IFDIR 0040000
# endif
#endif

bool vil_image_list::vil_is_directory(char const* fn)
{
  struct stat fs;
  return stat(fn, &fs) == 0 && (fs.st_mode & S_IFMT) == S_IFDIR;
}

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
#if defined(VCL_BORLAND_56)
# include <stdint.h> /* for intptr_t on Borland 5.6. */
#endif
#include <io.h>
vcl_vector<vcl_string> vil_image_list::files()
{
  vcl_vector<vcl_string> temp;
  if (!this->vil_is_directory(directory_.c_str()))
    return temp;
  //This mess should go away soon.
# if defined VCL_VC_6 || defined VCL_VC_5 || defined VCL_BORLAND_55 || defined __MINGW32__
  typedef long handle_type;      // works with msvc6
# else
  typedef intptr_t handle_type;  // not found by msvc6
#endif

  handle_type handle;
  struct _finddata_t data;
  handle = _findfirst((directory_+"\\*").c_str(), &data);
  if (handle<0)
    return temp;
  vcl_string s = data.name;
  vcl_string filename = directory_+ "\\" + s;
  vil_image_resource_sptr resc;
  if(!this->vil_is_directory(filename.c_str()))
    temp.push_back(filename);
  while ( true )
    {
      if (_findnext(handle, &data) != 0) {
        _findclose(handle);
        return temp;
      }
      s = data.name;
      filename = directory_+ "\\" + s;
      if (!this->vil_is_directory(filename.c_str()))
        temp.push_back(filename);
    }
  
  return temp;
}
#else // !defined(VCL_WIN32) || defined(__CYGWIN__)

#include <dirent.h>
vcl_vector<vcl_string> vil_image_list::files()
{
  vcl_vector<vcl_string> temp;
  if (!this->vil_is_directory(directory_.c_str()))
    return temp;
  DIR* dir_handle = opendir(directory_.c_str());
  dirent* de;
  de = readdir(dir_handle);
  if (de==0)
    return temp;
  vcl_string s = de->d_name;
  vcl_string filename = directory_+ "/" + s;
  if (!this->vil_is_directory(filename.c_str()))
    {
#ifdef IL_DEBUG
      vcl_cout << "Found File(0) " << filename << '\n';
#endif
      temp.push_back(filename);
    }
  while ( true )
    {
      de = readdir(dir_handle);
      if (de == 0){
        closedir(dir_handle);
        return temp;
      }
      s = de->d_name;
      filename = directory_+ "/" + s;
      if (!this->vil_is_directory(filename.c_str()))
        {
#ifdef IL_DEBUG
          vcl_cout << "Found File " << filename << '\n';
#endif
          temp.push_back(filename);
        }
    }
  return temp;
}

#endif // !defined(VCL_WIN32) || defined(__CYGWIN__)
vcl_vector<vil_image_resource_sptr> vil_image_list::resources()
{
  vcl_vector<vil_image_resource_sptr>  temp;
  vcl_vector<vcl_string> fs = this->files();
  for(vcl_vector<vcl_string>::iterator fit = fs.begin();
      fit != fs.end(); ++fit)
    {
      vil_image_resource_sptr resc = vil_load_image_resource((*fit).c_str());
      if(resc)
        temp.push_back(resc);
    }
  return temp;
}

vcl_vector<vil_image_resource_sptr> vil_image_list::blocked_resources()
{
  vcl_vector<vil_image_resource_sptr>  temp;
  vcl_vector<vcl_string> fs = this->files();
  for(vcl_vector<vcl_string>::iterator fit = fs.begin();
      fit != fs.end(); ++fit)
    {
      vil_image_resource_sptr resc = vil_load_image_resource((*fit).c_str());
      vil_image_resource_sptr bir = blocked_image_resource(resc).ptr();
      if(bir)
        temp.push_back(bir);
    }
  return temp;
}

vcl_vector<vil_image_resource_sptr> vil_image_list::pyramids()
{
  vcl_vector<vil_image_resource_sptr>  temp;
  vcl_vector<vcl_string> fs = this->files();
  for(vcl_vector<vcl_string>::iterator fit = fs.begin();
      fit != fs.end(); ++fit)
    {
      vil_pyramid_image_resource_sptr pyr = 
        vil_load_pyramid_resource((*fit).c_str());
      if(pyr)
        temp.push_back(pyr.ptr());
    }
  return temp;
}
//:remove a file
bool vil_image_list::remove_file(vcl_string& filename)
{
#if defined(VCL_WIN32) && !defined(__CYGWIN__)
  vcl_string command = "del " + filename;
#else
  vcl_string command = "rm " + filename;
#endif
  return vcl_system(command.c_str())==0;
}

//:removes all files from the directory. sub-directories are not touched
bool vil_image_list::clean_directory()
{
  vcl_vector<vcl_string> files = this->files();
  bool good = true;
  for(vcl_vector<vcl_string>::iterator fit = files.begin();
      fit != files.end(); ++fit)
    if(!this->remove_file(*fit))
      good = false;
  return good;
}
