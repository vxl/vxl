#include <vector>
#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_corr.h>
#include <bwm/video/bwm_video_corr_sptr.h>
#include <vgl/vgl_point_2d.h>

static void test_video_site_io()
{
  std::string xml_path = "site_io.xml";

  bwm_video_site_io sio;
  sio.set_name("my_name");
  sio.set_video_path("my_video");
  sio.set_camera_path("my_camera");
  std::vector<bwm_video_corr_sptr> corrs;
  bwm_video_corr_sptr c0 = new bwm_video_corr();
  bwm_video_corr_sptr c1 = new bwm_video_corr();
  vgl_point_2d<double> p00(0.0, 1.0), p01(1.0, 0.0);
  vgl_point_2d<double> p10(2.0, 1.0), p11(1.0, 2.0);
  c0->add(0, p00); c0->add(1, p01);
  c1->add(0, p10); c1->add(1, p11);
  corrs.push_back(c0);   corrs.push_back(c1);
  sio.set_corrs(corrs);
  sio.x_write(xml_path);
  sio.open(xml_path);
  bool good = sio.name() == "my_name";
  good = good && sio.video_path() == "my_video";
  good = good && sio.camera_path() == "my_camera";
  std::vector<bwm_video_corr_sptr> restored_corrs = sio.corrs();
  unsigned i = 0;
  for (auto cit = restored_corrs.begin();
       cit != restored_corrs.end(); ++cit, ++i)
  {
    vgl_point_2d<double> pt;
    vgl_point_2d<double> ptr;
    corrs[i]->match(i, pt);
    (*cit)->match(i, ptr);
    good = good && pt == ptr;
  }
  TEST("save and restore correspondences", good, true);
  vpl_unlink(xml_path.c_str());
}

TESTMAIN(test_video_site_io);
