// This is gel/vtol/tests/test_curve_algs.cxx
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <bdgl/bdgl_curve_algs.h>
#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;

  // we want to test the methods on bdgl_curve_algs
  vcl_cout << "Testing nearest point" << vcl_endl;
  // construct and edgel chain at 45 degrees
  int N = 10;
  vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain();
  for(int i = 0; i< N; i++)
    ec->add_edgel(vdgl_edgel((double)i,(double)i));
  int ic = bdgl_curve_algs::closest_point(ec, 7, 4);
  vcl_cout << "Closest point index for (7, 4)  = " << ic << vcl_endl;
  Assert(ic==5);
  vcl_cout << "finished testing bdgl_curve_algs" << vcl_endl;
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
