#include <vxl_copyright.h>
#include <vxl_version.h>
#include <vcl_iostream.h>

int main (int argc, char * argv[])
{
  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] == 'n' && argv[i][1] == 'l' && argv[i][2] == 0)
      vcl_cout << vcl_endl;
    else
      vcl_cout << argv[i] << " ";
  }
  vcl_cout << vcl_endl;
  return 0;
}
