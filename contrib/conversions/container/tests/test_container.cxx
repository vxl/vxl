#include <container/vcl_list_to_vcl_vector.h>
#include <container/vcl_vector_to_vcl_list.h>
#include <vnl/vnl_test.h>

void test_container_conversions()
{
  int data[] = { 1, 2, 3, 2, 1, 0, -1 };
  vcl_vector<int> l(data, data+7); 
  vcl_list<int>   a = vcl_vector_to_vcl_list(l);
  TEST("container conversions", *(a.begin()), 1);
  vcl_vector<int> b = vcl_list_to_vcl_vector(a);
  TEST("container conversions", b[0]==1 && b[1]==2 && b[2]==3 && b[3]==2 && b[4]==1 && b[5]==0 && b[6]==-1, true);
}

TESTMAIN(test_container_conversions);
