#ifndef vbl_user_info_h_
#define vbl_user_info_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vbl_user_info - Reads information about a user from the password file
// .LIBRARY vbl
// .HEADER  vxl package
// .INCLUDE vbl/vbl_user_info.h
// .FILE    vbl_user_info.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 08 Apr 99
//
// .SECTION Modifications
//     990408 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>

//
class vbl_user_info {
public:
  typedef unsigned long uid_type;

  bool ok;
  uid_type uid;
  uid_type gid;
  vcl_string name; // login name
  vcl_string home_directory;
  vcl_string full_name;
  vcl_string shell;
  vcl_string passwd;

  // Constructors/Destructors--------------------------------------------------

  vbl_user_info(uid_type uid);
  vbl_user_info(char const* name);
  vbl_user_info(vcl_string const& name);

  void init(uid_type);
  void init(char const* name);
};

#endif // vbl_user_info_h_
