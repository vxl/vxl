//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vbl_user_info.h"
#endif
//
// Class: vbl_user_info
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 08 Apr 99
// Modifications:
//   990408 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vbl_user_info.h"

#ifndef WIN32
#include <unistd.h>
#include <pwd.h>
#endif

#include <vcl/vcl_iostream.h>

// -- Lookup info from login name
vbl_user_info::vbl_user_info(const char* name)
{
  init(name);
}

// -- Lookup info from login name
vbl_user_info::vbl_user_info(const vcl_string& name)
{
  init(name.c_str());
}

// -- Lookup info from numeric user-id
vbl_user_info::vbl_user_info(uid_type uid)
{
  init(uid);
}

//  uid_type uid;
//  uid_type gid;
//  vcl_string name; // login name
//  vcl_string home_directory;
//  vcl_string full_name;
//  vcl_string shell;
//  vcl_string passwd;

void vbl_user_info::init(uid_type /*uid*/)
{
  cerr << "Not implemented\n";
}

void vbl_user_info::init(char const* name)
{
#ifndef WIN32
  struct passwd* pw = getpwnam(name);
  if (!pw) {
#endif
    this->ok = false;
    this->uid = 0;
    this->gid = 0;
    this->name = name;
#ifndef WIN32
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
