#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vpl/vpl.h>

#include <bmrf/bmrf_node_sptr.h>
#include <bmrf/bmrf_node.h>
 
//: Test the node class
void test_node()
{
  bmrf_node_sptr node_1 = new bmrf_node(NULL, 1, 0.5);
  bmrf_node_sptr node_2 = new bmrf_node(NULL, 2, 0.7);
  bmrf_node_sptr node_3 = new bmrf_node(NULL, 2, 0.2);

  TEST("Testing add_neighbor()",
       node_1->add_neighbor(node_2.ptr(), bmrf_node::TIME) &&
       node_1->add_neighbor(node_3.ptr(), bmrf_node::TIME) &&
       node_2->add_neighbor(node_3.ptr(), bmrf_node::SPACE) &&
       node_2->add_neighbor(node_1.ptr(), bmrf_node::TIME) &&
       node_2->add_neighbor(node_3.ptr(), bmrf_node::ALPHA) &&
       node_2->add_neighbor(node_1.ptr(), bmrf_node::ALPHA) &&
       !node_2->add_neighbor(node_1.ptr(), bmrf_node::ALPHA), // can't add the same thing twice
       true);

  TEST("Testing remove_neighbor()",
       node_2->remove_neighbor(node_1.ptr(), bmrf_node::ALPHA) &&
       !node_2->remove_neighbor(node_1.ptr(), bmrf_node::ALPHA), // can't remove twice
       true);

  int count = 0;
  for(int i=0; i<bmrf_node::ALL; ++i)
    count += node_2->num_neighbors(bmrf_node::neighbor_type(i));
  TEST("Testing num_neighbors()", node_2->num_neighbors(), count );

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
  for(int i=0; i<=bmrf_node::ALL; ++i)
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
