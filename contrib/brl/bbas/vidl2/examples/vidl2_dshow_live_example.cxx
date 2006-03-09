#include <vidl2/vidl2_dshow_live_istream.h>
#include <vidl2/vidl2_dshow_istream_params.h>
//#include <vidl2/vidl2_dshow_istream_params_esf.h>
#include <vidl2/vidl2_image_list_ostream.h>

#include <vidl2/vidl2_dshow.h>

#include <vul/vul_timer.h>
#include <vcl_fstream.h>
#include <mbl/mbl_read_props.h>

const vcl_string config_file
= "d:/cvs_root/vxl/contrib/brl/bbas/vidl2/examples/config.dshow.logitech";
//= "d:/cvs_root/vxl/contrib/brl/bbas/vidl2/examples/config.euresys.vid1";

int main()
{
  //vidl2_dshow::print_capture_device_names();

  //vidl2_dshow_istream_params::print_parameter_help("Logitech QuickCam PTZ");
  //vidl2_dshow_istream_params::print_parameter_help("Euresys PICOLO DILIGENT sn/29 - VID1");
  //vidl2_dshow_istream_params_esf::print_parameter_help("Euresys PICOLO DILIGENT sn/29 - VID1");

  vidl2_dshow_live_istream<vidl2_dshow_istream_params> test(
    vidl2_dshow_istream_params()
    .set_device_name("Logitech QuickCam PTZ")
    .set_properties(mbl_read_props(vcl_ifstream(config_file.c_str())))
    );

  //vidl2_dshow_live_istream<vidl2_dshow_istream_params_esf> test1(
  //  vidl2_dshow_istream_params_esf()
  //  .set_device_name("Euresys PICOLO DILIGENT sn/29 - VID1")
  //  .set_properties(mbl_read_props(vcl_ifstream(config_file.c_str())))
  //  );

  //vidl2_dshow_live_istream<vidl2_dshow_istream_params_esf> test2(
  //  vidl2_dshow_istream_params_esf()
  //  .set_device_name("Euresys PICOLO DILIGENT sn/29 - VID2")
  //  );

  //vidl2_dshow_live_istream<vidl2_dshow_istream_params_esf> test3(
  //  vidl2_dshow_istream_params_esf()
  //  .set_device_name("Euresys PICOLO DILIGENT sn/29 - VID3")
  //  );

  vidl2_image_list_ostream test_out("./dump","%05d","bmp");

  vul_timer timer;
  int i = 150;
  while ( test.advance()
       //   test1.advance()
       //&& test2.advance()
       //&& test3.advance()
       && --i)
  {
    vcl_cout << "Grabbing: " << i << vcl_endl;
    test.current_frame();
    test_out.write_frame(test.current_frame());
  }
  timer.print(vcl_cout);

  return 0;
}
