#include <iostream>
#include <testlib/testlib_test.h>

#include <boct/boct_bit_tree.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
    std::cout<<" bi : "<<i<<" center: "<<vgl_point_3d<float>(centerX[i], centerY[i], centerZ[i])<<std::endl;
  }

  //print out in an easily copied format.....
  std::cout<<"float centerX = { ";
  for (int i=0; i<584; ++i) {
    std::cout<<centerX[i]<<',';
    if (i%10 == 0) std::cout<<'\n';
  }
  std::cout<<centerX[584]<<"};"<<std::endl;

  std::cout<<"float centerY = { ";
  for (int i=0; i<584; ++i) {
    std::cout<<centerY[i]<<',';
    if (i%10 == 0) std::cout<<'\n';
  }
  std::cout<<centerY[584]<<"};"<<std::endl;

  std::cout<<"float centerZ = { ";
  for (int i=0; i<584; ++i) {
    std::cout<<centerZ[i]<<',';
    if (i%10 == 0) std::cout<<'\n';
  }
  std::cout<<centerZ[584]<<"};"<<std::endl;
}

static void test_bit_tree()
{

  {  // test set bit and parents to true
    boct_bit_tree tree;
    tree.set_bit_and_parents_to_true(17);
    bool good = true;
    for (int i = 0; i < 73; i++) {
      if (i == 17 || i == 2 || i == 0) {
        good &= (tree.bit_at(i) == 1);
      } else {
        good &= (tree.bit_at(i) == 0);
      }
    }
    TEST("Set bits and parents to true", good, true);
  }

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
        std::cout<<"i";
    }
    TEST("Set bits and get bits works ", true, good);

  //---------------------------------------------------------------------------
  //test copy assignment operator
  //---------------------------------------------------------------------------
    {
      {
        // both non-owning
        auto *bits1 = new unsigned char[16]();
        auto *bits2 = new unsigned char[16]();
        boct_bit_tree t1(bits1, false, 4);
        boct_bit_tree t2(bits2, false, 4);
        t1.set_bit_at(4, true);
        t1.set_data_ptr(1);
        t2.set_bit_at(5, true);
        t2.set_data_ptr(2);
        t2 = t1;
        good = t2.bit_at(4) && !t2.bit_at(5) && (t2.get_data_ptr() == 1);
        // This buffer should no longer affect t2.
        bits2[0] = 0xff;
        good &= !t2.bit_at(0);
        // This buffer should.
        bits1[0] = 0xff;
        good &= t2.bit_at(0);
        TEST("TEST copy-assignment between two non-owning trees", good, true);

        delete[] bits1;
        delete[] bits2;
      }
      {
        // both owning
        auto *bits1 = new unsigned char[16]();
        auto *bits2 = new unsigned char[16]();
        boct_bit_tree t1(bits1, true, 4);
        boct_bit_tree t2(bits2, true, 4);
        t1.set_bit_at(4, true);
        t1.set_data_ptr(1);
        t2.set_bit_at(5, true);
        t2.set_data_ptr(2);
        t2 = t1;
        // bits2 is deleted at this point
        good = t2.bit_at(4) && !t2.bit_at(5) && (t2.get_data_ptr() == 1);
        // This buffer should not affect t2 - t2 created a copy.
        bits1[0] = 0xff;
        good &= !t2.bit_at(0);
        TEST("TEST copy-assignment between two owning trees", good, true);
      }

      {
        // only t1 is owning
        auto *bits1 = new unsigned char[16]();
        auto *bits2 = new unsigned char[16]();
        boct_bit_tree t1(bits1, true, 4);
        boct_bit_tree t2(bits2, false, 4);
        t1.set_bit_at(4, true);
        t1.set_data_ptr(1);
        t2.set_bit_at(5, true);
        t2.set_data_ptr(2);
        t2 = t1;
        good = t2.bit_at(4) && !t2.bit_at(5) && (t2.get_data_ptr() == 1);
        // This buffer should no longer affect t2.
        bits2[0] = 0xff;
        good &= !t2.bit_at(0);
        // This buffer also should not - t2 created a copy.
        bits1[0] = 0xff;
        good &= !t2.bit_at(0);
        TEST("TEST tree copy-assignment t2 = t1 when t1 is owning", good, true);
        // Not sure how to test this, but bits2 should still be un-deleted, since t2 was non-owning.
        TEST("TEST old t2's buffer is still valid", boct_bit_tree(bits2).bit_at(5), true);
        delete[] bits2;
      }

      {
        // only t2 is owning
        auto *bits1 = new unsigned char[16]();
        auto *bits2 = new unsigned char[16]();
        boct_bit_tree t1(bits1, false, 4);
        boct_bit_tree t2(bits2, true, 4);
        t1.set_bit_at(4, true);
        t1.set_data_ptr(1);
        t2.set_bit_at(5, true);
        t2.set_data_ptr(2);
        t2 = t1;
        // bits2 deleted at this point
        good = t2.bit_at(4) && !t2.bit_at(5) && (t2.get_data_ptr() == 1);
        // This buffer should affect t2
        bits1[0] = 0xff;
        good &= t2.bit_at(0);
        TEST("TEST tree copy-assignment t2 = t1 when t2 is owning", good, true);
        delete[] bits1;
      }
      {
        // self assignment, owning
        auto *bits1 = new unsigned char[16]();
        boct_bit_tree t1(bits1, true, 4);
        boct_bit_tree t2(bits1, false, 4);
        t1.set_bit_at(4, true);
        t1.set_data_ptr(1);
        t2.set_bit_at(5, true);
        t2.set_data_ptr(2);
        t1 = t2;
        // bits1 not deleted at this point
        good = t2.bit_at(4) && t2.bit_at(5) && (t2.get_data_ptr() == 2);
        // This buffer should affect t2
        bits1[0] = 0xff;
        good &= t2.bit_at(0);
        TEST("TEST tree copy-assignment t2 = t1 when t2 is owning", good, true);
        delete[] bits1;
      }
    }

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
  std::cout<<"origin: "<<leaf_opt<<"  mid: "<<mid_opt<<"  high: "<<high_opt<<std::endl;
  good = (leaf_opt == leaf_bit_index) && (mid_opt == mid_bit_index);
  TEST("Traverse opt works ", good, true);

  //---------------------------------------------------------------------------
  //test data_index lookup
  //---------------------------------------------------------------------------
  int origin_data = tree.get_data_index(leaf_bit_index);
  std::cout<<origin_data<<std::endl;
  TEST("Origin data at 73", origin_data, 73);

  int mid_data = tree.get_data_index(mid_bit_index);
  std::cout<<mid_data<<std::endl;
  TEST("Mid data @ 12", mid_data, 12);

  int high_data = tree.get_data_index(high_bit_index);
  std::cout<<high_data<<std::endl;
  TEST("Mid data @ 12", mid_data, 12);
  //---------------------------------------------------------------------------
  //Test size of tree
  //---------------------------------------------------------------------------
  int size = tree.num_cells();
  TEST("Size of tree = 89", size, 89);

  //---------------------------------------------------------------------------
  // Test structure comparison
  //---------------------------------------------------------------------------
  {
    boct_bit_tree t1, t2, t3;
    t1.set_bit_at(4, true);
    t2.set_bit_at(4, true);
    t3.set_bit_at(4, true);

    t1.set_bit_at(5, true);
    t2.set_bit_at(5, true);

    t1.set_bit_at(11, true);
    t2.set_bit_at(11, true);

    // Totally different data pointers
    t1.set_data_ptr(4);
    t2.set_data_ptr(-1);
    t3.set_data_ptr(4);

    TEST("Space trees with same structure bits",
         boct_bit_tree::same_structure(t1, t2),
         true);
    TEST("Space trees with different structure bits",
         boct_bit_tree::same_structure(t1, t3),
         false);
  }

  test_print_centers();

  {
    // Test  to experiment with get_cell_bits and get_leaf_bits
    boct_bit_tree t1;
    t1.set_bit_and_parents_to_true(70);
    std::cout << "PPP " << t1.get_data_ptr() << std::endl;
    std::cout << "PPP " << t1.max_num_cells() << std::endl;
    std::cout << "PPP " << t1.max_num_inner_cells() << std::endl;
    std::vector<int> cells = t1.get_leaf_bits();
    std::cout << "num_cells() " << t1.num_leaves()
             << " get_leaf_bits(): " << t1.get_leaf_bits().size() << std::endl;
    for (int i = 0; i < cells.size(); ++i) {
      std::cout << "i: " << i << " cells[i]: " << cells[i]
               << " data index: " << t1.get_data_index(cells[i]) << std::endl;
    }
  }
}

TESTMAIN(test_bit_tree);
