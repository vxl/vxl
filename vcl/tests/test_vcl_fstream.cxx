// include all the streams headers and <string>, to 
// ensure they are compatible.
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_fstream.h>
#include <vcl/vcl_strstream.h>
#include <vcl/vcl_string.h>

int main(int, char **) 
{
  if (false) {
    ofstream f("/tmp/dont_worry_this_file_is_not_created", vcl_ios_binary);
    f.write((char*)"hello, file", 11);
    f.close();
  }
  return 0;
}
