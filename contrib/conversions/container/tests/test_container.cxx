#include <container/CoolArray_to_CoolList.h>
#include <container/CoolArray_to_vcl_list.h>
#include <container/CoolArray_to_vcl_vector.h>
#include <container/CoolList_to_CoolArray.h>
#include <container/CoolList_to_vcl_list.h>
#include <container/CoolList_to_vcl_vector.h>
#include <container/vcl_list_to_CoolArray.h>
#include <container/vcl_list_to_CoolList.h>
#include <container/vcl_list_to_vcl_vector.h>
#include <container/vcl_vector_to_CoolArray.h>
#include <container/vcl_vector_to_CoolList.h>
#include <container/vcl_vector_to_vcl_list.h>
#include <vnl/vnl_test.h>

void test_container_conversions()
{
  int data[] = { 1, 2, 3, 2, 1, 0, -1 };
  vcl_vector<int> l(data, data+7); 
  CoolArray<int>  a = vcl_vector_to_CoolArray(l);
  vcl_vector<int> b = CoolArray_to_vcl_vector(a);
  vcl_list<int>   c = vcl_vector_to_vcl_list(b);
  CoolList<int>   d = vcl_list_to_CoolList(c);
  vcl_list<int>   e = CoolList_to_vcl_list(d);
  vcl_vector<int> f = vcl_list_to_vcl_vector(e);
  TEST("container conversions", f[0]==1 && f[1]==2 && f[2]==3 && f[3]==2 && f[4]==1 && f[5]==0 && f[6]==-1, true);
}

TESTMAIN(test_container_conversions);
