#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vbl/vbl_test.h>
#include <vbl/io/vbl_io_sparse_array.h>


void test_sparse_array_io()
{
  vcl_cout << "***********************************" << vcl_endl;
  vcl_cout << "Testing vbl_sparse_array<double> io" << vcl_endl;
  vcl_cout << "***********************************" << vcl_endl;  

  vbl_sparse_array<double> v_out, v_in;
  unsigned key1=3,key2=4,key3=5;
  double data1=1.2, data2=3.4, data3=5.6;

  //create a sparse array - more than 5 elements so only 
  // the first 5 are written out
  v_out[key1]=data1;
  v_out[key2]=data2;
  v_out[key3]=data3;
  for (unsigned k=60; k<70; k++)
    v_out[k]=data1;

  vsl_b_ofstream bfs_out("vbl_sparse_array_test_io.bvl.tmp");
  TEST ("Created vbl_sparse_array_test_io.bvl.tmp for writing", 
    (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vbl_sparse_array_test_io.bvl.tmp");
  TEST ("Opened vbl_sparse_array_test_io.bvl.tmp for reading", 
    (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  bfs_in.close();
  
  bool test_result = true;
  //same number of non zero elements?
  if(v_out.count_nonempty() != v_in.count_nonempty())
    test_result=false;
  else {
    //check every key/data pair, require same order too.
  vbl_sparse_array<double>::const_iterator s = v_in.begin();
  vbl_sparse_array<double>::const_iterator r;
  //N.B. relies on sensible == operator for <T> 
  for(r = v_out.begin(); r != v_out.end(); ++r){
    if(((*s).first != (*r).first) || ((*s).second != (*r).second)) 
      test_result=false;
    s++;
  }
  }
  TEST ("v_out == v_in",test_result, true);

  vsl_print_summary(vcl_cout, v_in);
  vcl_cout << vcl_endl;
}
   
void test_sparse_array_prime()
{
  test_sparse_array_io();
}


TESTMAIN(test_sparse_array_prime);
