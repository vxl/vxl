#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include "mbl/mbl_index_sort.h"
#include <vnl/vnl_test.h>
#include <vcl_map.h>

void test_index_sort()
{
  vcl_cout << "**********************\n"
           << "Testing mbl_index_sort\n"
           << "**********************\n";

  const int n = 20;
  vcl_vector<double> x(n);
  vcl_vector<int> index;

  for (int i=0;i<n;++i) x[i]=n-i;

  mbl_index_sort(x,index);

  TEST("Correct number of elements",index.size()==n,true);

  bool order_ok = true;
  for (int i=1;i<n;++i)
    if (x[index[i]]<x[index[i-1]]) order_ok=false;

  TEST("First element",index[0],  n-1);
  TEST("Last element", index[n-1],0);

  TEST("Order correct",order_ok,true);

  for (int i=0;i<n;++i) x[i]=(i+3)%n;

  mbl_index_sort(x,index);

  TEST("Correct number of elements",index.size()==n,true);

  order_ok = true;
  for (int i=1;i<n;++i)
    if (x[index[i]]<x[index[i-1]]) order_ok=false;

  TEST("First element",index[0],  n-3);
  TEST("Last element", index[n-1],n-4);

  TEST("Order correct",order_ok,true);

  vcl_vector<unsigned> index2(n/2);
  for (unsigned i=0;i<n/2;++i) index2[i] = i*2;

  vcl_sort(index2.begin(), index2.end(),
    mbl_index_sort_cmp<double, unsigned, vcl_vector<double> >(x));
  TEST("Correct number of elements",index2.size()==n/2,true);

  order_ok = true;
  for (int i=1;i<n/2;++i)
    if (x[index2[i]]<x[index2[i-1]]) order_ok=false;

  TEST("First element",index2[0],  n-2);
  TEST("Last element", index2[n/2-1],n-4);

  TEST("Order correct",order_ok,true);
}

TESTMAIN(test_index_sort);
