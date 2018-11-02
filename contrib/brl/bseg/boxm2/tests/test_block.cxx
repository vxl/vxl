//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "test_utils.h"

#include <boxm2/boxm2_block.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <vnl/vnl_vector_fixed.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void test_block_id()
{
    std::vector<boxm2_block_id> list;
    std::map<boxm2_block_id, std::string> bmap;

    boxm2_block_id zero(0,0,0);
    boxm2_block_id one(0,0,1);
    boxm2_block_id two(0,1,0);
    boxm2_block_id three(0,1,1);
    boxm2_block_id four(1,0,0);
    boxm2_block_id five(1,0,1);
    boxm2_block_id five2(1,0,1);

    list.push_back(three);
    list.push_back(two);
    list.push_back(five);
    list.push_back(zero);
    list.push_back(four);
    list.push_back(one);

    TEST("block_id equal    :      ", true, five==five);
    TEST("block_id !equal   :      ", true, five!=four);
    TEST("block_id less than:      ", true, two < four);
    TEST("block_id less than eq:   ", true, five<=five2);
    TEST("block_id less than eq:   ", true, four<= five);
    TEST("block_id greater than:   ", true, five > two);
    TEST("block_id greater than eq:", true, five2>=five);
    TEST("block_id greater than eq:", true, four >=one);

    bmap[zero] = "zero";
    bmap[three] = "three";
    bmap[two] = "two";
    bmap[five] = "five";
    bmap[one] = "one";
    bmap[four] = "four";
    std::map<boxm2_block_id, std::string>::iterator iter;
    for (iter = bmap.begin(); iter != bmap.end(); ++iter) {
        boxm2_block_id b = (*iter).first;
        std::cout << b <<','<< (*iter).second <<std::endl;
    }
}

void test_block()
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    typedef vnl_vector_fixed<unsigned short, 2> ushort2;

    //test xml file
    std::string root_dir  = testlib_root_dir();
    std::string test_file = root_dir + "/contrib/brl/bseg/boxm2/tests/";

    //construct block from stream
    int nums[4] = { 64, 64, 64, 0 };
    double dims[4] = { 0.5, 0.5, 0.5, 0.0 };
    int numBuffers = 64;
    int treeLen    = 64*64*64;
    int init_level = 1;
    int max_level  = 4;
    int max_mb     = 400;
    vgl_point_3d<double> local_origin = vgl_point_3d<double>(0,0,0) + vgl_vector_3d<double>((double)nums[0]*dims[0]*(double)0,
                                                                                            (double)nums[0]*dims[0]*(double)0,
                                                                                            (double)nums[0]*dims[0]*(double)0);
    boxm2_block_id id(0,0,0);
    boxm2_block_metadata mdata(id,
                                local_origin,
                                vgl_vector_3d<double> (dims[0], dims[1],dims[2]),
                                vgl_vector_3d<unsigned>(nums[0],nums[1],nums[2]),
                                init_level,
                                max_level,
                                max_mb,
                                0.001,
                                2);

    char* stream = boxm2_test_utils::construct_block_test_stream( numBuffers,
                                                                  treeLen,
                                                                  nums,
                                                                  dims,
                                                                  init_level,
                                                                  max_level,
                                                                  max_mb );
    boxm2_block test_block(id, mdata,stream);


    //make sure all the meta data matches...
    if (test_block.block_id() != id) {
      TEST("boxm2_block: id failed", true, false);
      return;
    }

    if (test_block.tree_buff_length() != treeLen) {
        TEST("boxm2_block: tree_buff_length failed", true, false);
        return;
    }
    if (test_block.init_level() != init_level) {
        TEST("boxm2_block: init_level failed", true, false);
        return;
    }
    if (test_block.max_level() != max_level) {
        TEST("boxm2_block: max_level failed", true, false);
        return;
    }
    if (test_block.max_mb() != max_mb) {
        TEST("boxm2_block: max_mb failed", true, false);
        return;
    }
    if (test_block.sub_block_dim() != vgl_vector_3d<double>(dims[0], dims[1], dims[2])) {
        TEST("boxm2_block: sub_block_dim failed", true, false);
        return;
    }
    if (test_block.sub_block_num() != vgl_vector_3d<unsigned>(nums[0], nums[1], nums[2])) {
        TEST("boxm2_block: sub_block_num failed", true, false);
        return;
    }
    TEST("boxm2_block: meta data passed", true, true);

    const boxm2_array_3d<uchar16>&  trees = test_block.trees();
    uchar16 comp((unsigned char) 0);
    for (int i=0; i<nums[0]; i++) {
      for (int j=0; j<nums[1]; j++) {
        for (int k=0; k<nums[2]; k++) {
          if (trees[i][j][k][0] != (unsigned char) 0) {
            TEST("boxm2_block: trees not initialized properly", true, false);
            return;
          }
        }
      }
    }
    TEST("boxm2_block: trees initialized properly", true, true);

    // test various location and data index functions
    vgl_box_3d<double> bbox = test_block.bounding_box_global();
    bool good = true;
    unsigned indx, dpth;
    double side_length;
    vgl_point_3d<double> g_pt(0.51, 0.51, 0.51);
        good = good && bbox.contains(g_pt);
    vgl_point_3d<int> l_pt;
    good = good && test_block.contains(g_pt, l_pt);
    good = good && test_block.data_index(g_pt, indx, dpth, side_length);
    good = good && indx == 16656;
    good = good && dpth == 0;
    good = good && side_length == dims[0];
         TEST("boxm2_block: bounding box, contains and index", good, true);
    //clean up file left behind "tests/block_id.0.0.0.bin"
    test_block_id();
}


TESTMAIN( test_block );
