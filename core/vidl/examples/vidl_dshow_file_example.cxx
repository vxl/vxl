#include <iostream>
#include <vidl/vidl_dshow_file_istream.h>
#include <vidl/vidl_image_list_ostream.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

int main()
{
  vidl_dshow_file_istream test1("seq_ch2.avi");
  vidl_dshow_file_istream test2("test1.avi");
  vidl_dshow_file_istream test3("test2.avi");
  vidl_dshow_file_istream test4("test3.wmv");

  vidl_image_list_ostream test_out1("./dump","%05d","bmp");

  int i = 10;
  while (test1.advance() && test2.advance() && --i)
  {
    std::cout << "Frame: " << i << std::endl;
    test_out1.write_frame(test1.current_frame());
    test_out1.write_frame(test2.current_frame());
    test3.advance();
    test4.advance();
  }

  vidl_image_list_ostream test_out2("./dump","s%05d","bmp");
  if (test1.is_seekable() && test2.is_seekable())
  {
    if (test1.seek_frame(0)) { test_out2.write_frame(test1.current_frame()); }
    if (test1.seek_frame(1)) { test_out2.write_frame(test1.current_frame()); }
    if (test1.seek_frame(2)) { test_out2.write_frame(test1.current_frame()); }
    if (test1.seek_frame(0)) { test_out2.write_frame(test1.current_frame()); }
    if (test1.seek_frame(1)) { test_out2.write_frame(test1.current_frame()); }
    if (test1.seek_frame(2)) { test_out2.write_frame(test1.current_frame()); }
    if (test2.seek_frame(0)) { test_out2.write_frame(test2.current_frame()); }
    if (test2.seek_frame(1)) { test_out2.write_frame(test2.current_frame()); }
    if (test2.seek_frame(2)) { test_out2.write_frame(test2.current_frame()); }
    if (test2.seek_frame(0)) { test_out2.write_frame(test2.current_frame()); }
    if (test2.seek_frame(1)) { test_out2.write_frame(test2.current_frame()); }
    if (test2.seek_frame(2)) { test_out2.write_frame(test2.current_frame()); }
  }

  return 0;
}
