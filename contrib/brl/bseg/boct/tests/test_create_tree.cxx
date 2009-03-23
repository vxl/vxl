
#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>



MAIN( test_create_tree )
{
  START ("CREATE TREE");
  short nlevels=10;
  boct_tree * block=new boct_tree(nlevels);
  TEST("No of Max levels of tree",nlevels, block->numlevels());
  SUMMARY();

  
}
