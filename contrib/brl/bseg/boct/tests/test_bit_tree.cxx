#include <testlib/testlib_test.h>

#include <boct/boct_bit_tree.h>
#include <vnl/vnl_random.h>


static void test_bit_tree()
{
  
  char test_tree[73] = {  //{0} root depth 0
                          1,
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
                          0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,1
                        };
  
  boct_bit_tree tree(test_tree); 
  
  //test set and bit_at bits
  bool good = true;
  for(int i=0; i<73; i++) {
    good = good && (tree.bit_at(i) == test_tree[i]);
  }
  TEST("Set bits and get bits works ", true, good);

  //test traverse
  vgl_point_3d<double> o(0.0, 0.0, 0.0);
  int leaf_bit_index = tree.traverse(o);
  TEST("Traverse to origin works ", leaf_bit_index, 73);
  
  vgl_point_3d<double> m(0.25, 0.25, 0.0);
  int mid_bit_index = tree.traverse(m);
  TEST("Traverse to mid point works ", mid_bit_index, 12);

  //test data_index lookup
  int origin_data = tree.get_data_index(leaf_bit_index);
  TEST("Origin data at 73", origin_data, 73);

  int mid_data = tree.get_data_index(mid_bit_index);
  TEST("Mid data @ 12", mid_data, 12);
  
  //Test size of tree
  int size = tree.size();
  TEST("Size of tree = 89", size, 89);

}

TESTMAIN(test_bit_tree);
