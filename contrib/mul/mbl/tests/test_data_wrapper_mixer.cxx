// This is mul/mbl/tests/test_data_wrapper_mixer.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <mbl/mbl_data_wrapper_mixer.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vnl/vnl_vector.h>
#include <testlib/testlib_test.h>

void test_data_wrapper_pair_mixer(unsigned n1, unsigned n2)
{
  vcl_cout<<"Testing mix of wrappers of size "<<n1<<" and "<<n2<<vcl_endl;
  vcl_vector<vnl_vector<double> > data1(n1);
  vcl_vector<vnl_vector<double> > data2(n2);

  for (unsigned i=0;i<n1;++i)
  {
    data1[i].set_size(1);
    data1[i][0] = i;
  }
  for (unsigned i=0;i<n2;++i)
  {
    data2[i].set_size(1);
    data2[i][0] = i+n1;
  }

  mbl_data_array_wrapper<vnl_vector<double> > w1(data1);
  mbl_data_array_wrapper<vnl_vector<double> > w2(data2);

  vcl_vector<mbl_data_wrapper<vnl_vector<double> >*> wrappers(2);
  wrappers[0] = &w1;
  wrappers[1] = &w2;

  mbl_data_wrapper_mixer<vnl_vector<double> > mixer(wrappers);

  TEST("Size",mixer.size(),(n1+n2));
  TEST("Initial index",mixer.index(),0);
  mixer.next();
  TEST("Second element",vcl_fabs(mixer.current()[0]-1.0)<1e-6,true);
  mixer.next();
  TEST("Third element",vcl_fabs(mixer.current()[0]-2.0)<1e-6,true);

  if (n2>0)
  {
    mixer.set_index(n1+1);
    TEST("n+1-th element",vcl_fabs(mixer.current()[0]-n1-1)<1e-6,true);
  }

  unsigned n=1;
  mixer.reset();
  while (mixer.next()) ++n;
  TEST("Correct number of valid elements in loop",n,n1+n2);
}

void test_data_wrapper_mixer()
{
  vcl_cout << "********************************\n"
           << " Testing mbl_data_wrapper_mixer\n"
           << "********************************\n";

  test_data_wrapper_pair_mixer(3,7);
  test_data_wrapper_pair_mixer(1,7);
  vcl_cout<<"Testing empty data\n";
  test_data_wrapper_pair_mixer(5,0);
  test_data_wrapper_pair_mixer(0,5);
}

TESTMAIN(test_data_wrapper_mixer);
