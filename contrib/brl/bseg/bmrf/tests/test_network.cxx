//:
// \file
#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vpl/vpl.h>

#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_node.h>
#include <bmrf/bmrf_epipole.h>
#include <bmrf/bmrf_network.h>

//: Test the network class
void test_network()
{
  bmrf_node_sptr node_1 = new bmrf_node(new bmrf_epi_seg, 1, 0.1);
  bmrf_node_sptr node_2 = new bmrf_node(new bmrf_epi_seg, 1, 0.2);
  bmrf_node_sptr node_3 = new bmrf_node(new bmrf_epi_seg, 2, 0.3);
  bmrf_node_sptr node_4 = new bmrf_node(new bmrf_epi_seg, 2, 0.4);
  bmrf_node_sptr node_5 = new bmrf_node(new bmrf_epi_seg, 3, 0.5);

  bmrf_node_sptr bad_node = new bmrf_node(NULL, 5, 0.0);

  bmrf_network_sptr the_network = new bmrf_network;

  TEST("Testing add_node()",
       the_network->add_node(node_1) &&
       the_network->add_node(node_2) &&
       the_network->add_node(node_3) &&
       the_network->add_node(node_4) &&
       the_network->add_node(node_5) &&
       !the_network->add_node(node_5) && // can't add a node twice
       !the_network->add_node(bad_node), // can't add a node with no segment
       true);
  
  TEST("Testing remove_node()",
       the_network->remove_node(node_3) &&
       the_network->remove_node(node_4) &&
       !the_network->remove_node(node_4) && // can't remove a node not in the network
       !the_network->remove_node(bad_node), // can't remove a node with no segment
       true);

  // make the arcs
  TEST("Testing add_arc()",     
       the_network->add_arc(node_1, node_2, bmrf_node::ALPHA) &&
       the_network->add_arc(node_2, node_1, bmrf_node::ALPHA) &&
       the_network->add_arc(node_1, node_3, bmrf_node::TIME) && // node_3 added back in
       the_network->add_arc(node_4, node_2, bmrf_node::TIME) && // node_4 added back in
       the_network->add_arc(node_2, node_4, bmrf_node::TIME) &&
       the_network->add_arc(node_4, node_3, bmrf_node::SPACE) &&
       the_network->add_arc(node_4, node_5, bmrf_node::TIME) &&
       the_network->add_arc(node_3, node_5, bmrf_node::SPACE) &&
       the_network->add_arc(node_3, node_5, bmrf_node::ALPHA) &&
       the_network->add_arc(node_5, node_3, bmrf_node::TIME) &&
       !the_network->add_arc(node_5, node_3, bmrf_node::TIME) && // can't add the same arc twice
       !the_network->add_arc(node_1, node_1, bmrf_node::ALPHA), // can't arc to self
       true);

  TEST("Testing node->size()",
       node_4->num_neighbors(bmrf_node::TIME) == 2 &&
       node_4->num_neighbors(bmrf_node::SPACE) == 1 &&
       node_4->num_neighbors(bmrf_node::ALPHA) == 0 &&
       node_4->num_neighbors() == 3,
       true);

  TEST("Testing seg_to_node()",
       the_network->seg_to_node(node_4->epi_seg()) == node_4 &&
       the_network->seg_to_node(node_1->epi_seg(),1) == node_1 &&
       the_network->seg_to_node(node_1->epi_seg(),2) == bmrf_node_sptr(NULL) &&
       the_network->seg_to_node(NULL) == bmrf_node_sptr(NULL),
       true);

  // remove arcs
  TEST("Testing remove_arc()",
       the_network->remove_arc(node_3, node_5, bmrf_node::ALPHA) &&
       the_network->remove_arc(node_3, node_5) &&
       !the_network->remove_arc(node_3, node_5), // can't remove an arc not in the graph
       true);

  TEST("Testing size()",
       the_network->size() == 5 &&
       the_network->size(1) == 2 &&
       the_network->size(2) == 2 &&
       the_network->size(3) == 1 &&
       the_network->size(10) == 0,
       true);

  TEST("Testing purge()",the_network->purge(),false);

  bmrf_node_sptr depth_order[] = {node_4, node_3, node_2, node_1, node_5};
  bool depth_check = true;
  bmrf_network::depth_iterator d_itr = the_network->depth_begin(node_4);
  for(int i=0; d_itr != the_network->depth_end(); ++d_itr, ++i)
    depth_check = ((*d_itr) == depth_order[i]) && depth_check;
  TEST("Testing depth_iterator", depth_check, true);

  bmrf_node_sptr breadth_order[] = {node_4, node_3, node_2, node_5, node_1};
  bool breadth_check = true;
  bmrf_network::breadth_iterator b_itr = the_network->breadth_begin(node_4);
  for(int i=0; b_itr != the_network->breadth_end(); ++b_itr, ++i) 
    breadth_check = ((*b_itr) == breadth_order[i]) && breadth_check;
  TEST("Testing breadth_iterator", breadth_check, true);

  bmrf_epipole ep(-16,432);
  the_network->set_epipole(ep,1);
  TEST("Testing epipole", the_network->epipole(1).location(), ep.location());

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
  vpl_unlink ("test_network_io.tmp");

  TEST("Testing size() on loaded network",
       network_in->size() == the_network->size() &&
       network_in->size(1) == the_network->size(1) &&
       network_in->size(2) == the_network->size(2) &&
       network_in->size(3) == the_network->size(3) &&
       network_in->size(10) == the_network->size(10),
       true);

  TEST("Testing loaded epipole", 
       the_network->epipole(1).location() ,
       network_in->epipole(1).location() );
}


MAIN( test_network )
{
  START( "bmrf_network" );
  test_network();
  SUMMARY();
}
