// This is brl/bbas/bxml/tests/bxml_test_vsol_vertex_2d_input_converter.cxx
#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <bxml/bxml_io.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>

bool near_eq(double x, double y)
{
  double d = x-y;
  double er = vcl_abs(d);
  return (er<1e-03);
}


#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

#define test_path "c:/vxl/vxl/contrib/brl/bbas/bxml/tests/"

void print_gp_point(bxml_generic_ptr& gp)
{
  vsol_spatial_object_2d* so = gp.get_vsol_spatial_object();
  if(!so)
    {
      vcl_cout << " null so in gp" << vcl_endl;
      return;
    }
  if(so->spatial_type()!=2)
    {
      vcl_cout << " so is not a point" << vcl_endl;
      return;
    }
  vsol_point_2d* p = (vsol_point_2d*)so;
  vcl_cout << "point = (" << p->x() << " " << p->y() << ")" << vcl_endl;
}

class test
{
public:	
  static void init(){
    vcl_string s0 = "0", s1 = "1", s2 = "2" , s3 = "3";
    vsol_spatial_object_2d* p0 = new vsol_point_2d(0,0);
    vsol_spatial_object_2d* p1 = new vsol_point_2d(1,1);
    vsol_spatial_object_2d* p2 = new vsol_point_2d(2,2);
    vsol_spatial_object_2d* p3 = new vsol_point_2d(3,3);
    bxml_generic_ptr gp0(p0), gp1(p1), gp2(p2), gp3(p3);
	  _tab[s0]=p0;   _tab[s1]=p1;   _tab[s2]=p2; _tab[s3]=p3;
  }
  static vcl_map<vcl_string,bxml_generic_ptr,vcl_less<vcl_string> >& get_map()
  {return _tab;}
  static vcl_map<vcl_string,bxml_generic_ptr,vcl_less<vcl_string> > _tab;
};
typedef vcl_map<vcl_string,bxml_generic_ptr,vcl_less<vcl_string> > OTAB;
OTAB test::_tab;

int main(int, char **)
{

  int success=0, failures=0;
  test::init();
  // we want to test the methods on bxml_vtol_vertex_2d_input_converter
  vcl_cout << "Testing stl map<string.bxml_generic_ptr>" << vcl_endl;

  vcl_string s0 = "0", s1 = "1", s2 = "2" , s3 = "3";
  //program scope 

  print_gp_point(test::get_map()[s0]);   print_gp_point(test::get_map()[s1]);
  print_gp_point(test::get_map()[s2]);   print_gp_point(test::get_map()[s3]);
  
  vcl_cout << "finished testing stl map<string.bxml_generic_ptr>"
           << vcl_endl;
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
