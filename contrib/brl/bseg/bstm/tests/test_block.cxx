//:
// \file
// \author Ali Osman Ulusoy
// \date 03-Aug-2012
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>
#include <bstm/bstm_time_block.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>

void test_block_id()
{
  vcl_vector<bstm_block_id> list;
  vcl_map<bstm_block_id, vcl_string> bmap;

  bstm_block_id zero(0,0,0,0);
  bstm_block_id one(0,0,0,1);
  bstm_block_id two(0,0,1,0);
  bstm_block_id three(0,0,1,1);
  bstm_block_id four(0,1,0,0);
  bstm_block_id five(0,1,0,1);
  bstm_block_id five2(0,1,0,1);
  bstm_block_id eight(1,0,0,0);
  bstm_block_id nine(1,0,0,1);

  TEST("block_id equal    :      ", true, five==five);
  TEST("block_id !equal   :      ", true, five!=four);
  TEST("block_id less than:      ", true, two < four);
  TEST("block_id less than eq:   ", true, five<=five2);
  TEST("block_id less than eq:   ", true, four<= five);
  TEST("block_id greater than:   ", true, eight > two);
  TEST("block_id greater than eq: ", true, nine >= eight);
  TEST("block_id greater than eq:", true, five2>=five);
  TEST("block_id greater than eq:", true, four >=one);

  bmap[nine] = "nine";
  bmap[zero] = "zero";
  bmap[three] = "three";
  bmap[two] = "two";
  bmap[five] = "five";
  bmap[one] = "one";
  bmap[four] = "four";
  bmap[eight] = "eight";
  vcl_map<bstm_block_id, vcl_string>::iterator iter;
  for (iter = bmap.begin(); iter != bmap.end(); ++iter) {
    bstm_block_id b = (*iter).first;
      vcl_cout << b <<','<< (*iter).second <<vcl_endl;
  }
}


void test_block()
{
    test_block_id();
}


TESTMAIN( test_block );
