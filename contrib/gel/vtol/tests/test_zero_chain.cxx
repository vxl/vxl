#include <vcl_fstream.h>

#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_zero_chain_sptr.h>
#include <vtol/vtol_zero_chain.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;

  vcl_cout << "testing zero chain" << vcl_endl;

  vtol_vertex_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_sptr v2 = new vtol_vertex_2d(1.0,1.0);

  vtol_zero_chain_sptr zc1 = new vtol_zero_chain(*v1,*v2);

  vtol_zero_chain_sptr zc1_clone = zc1->clone()->cast_to_topology_object()->
    cast_to_zero_chain();


  vtol_vertex_sptr v3 = new vtol_vertex_2d(2.0,2.0);
  vtol_vertex_sptr v4 = new vtol_vertex_2d(3.0,3.0);


  vcl_vector<vtol_vertex_sptr> v_list;
  v_list.push_back(v2);
  v_list.push_back(v3);

  vtol_zero_chain_sptr zc1a = new vtol_zero_chain(v_list);

  Assert(*zc1 != *zc1a);
  Assert(*zc1 == *zc1);

  Assert(zc1->valid_inferior_type(*v1));
  Assert(!(zc1->valid_superior_type(*v1)));
  Assert(zc1->length()==2);
  Assert(zc1->topology_type()==vtol_topology_object::ZEROCHAIN);

  vcl_cout << "Finished testing zero chain 2d" << vcl_endl;
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
