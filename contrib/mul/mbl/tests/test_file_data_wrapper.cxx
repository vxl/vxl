// This is mul/mbl/tests/test_file_data_wrapper.cxx
#include <testlib/testlib_test.h>

#include <mbl/mbl_file_data_collector.h>
#include <vnl/vnl_double_2.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h> // vpl_unlink()

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

void test_file_data_wrapper()
{
  vcl_cout << "\n*********************************\n"
           <<   " Testing mbl_file_data_collector\n"
           <<   "*********************************\n";

  // create data
  vnl_double_2 v1( -3.0, 4.0);
  vnl_double_2 v2(  8.0,-7.0);
  vnl_double_2 v3(-40.0,10.0);

  vcl_cout<<"v1= "<<v1<<vcl_endl
          <<"v2= "<<v2<<vcl_endl
          <<"v3= "<<v3<<vcl_endl;

  // collect data using mbl_file_data_collector
  vcl_string path= "test_file_data_wrapper.bvl.tmp";

  mbl_file_data_collector< vnl_vector<double> > collector(path);
  collector.record(v1);
  collector.record(v2);

  // read in data using mbl_file_data_wrapper
  mbl_data_wrapper<vnl_vector<double> >& wrapper =collector.data_wrapper();

  // test size of data
  vcl_cout<<"wrapper.size()= "<<wrapper.size()<<vcl_endl;
  TEST("Size of data",wrapper.size(),2);

  vcl_cout<<"first element= "<<wrapper.current()<<vcl_endl;
  TEST("First element",wrapper.current(),v1);

  wrapper.next();
  vcl_cout<<"second element= "<<wrapper.current()<<vcl_endl;
  TEST("Second element",wrapper.current(),v2);

  wrapper.next();
  vcl_cout<<"current element= "<<wrapper.current()<<vcl_endl;
  TEST("Back to start,first element",wrapper.current(),v1);

  // test set_index method (inherited from mbl_data_wrapper)
  wrapper.set_index(1);
  vcl_cout<<"current element= "<<wrapper.current()<<vcl_endl;
  TEST("set index=1 ie second element",wrapper.current(),v2);

  wrapper.set_index(0);
  vcl_cout<<"current element= "<<wrapper.current()<<vcl_endl;
  TEST("set index=0 ie first element",wrapper.current(),v1);

  // try to get new wrapper should just return old on, but reset it!
  mbl_data_wrapper<vnl_vector<double> >& wrapper2 =collector.data_wrapper();
  vcl_cout<<"current element= "<<wrapper.current()<<vcl_endl;
  TEST("testing wrapper2, should be set to first element",wrapper2.current(),v1);

  // record more data then get new wrapper
  // can't really have more than one wrapper, so just get reference to old one!
  collector.record(v3);
  mbl_data_wrapper<vnl_vector<double> >& wrapper3 =collector.data_wrapper();
  vcl_cout<<"wrapper3.size()= "<<wrapper3.size()<<vcl_endl;
  TEST("wrapper3 should still be same size, even though data file extended",
       wrapper3.size(),2);

  wrapper3.next();
  wrapper3.next();
  vcl_cout<<"current element= "<<wrapper.current()<<vcl_endl;
  TEST("testing wrapper3, should be set to first element",wrapper3.current(),v1);

  collector.delete_stuff();

#if !LEAVE_FILES_BEHIND
  vpl_unlink(path.c_str());
#endif
}

TESTMAIN(test_file_data_wrapper);
