//:
// \file
#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vpl/vpl.h>

#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_node.h>
#include <bmrf/bmrf_network.h>

//: Test the network class
void test_network()
{
  bmrf_node_sptr node_1 = new bmrf_node(new bmrf_epi_seg, 1, 0.5);
  bmrf_node_sptr node_2 = new bmrf_node(new bmrf_epi_seg, 1, 0.7);
  bmrf_node_sptr node_3 = new bmrf_node(new bmrf_epi_seg, 2, 0.2);
  bmrf_node_sptr node_4 = new bmrf_node(new bmrf_epi_seg, 2, 0.1349);
  bmrf_node_sptr node_5 = new bmrf_node(new bmrf_epi_seg, 3, 0.8928);

  bmrf_node_sptr bad_node = new bmrf_node(NULL, 5, 0.0);

  bmrf_network_sptr the_network = new bmrf_network;

  TEST("Testing add_node()",
       the_network->add_node(node_1) &&
       the_network->add_node(node_2) &&
       the_network->add_node(node_3) &&
       the_network->add_node(node_4) &&
       the_network->add_node(node_5) &&
       !the_network->add_node(bad_node), // can't add a node with no segment
       true);


//----------------------------------------------------------------------------------------
// I/O Tests
//----------------------------------------------------------------------------------------

  // binary test output file stream
  vsl_b_ofstream bfs_out("test_network_io.tmp");
  TEST("Created test_network_io.tmp for writing",(!bfs_out), false);
  vsl_b_write(bfs_out, the_network);
  bfs_out.close();

  bmrf_network_sptr network_in;

  // binary test input file stream
  vsl_b_ifstream bfs_in("test_network_io.tmp");
  TEST("Opened test_network_io.tmp for reading",(!bfs_in), false);
  vsl_b_read(bfs_in, network_in);
  bfs_in.close();

  // remove the temporary file
  vpl_unlink ("test_node_io.tmp");
}


MAIN( test_network )
{
  START( "bmrf_network" );
  test_network();
  SUMMARY();
}
