//:
// \file
// \author Ali Osman Ulusoy
// \date 03-Aug-2012
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>
#include <bstm/bstm_time_block.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void test_block_id()
{
  std::vector<bstm_block_id> list;
  std::map<bstm_block_id, std::string> bmap;

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
  std::map<bstm_block_id, std::string>::iterator iter;
  for (iter = bmap.begin(); iter != bmap.end(); ++iter) {
    bstm_block_id b = (*iter).first;
      std::cout << b <<','<< (*iter).second <<std::endl;
  }
}


void test_block()
{
    test_block_id();
}


TESTMAIN( test_block );
