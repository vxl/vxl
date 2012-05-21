#include <testlib/testlib_test.h>

#include <boct/boct_bit_tree.h>
#include <vcl_iostream.h>

void test_print_centers()
{
  //store center x, y z
  float centerX[585], centerY[585], centerZ[585];
  centerX[0] = .5f;
  centerY[0] = .5f;
  centerZ[0] = .5f;

  //bottom left origins for calculating centers
  float ox[585], oy[585], oz[585];
  ox[0] = 0.0f;
  oy[0] = 0.0f;
  oz[0] = 0.0f;

  boct_bit_tree tree;
  for (int i=1; i<585; ++i) {

      //////////////////////////////////////////////////////////////
      //Determine octant that this child lies in (w.r.t it's parent)
      //[0-7] bit index of cell being looked up (@bit_index)
      unsigned char child_offset = (i-1)&(8-1);

      //decompose child to x,y,z \in {0,1} => determines octant
      unsigned char dx = (child_offset)    & 1;
      unsigned char dy = (child_offset>>1) & 1;
      unsigned char dz = (child_offset>>2) & 1;

      //////////////////////////////////////////////////////////////
      //record the origin of this cell (based on paretns origin and current octant)
      //calculate the side length of this cell
      int curr_depth = tree.depth_at(i);
      float side_len = 1.0f / (float) (1<<curr_depth);

      //bit index of parent
      unsigned char pi = (i-1)>>3;
      ox[i] = ox[pi] + side_len * dx;
      oy[i] = oy[pi] + side_len * dy;
      oz[i] = oz[pi] + side_len * dz;

      //intersect the cell, grab the intersection volume
      //vgl_point_3d<float> localCenter = tree.cell_center(bitI)

      centerX[i] = ox[i] + .5f * side_len;
      centerY[i] = oy[i] + .5f * side_len;
      centerZ[i] = oz[i] + .5f * side_len;
  }

  for (int i=0; i<25; ++i) {
    vcl_cout<<" bi : "<<i<<" center: "<<vgl_point_3d<float>(centerX[i], centerY[i], centerZ[i])<<vcl_endl;
  }

  //print out in an easily copied format.....
  vcl_cout<<"float centerX = { ";
  for (int i=0; i<584; ++i) {
    vcl_cout<<centerX[i]<<',';
    if (i%10 == 0) vcl_cout<<'\n';
  }
  vcl_cout<<centerX[584]<<"};"<<vcl_endl;

  vcl_cout<<"float centerY = { ";
  for (int i=0; i<584; ++i) {
    vcl_cout<<centerY[i]<<',';
    if (i%10 == 0) vcl_cout<<'\n';
  }
  vcl_cout<<centerY[584]<<"};"<<vcl_endl;

  vcl_cout<<"float centerZ = { ";
  for (int i=0; i<584; ++i) {
    vcl_cout<<centerZ[i]<<',';
    if (i%10 == 0) vcl_cout<<'\n';
  }
  vcl_cout<<centerZ[584]<<"};"<<vcl_endl;
}

static void test_bit_tree()
{
    unsigned char bits[73] = {  1,
                                //{1-8}  depth 1
                                1,1,1,1,1,1,1,1,
                                //DEPTH 2
                                //{9-16}               {17-24}
                                1,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,
                                //{25-32}              {33-40}
                                0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,
                                //{41-48}              {49-56}
                                0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,
                                //{57-64}              {65-72}
                                0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,1};
    unsigned char test_tree[16] = {1,255,1,0,0,0,0,0,0,128,0,0,0,0,0,0};
    boct_bit_tree tree(test_tree,4);
    //---------------------------------------------------------------------------
    //test set and bit_at bits
    //---------------------------------------------------------------------------
    bool good = true;
    for (int i=0; i<73; i++) {
      good = good && (tree.bit_at(i) == bits[i]);
      if (!good)
        vcl_cout<<"i";
    }
    TEST("Set bits and get bits works ", true, good);

  //---------------------------------------------------------------------------
  //test traverse
  //---------------------------------------------------------------------------
  vgl_point_3d<double> o(0.0, 0.0, 0.0);
  int leaf_bit_index = tree.traverse(o);
  TEST("Traverse to origin works ", leaf_bit_index, 73);

  vgl_point_3d<double> m(0.25, 0.25, 0.0);
  int mid_bit_index = tree.traverse(m);
  TEST("Traverse to mid point works ", mid_bit_index, 12);

  vgl_point_3d<double> h(.99, .99, .99);
  int high_bit_index = tree.traverse(h);
  TEST("Traverse to far edge works ", high_bit_index, 584);

  //---------------------------------------------------------------------------
  //test traverse opt
  //---------------------------------------------------------------------------
  int leaf_opt = tree.traverse(o);
  int mid_opt = tree.traverse(m);
  int high_opt = tree.traverse(h);
  vcl_cout<<"origin: "<<leaf_opt<<"  mid: "<<mid_opt<<"  high: "<<high_opt<<vcl_endl;
  good = (leaf_opt == leaf_bit_index) && (mid_opt == mid_bit_index);
  TEST("Traverse opt works ", good, true);

  //---------------------------------------------------------------------------
  //test data_index lookup
  //---------------------------------------------------------------------------
  int origin_data = tree.get_data_index(leaf_bit_index);
  vcl_cout<<origin_data<<vcl_endl;
  TEST("Origin data at 73", origin_data, 73);

  int mid_data = tree.get_data_index(mid_bit_index);
  vcl_cout<<mid_data<<vcl_endl;
  TEST("Mid data @ 12", mid_data, 12);

  int high_data = tree.get_data_index(high_bit_index);
  vcl_cout<<high_data<<vcl_endl;
  TEST("Mid data @ 12", mid_data, 12);
  //---------------------------------------------------------------------------
  //Test size of tree
  //---------------------------------------------------------------------------
  int size = tree.num_cells();
  TEST("Size of tree = 89", size, 89);

  test_print_centers();
}

TESTMAIN(test_bit_tree);
