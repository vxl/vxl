#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vul/vul_root_dir.h>

int main()
{
  // Check that a file exists
  vcl_string path = vul_root_dir() + "/vxl/vul/vul_root_dir.h";

  vcl_fstream is(path.c_str(),vcl_ios_in);

  if (!is)
  {
    vcl_cerr<<"Unable to open "<<path<<"\nvul_root_dir() is probably wrong.\n"
              "Try setting $VXLSRC to the source root directory."<<vcl_endl;
    return 1;
  }
  is.close();

  vcl_cout<<"Root Dir: "<<vul_root_dir()<<" appears to be correct."<<vcl_endl;

  return 0;
}
