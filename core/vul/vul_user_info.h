// This is core/vul/vul_user_info.h
#ifndef vul_user_info_h_
#define vul_user_info_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Reads information about a user from the password file.
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   08 Apr 99
//
// \verbatim
//  Modifications
//   990408 AWF Initial version.
//   010321 C. Beeston documentation changes
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_string.h>

//: Reads information about a user from the password file
class vul_user_info
{
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

  //: Lookup info from numeric user-id
  vul_user_info(uid_type uid) { init(uid); }
  //: Lookup info from login name
  vul_user_info(char const* name) { init(name); }
  //: Lookup info from login name
  vul_user_info(vcl_string const& name) { init(name.c_str()); }
  //: NYI
  void init(uid_type);
  //: initialise vul_user_info for given login name
  void init(char const* name);
};

#endif // vul_user_info_h_
