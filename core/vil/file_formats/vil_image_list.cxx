#include <vil/file_formats/vil_image_list.h>
#include <sys/stat.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>

#define IL_DEBUG

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
vcl_vector<vil_image_resource_sptr> vil_image_list::resources()
{
  vcl_vector<vil_image_resource_sptr> temp;
  if(!this->vil_is_directory(directory_.c_str()))
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
  if(handle<0)
    return temp;
  vcl_string s = data.name;
  vcl_string filename = directory_+ "\\" + s;
  vil_image_resource_sptr resc;
  if(s != "."&&s!="..")
    {
       resc = vil_load_image_resource(filename.c_str());
      if(resc)
        temp.push_back(resc);
    }
  while ( true )
    {
      if (_findnext(handle, &data) != 0) {
      _findclose(handle);
        return temp;
      }
      s = data.name;
      if(s != "."&&s!="..")
        {
          filename = directory_+ "\\" + s;
          resc = vil_load_image_resource(filename.c_str());
          if(resc)
            temp.push_back(resc);
        }
    }
  return temp;
}
#else // !defined(VCL_WIN32) || defined(__CYGWIN__)

#include <dirent.h>
vcl_vector<vil_image_resource_sptr> vil_image_list::resources()
{
  vcl_vector<vil_image_resource_sptr> temp;
  if(!this->vil_is_directory(directory_.c_str()))
    return temp;
  DIR* dir_handle = opendir(directory_.c_str());
  dirent* de;
  de = readdir(dir_handle);
  if(de==0)
    return temp;
  vcl_string s = de->d_name;
  vcl_string filename = directory_+ "\/" + s;
  vil_image_resource_sptr resc;
  if(s != "."&&s!="..")
    {
#ifdef IL_DEBUG
      vcl_cout << "Found File(0) " << filename << '\n';
#endif
      resc = vil_load_image_resource(filename.c_str());
      if(resc)
        temp.push_back(resc);
    }
  while ( true )
    {
      de = readdir(dir_handle);
      if(de == 0){
        closedir(dir_handle);
        return temp;
      }
      s = de->d_name;
      filename = directory_+ "\/" + s;
      if(s != "."&&s!="..")
        {
#ifdef IL_DEBUG
          vcl_cout << "Found File " << filename << '\n';
#endif
          resc = vil_load_image_resource(filename.c_str());
          if(resc)
            temp.push_back(resc);
        }
    }
  return temp;
}
#endif // !defined(VCL_WIN32) || defined(__CYGWIN__)

