// This is core/vul/vul_user_info.cxx
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   08 Apr 99
//
//-----------------------------------------------------------------------------

#include <iostream>
#include "vul_user_info.h"


#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//  uid_type uid;
//  uid_type gid;
//  std::string name; // login name
//  std::string home_directory;
//  std::string full_name;
//  std::string shell;
//  std::string passwd;

void vul_user_info::init(uid_type /*uid*/)
{
  std::cerr << "Not implemented\n";
}

void vul_user_info::init(char const* name_)
{
#if !defined(_WIN32) || defined(__CYGWIN__)
  struct passwd* pw = getpwnam(name_);
  if (!pw) {
#endif
    this->ok = false;
    this->uid = 0;
    this->gid = 0;
    this->name = name_;
#if !defined(_WIN32) || defined(__CYGWIN__)
  } else {
    this->ok = true;
    this->uid = pw->pw_uid;
    this->gid = pw->pw_gid;
    this->name = pw->pw_name; // login name
    this->home_directory = pw->pw_dir;
    this->full_name = pw->pw_gecos;
    this->shell = pw->pw_shell;
    this->passwd = pw->pw_passwd;
  }
#endif
}
