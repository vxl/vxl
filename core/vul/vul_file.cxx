// This is core/vul/vul_file.cxx
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   02 Nov 1998
//
// \verbatim
//  Modifications
//   08 Jan 2009  Peter Vanroose- simplified "delete_file_glob()" implementation
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <cstring>
#include <cctype>
#include <cstdlib>
#include "vul_file.h"

#include <sys/stat.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h> // for getcwd, mkdir
#else
#include <unistd.h>
#endif

#include <vul/vul_user_info.h>

std::string vul_file::get_cwd()
{
  constexpr int BIG = 65536;
  char buf[BIG];
  if (getcwd(buf,BIG-1))
    return buf;
  else
    return "Error returned by getcwd";
}

bool vul_file::change_directory(char const* dirname)
{
  return 0 == chdir(dirname);
}

bool vul_file::make_directory(char const* name)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return -1 != mkdir(name);
#else
  return -1 != mkdir(name, 0755);
#endif
}

bool vul_file::is_directory(char const* fn)
{
  struct stat fs;
  return stat(fn, &fs) == 0
      && (fs.st_mode & S_IFMT) == S_IFDIR;
}

std::time_t vul_file::time_modified(char const* filename){
  struct stat fs;
  if(stat(filename, &fs)!=0)
    return std::time_t(0);
  return fs.st_mtime;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
bool vul_file::is_drive(char const* fn)
{
  // a drive string looks like "c:", "z:"
  return fn
      && std::isalpha(fn[0])
      && fn[1]==':'
      && fn[2]=='\0';
}
#endif

//: Make a writable directory, including any necessary parents.
// Returns true if successful, or if the directory already exists.
// Implemented by calling itself recursively on the parent directory.
bool vul_file::make_directory_path(char const* filename)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  if (is_directory(filename) || is_drive(filename)) return true;
#else
  if (is_directory(filename)) return true;
#endif
  // You can get a race condition here if there are multiple
  // process/threads trying to make the same dir at the same time
  // So if the make_directory fails, just check that someone else
  // didn't make it in the intervening time.
  return make_directory_path(dirname(filename))
      && (make_directory(filename) || is_directory(filename));
}


unsigned long vul_file::size(char const* fn)
{
  struct stat fs;
  if (stat(fn, &fs) == 0)
    return fs.st_size;
  else
    return 0L;
}

bool vul_file::exists(char const* fn)
{
  struct stat fs;
  std::string name(fn);

#if defined(_WIN32) && !defined(__CYGWIN__)
  std::string::size_type last_non_slash_index = name.find_last_not_of("\\/");
#else
  std::string::size_type last_non_slash_index = name.find_last_not_of('/');
#endif
  if (last_non_slash_index != std::string::npos)
    last_non_slash_index++;
  name = name.substr(0, last_non_slash_index);
  return stat(name.c_str(), &fs) == 0;
}

std::string vul_file::dirname(char const* fn)
{
  std::string self(fn);

#if defined(_WIN32) && !defined(__CYGWIN__)
  std::string::size_type slash_index = self.find_last_of("\\/");
#else
  std::string::size_type slash_index = self.rfind('/');
#endif
  if (slash_index == std::string::npos)
    return ".";


  return self.substr(0, slash_index);
}

std::string vul_file::extension(char const* fn)
{
  std::string self(fn);

  std::string::size_type dot_index = self.rfind('.');
  if (dot_index != std::string::npos)
    return self.substr(dot_index, std::string::npos);
  else
    return std::string();
}

std::string vul_file::strip_directory(char const* fn)
{
  std::string self(fn);

#if defined(_WIN32) && !defined(__CYGWIN__)
  std::string::size_type slash_index = self.find_last_of("\\/");
#else
  std::string::size_type slash_index = self.rfind('/');
#endif
  if (slash_index != std::string::npos)
    self.erase(0, slash_index+1);

  return self;
}

std::string vul_file::strip_extension(char const* fn)
{
  std::string self(fn);

  std::string::size_type dot_index = self.rfind('.');
  if (dot_index != std::string::npos)
    self.erase(dot_index, std::string::npos);

  return self;
}

std::string vul_file::basename(char const* fn, char const * suffix)
{
  // First strip dir
  std::string self(fn);

#if defined(_WIN32) && !defined(__CYGWIN__)
  std::string::size_type slash_index = self.find_last_of("\\/");
#else
  std::string::size_type slash_index = self.rfind('/');
#endif

  if (slash_index != std::string::npos)
    self.erase(0, slash_index+1);

  // Now strip suffix if any
  if (suffix) {
    int start = (int)(self.size() - std::strlen(suffix));
    if (start >= 0)
      if (std::string(self.begin()+start, self.end()) == suffix)
        self.erase(start, std::string::npos);
  }
  return self;
}


#if defined(_WIN32) && !defined(__CYGWIN__)
//: replace instances of 'from' in 's' with 'to'
static unsigned replace(char from, char to, std::string &s)
{
  unsigned c = 0;
  for (unsigned i=0; i<s.size(); ++i)
    if (s[i] == from)
    {
      c++;
      s[i] = to;
    }
    return c;
}
#endif

