
#include <iostream.h>

#include <vcl/vcl_compiler.h>

int main()
{
  for(int i = 0; i < 2; ++i)
    cerr << i << endl;
  for(int i = 0; i < 2; ++i);

  return 0;
}
