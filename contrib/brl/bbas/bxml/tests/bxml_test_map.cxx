// This is brl/bbas/bxml/tests/bxml_test_map.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <bxml/bxml_io.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>

void print_gp_point(bxml_generic_ptr& gp)
{
  vsol_spatial_object_2d* so = gp.get_vsol_spatial_object();
  if (!so)
  {
    vcl_cout << " null so in gp\n";
    return;
  }
  vsol_point_2d* p = so->cast_to_point();
  if (!p)
  {
    vcl_cout << " so is not a point\n";
    return;
  }
  vcl_cout << "point = (" << p->x() << ' ' << p->y() << ")\n";
}

class test
{
 public:
  static void init() {
    vcl_string s0 = "0", s1 = "1", s2 = "2" , s3 = "3";
    vsol_spatial_object_2d* p0 = new vsol_point_2d(0,0);
    vsol_spatial_object_2d* p1 = new vsol_point_2d(1,1);
    vsol_spatial_object_2d* p2 = new vsol_point_2d(2,2);
    vsol_spatial_object_2d* p3 = new vsol_point_2d(3,3);
    bxml_generic_ptr gp0(p0), gp1(p1), gp2(p2), gp3(p3);
    vcl_cout << "p0: " << gp0 << "p1: " << gp1 << "p2: " << gp2 << "p3: " <<gp3;
    tab_[s0]=p0;   tab_[s1]=p1;   tab_[s2]=p2; tab_[s3]=p3;
  }
  static vcl_map<vcl_string,bxml_generic_ptr,vcl_less<vcl_string> >& get_map()
  { return tab_; }
  static vcl_map<vcl_string,bxml_generic_ptr,vcl_less<vcl_string> > tab_;
};

vcl_map<vcl_string,bxml_generic_ptr,vcl_less<vcl_string> > test::tab_;

void bxml_test_map()
{
  test::init();

  vcl_string s0 = "0", s1 = "1", s2 = "2" , s3 = "3";

  print_gp_point(test::get_map()[s0]);
  print_gp_point(test::get_map()[s1]);
  print_gp_point(test::get_map()[s2]);
  print_gp_point(test::get_map()[s3]);
}

TESTMAIN(bxml_test_map);
