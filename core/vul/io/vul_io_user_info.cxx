// This is core/vul/io/vul_io_user_info.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vul_io_user_info.h"
#include <vul/vul_user_info.h>

//=========================================================================
//: Binary save self to stream.
void vsl_b_write(vsl_b_ostream &os, const vul_user_info & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.uid);
  vsl_b_write(os, p.gid);
  vsl_b_write(os, p.name);
  vsl_b_write(os, p.home_directory);
  vsl_b_write(os, p.full_name);
  vsl_b_write(os, p.shell);
  vsl_b_write(os, p.passwd);
}

//=========================================================================
//: Binary load self from stream.
void vsl_b_read(vsl_b_istream &is, vul_user_info & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    vsl_b_read(is, p.uid);
    vsl_b_read(is, p.gid);
    vsl_b_read(is, p.name);
    vsl_b_read(is, p.home_directory);
    vsl_b_read(is, p.full_name);
    vsl_b_read(is, p.shell);
    vsl_b_read(is, p.passwd);
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vul_user_info&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//=========================================================================
//: Output a human readable summary to the stream
void vsl_print_summary(vcl_ostream& os,const vul_user_info & p)
{
      os<<"( uid,gid,name,home_directory,full_name,shell,passwd="<<
      p.uid<<','<<p.gid<<','<<p.name<<
      " home_directory,full_name,shell=" <<
      p.home_directory<<','<< p.full_name<<','<<p.shell<<
      " passwd="<<
      p.passwd<<')';
}
