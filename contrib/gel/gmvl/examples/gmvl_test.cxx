// This is gel/gmvl/examples/gmvl_test.cxx

//:
// \file
// \author crossge@crd.ge.com

#include <vcl_fstream.h>

#include <gmvl/gmvl_database.h>
#include <gmvl/gmvl_image_node.h>
#include <gmvl/gmvl_node_sptr.h>
#include <gmvl/gmvl_tag_node.h>
#include <gmvl/gmvl_helpers.h>

int main( int /* argc */, char ** /* argv */)
{
#if 0
  vbl_bit_array_2d ba(5,5, false);
  vcl_cerr << ba << vcl_endl;
  ba.put(0,0,true);
  ba.put(0,4,true);
  ba.put(1,1,true);
  ba.put(1,3,true);
  ba.put(2,0,true);
  ba.put(2,2,true);
  ba.put(2,4,true);
  ba.put(3,1,true);
  ba.put(3,3,true);
  ba.put(4,0,true);
  ba.put(4,2,true);
  ba.put(4,4,true);

  vcl_cerr << ba << vcl_endl;
  ba.enlarge(12,5);
  vcl_cerr << ba << vcl_endl;
  vbl_bit_array_2d cd( ba);
  vcl_cerr << cd << vcl_endl;
  vcl_exit(1);
#endif
  gmvl_node_sptr image0= gmvl_load_image( "/home/crossge/images/basement/bt.000.pgm");
  gmvl_node_sptr image1= gmvl_load_image( "/home/crossge/images/basement/bt.001.pgm");
  gmvl_node_sptr image2= gmvl_load_image( "/home/crossge/images/basement/bt.002.pgm");
  gmvl_node_sptr image3= gmvl_load_image( "/home/crossge/images/basement/bt.003.pgm");
  gmvl_node_sptr image4= gmvl_load_image( "/home/crossge/images/basement/bt.004.pgm");
  gmvl_node_sptr image5= gmvl_load_image( "/home/crossge/images/basement/bt.005.pgm");
  gmvl_node_sptr image6= gmvl_load_image( "/home/crossge/images/basement/bt.006.pgm");

  vcl_vector<gmvl_node_sptr> corners0= gmvl_load_raw_corners("/home/crossge/images/basement/bt.000.corners");
  vcl_vector<gmvl_node_sptr> corners1= gmvl_load_raw_corners("/home/crossge/images/basement/bt.001.corners");
  vcl_vector<gmvl_node_sptr> corners2= gmvl_load_raw_corners("/home/crossge/images/basement/bt.002.corners");
  vcl_vector<gmvl_node_sptr> corners3= gmvl_load_raw_corners("/home/crossge/images/basement/bt.003.corners");
  vcl_vector<gmvl_node_sptr> corners4= gmvl_load_raw_corners("/home/crossge/images/basement/bt.004.corners");
  vcl_vector<gmvl_node_sptr> corners5= gmvl_load_raw_corners("/home/crossge/images/basement/bt.005.corners");
  vcl_vector<gmvl_node_sptr> corners6= gmvl_load_raw_corners("/home/crossge/images/basement/bt.006.corners");

  gmvl_node_sptr tagnode1( new gmvl_tag_node( "Input Images"));
  gmvl_node_sptr tagnode2( new gmvl_tag_node( "Harris Corners"));
  gmvl_node_sptr tagnode3( new gmvl_tag_node( "First Corner"));

  gmvl_database db;

  vcl_cerr << "Connecting image-tag to images\n";
  db.add_connection( tagnode1, image0);
  db.add_connection( tagnode1, image1);
  db.add_connection( tagnode1, image2);
  db.add_connection( tagnode1, image3);
  db.add_connection( tagnode1, image4);
  db.add_connection( tagnode1, image5);
  db.add_connection( tagnode1, image6);
  vcl_cerr << "Connecting 5th corner to first-corner-node\n";
  db.add_connection( tagnode3, corners0[0]);
  db.add_connection( tagnode3, corners1[0]);
  db.add_connection( tagnode3, corners2[0]);
  db.add_connection( tagnode3, corners3[0]);
  db.add_connection( tagnode3, corners4[0]);
  db.add_connection( tagnode3, corners5[0]);
  db.add_connection( tagnode3, corners6[0]);
  vcl_cerr << "Connecting harris-corner-node to corners\n";
  db.add_connections( tagnode2, corners0);
  db.add_connections( tagnode2, corners1);
  db.add_connections( tagnode2, corners2);
  db.add_connections( tagnode2, corners3);
  db.add_connections( tagnode2, corners4);
  db.add_connections( tagnode2, corners5);
  db.add_connections( tagnode2, corners6);
  vcl_cerr << "Connecting images to corners\n";
  db.add_connections( image0, corners0);
  db.add_connections( image1, corners1);
  db.add_connections( image2, corners2);
  db.add_connections( image3, corners3);
  db.add_connections( image4, corners4);
  db.add_connections( image5, corners5);
  db.add_connections( image6, corners6);

  vcl_cerr << "Done loading...\n";

#if 0 ////////////////////////////////////////////////////////////////////////
  vcl_cerr << db << vcl_endl;

  vcl_vector<gmvl_node_sptr> nn= db.get_nodes( "gmvl_tag_node");
  for ( int i=0; i< nn.size(); i++)
    vcl_cerr << ":" << *nn[i] << ":\n";

  vcl_vector<gmvl_node_sptr> jj= db.get_nodes( "gmvl_image_node");
  for ( int i=0; i< jj.size(); i++)
    vcl_cerr << ":" << *jj[i] << ":\n";
//////////////////////////////////////////////////////////////////////////////

  vcl_ofstream fout("/tmp/qwerty");
  fout << db << vcl_endl;

  vcl_cout << db << vcl_endl;

  vcl_vector<gmvl_node_sptr> tags;
  tags.push_back( tagnode2); // "Harris corners"
  tags.push_back( image0);   // "Image 0"
  tags.push_back( tagnode3); // "First corner"
#endif

  vcl_vector<gmvl_node_sptr> nn= db.get_connected_nodes( tagnode3);

  vcl_cerr << "Tagnode1 (all images) = " << *tagnode1 << vcl_endl;
  vcl_cerr << "Tagnode2 (all corners)= " << *tagnode2 << vcl_endl;
  vcl_cerr << "Tagnode3 (5th corners)= " << *tagnode3 << vcl_endl;
  vcl_cerr << "Image 0               = " << *image0 << vcl_endl;

  vcl_cerr << vcl_endl << vcl_endl;

  for ( unsigned int i=0; i< nn.size(); i++)
    vcl_cerr << *nn[i] << vcl_endl;

  return 0;
}
