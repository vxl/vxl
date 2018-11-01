// This is core/vul/vul_user_info.h
#ifndef vul_user_info_h_
#define vul_user_info_h_
//:
// \file
// \brief Reads information about a user from the password file.
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   08 Apr 1999
//
// \verbatim
//  Modifications
//   990408 AWF Initial version.
//   010321 C. Beeston documentation changes
// \endverbatim
//-----------------------------------------------------------------------------

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Reads information about a user from the password file
class vul_user_info
{
 public:
  typedef unsigned long uid_type;

  bool ok;
  uid_type uid;
  uid_type gid;
  std::string name; // login name
  std::string home_directory;
  std::string full_name;
  std::string shell;
  std::string passwd;

  // Constructors/Destructors--------------------------------------------------

  //: Lookup info from numeric user-id
  vul_user_info(uid_type u_id) { init(u_id); }
  //: Lookup info from login name
  vul_user_info(char const* nam) { init(nam); }
  //: Lookup info from login name
  vul_user_info(std::string const& nam) { init(nam.c_str()); }
  //: NYI
  void init(uid_type);
  //: initialise vul_user_info for given login name
  void init(char const* nam);
};

#endif // vul_user_info_h_
