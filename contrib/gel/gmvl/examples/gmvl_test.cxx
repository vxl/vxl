/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "main"
#endif

#include <vcl/vcl_fstream.h>

#include <vil/vil_image.h>
#include <vil/vil_load.h>

#include <gmvl/gmvl_database.h>
#include <gmvl/gmvl_image_node.h>
#include <gmvl/gmvl_node_ref.h>
#include <gmvl/gmvl_tag_node.h>
#include <gmvl/gmvl_helpers.h>

int main( int argc, char **argv)
{
//   gbl_bit_array_2d ba(5,5, false);
//   cerr << ba << endl;
//   ba.put(0,0,true);
//   ba.put(0,4,true);
//   ba.put(1,1,true);
//   ba.put(1,3,true);
//   ba.put(2,0,true);
//   ba.put(2,2,true);
//   ba.put(2,4,true);
//   ba.put(3,1,true);
//   ba.put(3,3,true);
//   ba.put(4,0,true);
//   ba.put(4,2,true);
//   ba.put(4,4,true);

//   cerr << ba << endl;
//   ba.enlarge(12,5);
//   cerr << ba << endl;
//   gbl_bit_array_2d cd( ba);
//   cerr << cd << endl;
//   exit(1);

  gmvl_node_ref image0= gmvl_load_image( "/home/crossge/images/basement/bt.000.pgm");
  gmvl_node_ref image1= gmvl_load_image( "/home/crossge/images/basement/bt.001.pgm");
  gmvl_node_ref image2= gmvl_load_image( "/home/crossge/images/basement/bt.002.pgm");
  gmvl_node_ref image3= gmvl_load_image( "/home/crossge/images/basement/bt.003.pgm");
  gmvl_node_ref image4= gmvl_load_image( "/home/crossge/images/basement/bt.004.pgm");
  gmvl_node_ref image5= gmvl_load_image( "/home/crossge/images/basement/bt.005.pgm");
  gmvl_node_ref image6= gmvl_load_image( "/home/crossge/images/basement/bt.006.pgm");

  vcl_vector<gmvl_node_ref> corners0= gmvl_load_raw_corners("/home/crossge/images/basement/bt.000.corners");
  vcl_vector<gmvl_node_ref> corners1= gmvl_load_raw_corners("/home/crossge/images/basement/bt.001.corners");
  vcl_vector<gmvl_node_ref> corners2= gmvl_load_raw_corners("/home/crossge/images/basement/bt.002.corners");
  vcl_vector<gmvl_node_ref> corners3= gmvl_load_raw_corners("/home/crossge/images/basement/bt.003.corners");
  vcl_vector<gmvl_node_ref> corners4= gmvl_load_raw_corners("/home/crossge/images/basement/bt.004.corners");
  vcl_vector<gmvl_node_ref> corners5= gmvl_load_raw_corners("/home/crossge/images/basement/bt.005.corners");
  vcl_vector<gmvl_node_ref> corners6= gmvl_load_raw_corners("/home/crossge/images/basement/bt.006.corners");

  gmvl_node_ref tagnode1( new gmvl_tag_node( "Input Images"));
  gmvl_node_ref tagnode2( new gmvl_tag_node( "Harris Corners"));
  gmvl_node_ref tagnode3( new gmvl_tag_node( "First Corner"));

  gmvl_database db;

  cerr << "Connecting image-tag to images" << endl;
  db.add_connection( tagnode1, image0);
  db.add_connection( tagnode1, image1);
  db.add_connection( tagnode1, image2);
  db.add_connection( tagnode1, image3);
  db.add_connection( tagnode1, image4);
  db.add_connection( tagnode1, image5);
  db.add_connection( tagnode1, image6);
  cerr << "Connecting 5th corner to first-corner-node" << endl;
  db.add_connection( tagnode3, corners0[0]);
  db.add_connection( tagnode3, corners1[0]);
  db.add_connection( tagnode3, corners2[0]);
  db.add_connection( tagnode3, corners3[0]);
  db.add_connection( tagnode3, corners4[0]);
  db.add_connection( tagnode3, corners5[0]);
  db.add_connection( tagnode3, corners6[0]);
  cerr << "Connecting harris-corner-node to corners" << endl;
  db.add_connections( tagnode2, corners0);
  db.add_connections( tagnode2, corners1);
  db.add_connections( tagnode2, corners2);
  db.add_connections( tagnode2, corners3);
  db.add_connections( tagnode2, corners4);
  db.add_connections( tagnode2, corners5);
  db.add_connections( tagnode2, corners6);
  cerr << "Connecting images to corners" << endl;
  db.add_connections( image0, corners0);
  db.add_connections( image1, corners1);
  db.add_connections( image2, corners2);
  db.add_connections( image3, corners3);
  db.add_connections( image4, corners4);
  db.add_connections( image5, corners5);
  db.add_connections( image6, corners6);

  cerr << "Done loading..." << endl;

  ////////////////////////////////////////////////////////////////////////////
  //   cerr << db << endl;
  //
  //   vcl_vector<gmvl_node_ref> nn= db.get_nodes( "gmvl_tag_node");
  //   for( int i=0; i< nn.size(); i++) cerr << ":" << *nn[i] << ":" << endl;
  //
  //   vcl_vector<gmvl_node_ref> jj= db.get_nodes( "gmvl_image_node");
  //   for( int i=0; i< jj.size(); i++) cerr << ":" << *jj[i] << ":" << endl;
  ////////////////////////////////////////////////////////////////////////////

  //  ofstream fout("/tmp/qwerty");
  //  fout << db << endl;

  //  cout << db << endl;

  //  vcl_vector<gmvl_node_ref> tags;
  //  tags.push_back( tagnode2); // "Harris corners"
  //  tags.push_back( image0);   // "Image 0"
  //  tags.push_back( tagnode3); // "First corner"

  vcl_vector<gmvl_node_ref> nn= db.get_connected_nodes( tagnode3);

  cerr << "Tagnode1 (all images) = " << *tagnode1 << endl;
  cerr << "Tagnode2 (all corners)= " << *tagnode2 << endl;
  cerr << "Tagnode3 (5th corners)= " << *tagnode3 << endl;
  cerr << "Image 0               = " << *image0 << endl;

  cerr << endl << endl;

  for( int i=0; i< nn.size(); i++) cerr << *nn[i] << endl;

  return 0;
}
