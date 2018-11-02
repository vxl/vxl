#include <fstream>
#include <vidl/vidl_dshow_live_istream.h>
#include <vidl/vidl_dshow_istream_params.h>
#include <vidl/vidl_image_list_ostream.h>

#include <vidl/vidl_dshow.h>

#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if 0 // Commented out
const std::string config_file = "core/vidl/examples/config.dshow.logitech"; // config.euresys.vid1
#endif

int main()
{
#if 0 // Commented out
  vidl_dshow::print_capture_device_names();

  vidl_dshow_istream_params::print_parameter_help("Logitech QuickCam PTZ");
  vidl_dshow_istream_params::print_parameter_help("Euresys PICOLO DILIGENT sn/29 - VID1");
  vidl_dshow_istream_params_esf::print_parameter_help("Euresys PICOLO DILIGENT sn/29 - VID1");
#endif // 0

#if 0 // There should be no reference to mbl_* in core !!
  vidl_dshow_live_istream<vidl_dshow_istream_params> test(
    vidl_dshow_istream_params()
    .set_device_name("Logitech QuickCam PTZ")
    .set_properties(mbl_read_props(std::ifstream(config_file.c_str())))
    );

  vidl_dshow_live_istream<vidl_dshow_istream_params_esf> test1(
    vidl_dshow_istream_params_esf()
    .set_device_name("Euresys PICOLO DILIGENT sn/29 - VID1")
    .set_properties(mbl_read_props(std::ifstream(config_file.c_str())))
    );
#else
  vidl_dshow_live_istream<vidl_dshow_istream_params> test;
#endif // 0

#if 0 // commented out
  vidl_dshow_live_istream<vidl_dshow_istream_params_esf> test2(
    vidl_dshow_istream_params_esf()
    .set_device_name("Euresys PICOLO DILIGENT sn/29 - VID2")
    );

  vidl_dshow_live_istream<vidl_dshow_istream_params_esf> test3(
    vidl_dshow_istream_params_esf()
    .set_device_name("Euresys PICOLO DILIGENT sn/29 - VID3")
    );
#endif // 0

  vidl_image_list_ostream test_out("./dump","%05d","bmp");

  vul_timer timer;
  int i = 150;
  while ( test.advance()
          //   test1.advance()
          //&& test2.advance()
          //&& test3.advance()
          && --i)
  {
    std::cout << "Grabbing: " << i << std::endl;
    test.current_frame();
    test_out.write_frame(test.current_frame());
  }
  timer.print(std::cout);

  return 0;
}
