//:
// \file
#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vpl/vpl.h>

#include <bmrf/bmrf_node_sptr.h>
#include <vbl/vbl_smart_ptr.h>
#include <bmrf/bmrf_node.h>

class bmrf_node_tester : public bmrf_node
{
public:
  bool add_neighbor( bmrf_node *node, neighbor_type type );

  bool remove_neighbor( bmrf_node *node, neighbor_type type = ALL);

  void strip();

  bool purge();
};

typedef vbl_smart_ptr<bmrf_node_tester> bmrf_node_tester_sptr;

//: Test the node class
void test_node()
{
  bmrf_node_sptr node_1 = new bmrf_node(NULL, 1, 0.5);
  bmrf_node_sptr node_2 = new bmrf_node(NULL, 2, 0.7);
  bmrf_node_sptr node_3 = new bmrf_node(NULL, 2, 0.2);

  testlib_test_begin("Testing frame_num() ");
  testlib_test_perform( node_1->frame_num() == 1 &&
                        node_2->frame_num() == 2 &&
                        node_3->frame_num() == 2 );

  testlib_test_begin("Testing probability()");
  testlib_test_perform( node_1->probability() == 0.5 &&
                        node_2->probability() == 0.7 &&
                        node_3->probability() == 0.2 );

//----------------------------------------------------------------------------------------
// I/O Tests
//----------------------------------------------------------------------------------------

  // binary test output file stream
  vsl_b_ofstream bfs_out("test_node_io.tmp");
  TEST("Created test_node_io.tmp for writing",(!bfs_out), false);
  vsl_b_write(bfs_out, node_1);
  bfs_out.close();

  bmrf_node_sptr node_in_1, node_in_2, node_in_3;

  // binary test input file stream
  vsl_b_ifstream bfs_in("test_node_io.tmp");
  TEST("Opened test_node_io.tmp for reading",(!bfs_in), false);
  vsl_b_read(bfs_in, node_in_1);
  bfs_in.close();

  bool same_neighbor_count = true;
  for (int i=0; i<=bmrf_node::ALL; ++i)
    same_neighbor_count = same_neighbor_count &&
                          ( node_in_1->num_neighbors(bmrf_node::neighbor_type(i))
                            == node_1->num_neighbors(bmrf_node::neighbor_type(i)) );
  // Compare the original data to the saved/loaded data
  TEST("Compared first saved to original",
       (node_in_1->probability() == node_1->probability()) &&
       (node_in_1->frame_num() == node_1->frame_num()) &&
       same_neighbor_count, true );

  // remove the temporary file
  vpl_unlink ("test_node_io.tmp");

}


MAIN( test_node )
{
  START( "bmrf_node" );
  test_node();
  SUMMARY();
}
