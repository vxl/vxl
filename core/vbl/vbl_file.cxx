//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vbl_file.h"
#endif
//
// Class: vbl_file
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 02 Nov 98
// Modifications:
//   981102 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vbl_file.h"

#include <sys/stat.h>
#include <stdlib.h>

#ifdef WIN32
#include <direct.h> // for getcwd, mkdir
#else
#include <unistd.h>
#endif

#include <vbl/vbl_user_info.h>

vcl_string vbl_file::get_cwd()
{
  const int BIG = 65536;
  char buf[BIG];
  getcwd(buf,BIG-1);
  return buf;
}

bool vbl_file::make_directory(char const* name)
{
#ifdef WIN32
  return mkdir(name);
#else 
  return -1 != mkdir(name, 0755);
#endif
}

bool vbl_file::is_directory(char const* fn)
{
  struct stat fs;
  return (stat(fn, &fs) == 0) && ((fs.st_mode & S_IFMT) == S_IFDIR);
}

int vbl_file::size(char const* fn)
{
  struct stat fs;
  if (stat(fn, &fs) == 0)
    return fs.st_size;
  else
    return 0;
}

int vbl_file::exists(char const* fn)
{
  struct stat fs;
  return (stat(fn, &fs) == 0);
}

vcl_string vbl_file::dirname(char const* fn)
{
  vcl_string self(fn);

  unsigned int slash_index = self.rfind('/');
  if (slash_index == vcl_string::npos)
    return ".";
  
  return self.substr(0, slash_index);
}

vcl_string vbl_file::expand_tilde(char const* vbl_filename)
{
  if (!vbl_filename || (strlen(vbl_filename) == 0))
    return "";

#ifdef WIN32
  // ~ meaningless on win32
  return vcl_string(vbl_filename);
#else

  if (vbl_filename[0] != '~')
    return vcl_string(vbl_filename);
  
  //// ** Have a tilde, go for it
  
  // 1. Strip to directory only, and remove the tilde itself
  vcl_string fn(vbl_filename);
  vcl_string dir;
  unsigned int first_slash =  fn.find('/');
  if (first_slash != vcl_string::npos) {
    dir = fn.substr(1, first_slash-1);
    fn = fn.substr(first_slash, vcl_string::npos);
  } else {
    dir = fn.substr(1, vcl_string::npos);
    fn = "";
  }
  // Now, from original to  (dir, vbl_filename) is one of
  //  ~            ""     "" 
  //  ~fre         "fre"  ""
  //  ~/fred       ""     "/fred"
  //  ~user/fred   "user" "/fred"
  
  if (dir.size() == 0) {
    // Was just ~, use getenv(HOME)
    char * home_directory = getenv("HOME");
    if (!home_directory) home_directory = "";
    return vcl_string(home_directory) + fn;
  }

  // Was ~user, Check password list for match
  vbl_user_info user(dir);
  if (!user.ok)
    return vcl_string(vbl_filename);
  
  // Got user info
  return user.home_directory + fn;
#endif
}
