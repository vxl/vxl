#include <vidl2/vidl2_dshow_file_istream.h>
#include <vidl2/vidl2_image_list_ostream.h>

#include <vcl_iostream.h>

int main()
{
  vidl2_dshow_file_istream test1("seq_ch2.avi");
  vidl2_dshow_file_istream test2("test1.avi");
  vidl2_dshow_file_istream test3("test2.avi");
  vidl2_dshow_file_istream test4("test3.wmv");

  vidl2_image_list_ostream test_out1("./dump","%05d","bmp");

  int i = 10;
  while (test1.advance() && test2.advance() && --i)
  {
    vcl_cout << "Frame: " << i << vcl_endl;
    test_out1.write_frame(test1.current_frame());
    test_out1.write_frame(test2.current_frame());
    test3.advance();
    test4.advance();
  }

  vidl2_image_list_ostream test_out2("./dump","s%05d","bmp");
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