//: Delete 1 or more files using the Local OS preferred globbing.
// E.g. \c delete_file_glob("*"); will delete all the files in the
// current directory on most operating systems.
// Takes Posix path separators i.e. '/'
bool vul_file::delete_file_glob(std::string const& file_glob)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  std::string command = file_glob;
  replace('/', '\\', command);
  command = "del /Q " + command;
#else
  std::string command = "/bin/rm -f " + file_glob;
#endif
  return std::system(command.c_str())==0;
}


std::string vul_file::expand_tilde(char const* vul_filename)
{
  if (!vul_filename || (std::strlen(vul_filename) == 0))
    return "";

#if defined(_WIN32) && !defined(__CYGWIN__)
  // ~ meaningless on win32
  return std::string(vul_filename);
#else

  if (vul_filename[0] != '~')
    return std::string(vul_filename);

  //// ** Have a tilde, go for it

  // 1. Strip to directory only, and remove the tilde itself
  std::string fn(vul_filename);
  std::string dir;
  std::string::size_type first_slash =  fn.find('/');
  if (first_slash != std::string::npos) {
    dir = fn.substr(1, first_slash-1);
    fn = fn.substr(first_slash, std::string::npos);
  }
  else {
    dir = fn.substr(1, std::string::npos);
    fn = "";
  }
  // Now, from original to  (dir, vul_filename) is one of
  //  ~            ""     ""
  //  ~fre         "fre"  ""
  //  ~/fred       ""     "/fred"
  //  ~user/fred   "user" "/fred"

  if (dir.size() == 0) {
    // Was just ~, use getenv(HOME)
    char const * home_directory = getenv("HOME");
    if (!home_directory) home_directory = "";
    return std::string(home_directory) + fn;
  }

  // Was ~user, Check password list for match
  vul_user_info user(dir);
  if (!user.ok)
    return std::string(vul_filename);

  // Got user info
  return user.home_directory + fn;
#endif
}


#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
#include <cwchar>

std::wstring
vul_file::get_cwd(wchar_t* /*dummy*/)
{
  constexpr int BIG = 65536;
  wchar_t buf[BIG];
  buf[0] = L'\0';
  _wgetcwd(buf,BIG-1);
  return buf;
}

bool vul_file::change_directory(wchar_t const* dirname)
{
  return 0 == _wchdir(dirname);
}

bool vul_file::make_directory(wchar_t const* name)
{
  return -1 != _wmkdir(name);
}

bool vul_file::is_directory(wchar_t const* fn)
{
  struct _stat fs;
  return _wstat(fn, &fs) == 0
      && (fs.st_mode & S_IFMT) == S_IFDIR;
}

bool vul_file::is_drive(wchar_t const* fn)
{
  // a drive string looks like "c:", "z:"
  return fn
      && iswalpha(fn[0])
      && fn[1]==L':'
      && fn[2]==L'\0';
}

//: Make a writable directory, including any necessary parents.
// Returns true if successful, or if the directory already exists.
bool vul_file::make_directory_path(wchar_t const* filename)
{
  if (is_directory(filename) || is_drive(filename)) return true;

  return make_directory_path(dirname(filename))
      && make_directory(filename);
}

bool vul_file::exists(wchar_t const* fn)
{
  struct _stat fs;
  return _wstat(fn, &fs) == 0;
}

std::wstring vul_file::dirname(wchar_t const* fn)
{
  std::wstring self(fn);

  std::wstring::size_type slash_index = self.find_last_of(L"\\/");
  if (slash_index == std::wstring::npos)
    return L".";

  return self.substr(0, slash_index);
}

std::wstring vul_file::extension(wchar_t const* fn)
{
  std::wstring self(fn);

  std::wstring::size_type dot_index = self.rfind(L'.');
  if (dot_index != std::wstring::npos)
    return self.substr(dot_index, std::wstring::npos);
  else
    return std::wstring();
}

std::wstring vul_file::strip_directory(wchar_t const* fn)
{
  std::wstring self(fn);

  std::wstring::size_type slash_index = self.find_last_of(L"\\/");
  if (slash_index != std::wstring::npos)
    self.erase(0, slash_index+1);

  return self;
}

std::wstring vul_file::strip_extension(wchar_t const* fn)
{
  std::wstring self(fn);

  std::wstring::size_type dot_index = self.rfind(L'.');
  if (dot_index != std::wstring::npos)
    self.erase(dot_index, std::wstring::npos);

  return self;
}

std::wstring vul_file::basename(wchar_t const* fn, wchar_t const * suffix)
{
  // First strip dir
  std::wstring self(fn);

  std::wstring::size_type slash_index = self.find_last_of(L"\\/");

  if (slash_index != std::wstring::npos)
    self.erase(0, slash_index+1);

  // Now strip suffix if any
  if (suffix) {
    int start = (int)self.size() - (int)wcslen(suffix);
    if (start >= 0)
      if (std::wstring(self.begin()+start, self.end()) == suffix)
        self.erase(start, std::wstring::npos);
  }
  return self;
}

#endif
