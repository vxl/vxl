
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_compiler.h>

int main()
{
  for(int i = 0; i < 2; ++i)
    vcl_cerr << i << vcl_endl;
  for(int i = 0; i < 2; ++i);

  return 0;
}
