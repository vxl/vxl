#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <mbl/mbl_index_sort.h>

void test_index_sort()
{
  vcl_cout << "**********************" << vcl_endl;
  vcl_cout << " Testing mbl_index_sort " << vcl_endl;
  vcl_cout << "**********************" << vcl_endl;

	int n = 20;
  vcl_vector<double> x(n);
	vcl_vector<int> index;

	for (int i=0;i<n;++i) x[i]=n-i;

	mbl_index_sort(x,index);

	TEST("Correct number of elements",index.size()==n,true);

	bool order_ok = true;
	for (int i=1;i<n;++i)
	  if (x[index[i]]<x[index[i-1]]) order_ok=false;

  TEST("First element",index[0]==(n-1),true);
  TEST("Last element",index[n-1]==0,true);

  TEST("Order correct",order_ok,true);

	for (int i=0;i<n;++i) x[i]=(i+3)%n;

	mbl_index_sort(x,index);

	TEST("Correct number of elements",index.size()==n,true);

	order_ok = true;
	for (int i=1;i<n;++i)
	  if (x[index[i]]<x[index[i-1]]) order_ok=false;

  TEST("Order correct",order_ok,true);


}

TESTMAIN(test_index_sort);
