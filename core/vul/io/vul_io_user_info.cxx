// This is vxl/vul/io/vul_io_user_info.cxx

#include <vcl_cstdlib.h> // vcl_abort()
#include <vul/vul_user_info.h>
#include <vul/io/vul_io_user_info.h>

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
  short v;
  vsl_b_read(is, v);
  switch(v)
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
    vcl_cerr << "vsl_b_read() Unknown version number "<< v << vcl_endl;
    vcl_abort();
  }
}


//=========================================================================
//: Output a human readable summary to the stream
void vsl_print_summary(vcl_ostream& os,const vul_user_info & p)
{
      os<<"( uid,gid,name,home_directory,full_name,shell,passwd="<<
      p.uid<<","<<p.gid<<","<<p.name<<
      " home_directory,full_name,shell=" <<
      p.home_directory<<","<< p.full_name<<","<<p.shell<<
      " passwd="<<
      p.passwd<<")";
}

