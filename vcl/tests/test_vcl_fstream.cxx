#include <vcl/vcl_fstream.h>

int main(int, char **) 
{
  if (false) {
    ofstream f("dont_worry_this_file_is_not_created", vcl_ios_binary);

    f.write((char*)"hello, file", 11);

    f.close();
  }
  return 0;
}
