// This is mul/mbl/tests/test_stochastic_data_collector.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Ian Scott
// \brief test vpdfl_pc_gaussian, building, sampling, saving etc.

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vsl/vsl_binary_loader.h>
#include <vnl/io/vnl_io_vector.h>
#include <vul/vul_printf.h>
#include <mbl/mbl_stochastic_data_collector.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//=======================================================================

void configure()
{
 vsl_add_to_binary_loader(mbl_stochastic_data_collector<vnl_vector<double> >());
}

//=======================================================================


//: The main control program
void test_stochastic_data_collector()
{
  vcl_cout << "***************************************\n"
           << " Testing mbl_stochastic_data_collector\n"
           << "***************************************\n";

  configure();

  mbl_stochastic_data_collector<vnl_vector<double> > collector(100);
  collector.reseed(14545);

  vcl_cout << "===========Generate data\n";
  vcl_vector<unsigned> hist(10, 0u);
  const int n_expts = 50;
  for (int i=0;i<n_expts;++i)
  {
    vnl_vector<double> v(1);

    collector.clear();
    for (v(0) = 0.0; v(0) < 5000.0; v(0) += 1.0)
    {
      if (collector.store_next()) collector.force_record(v);
    }
//    collector.record(v);

    mbl_data_wrapper<vnl_vector<double> > &data = collector.data_wrapper();
    data.reset();
    do
    {
      v = data.current();
      hist[((int)(v(0))) / 500] ++;
    }
    while (data.next());
  }

  vcl_cout << "Histogram output, over " << n_expts << "experiments\n";
  for (int i=0; i < 10; i++)
    vul_printf(vcl_cout, "From %4d to %4d there were on average %4f items stored.\n",
               i * 500, i*500 + 499, ((double)hist[i])/((double)n_expts))  ;

  unsigned correct_hist[] = {501, 543, 499, 495, 461, 539, 490, 515, 460, 497};

  TEST("Found correct values",
       vnl_c_vector<unsigned>::euclid_dist_sq(&hist[0], correct_hist, 10), 0);

  vcl_cout << "=========Testing IO\n";

  mbl_stochastic_data_collector<vnl_vector<double> > collector2;
  mbl_data_collector_base *collector3=0;

  vcl_string path = "test_stochastic_data_collector.bvl.tmp";
  vcl_cout<<"Saving : "<<collector<<'\n';
  vsl_b_ofstream bfs_out(path);
  TEST(("Opened " + path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_write(bfs_out,collector);
  vsl_b_write(bfs_out,(mbl_data_collector_base*)&collector);
  bfs_out.close();

  vsl_b_ifstream bfs_in(path);
  TEST(("Opened " + path + " for reading").c_str(), (!bfs_in ), false);
  vsl_b_read(bfs_in,collector2);
  vsl_b_read(bfs_in,collector3);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(path.c_str());
#endif

  vcl_cout << "Loaded : " << collector2 << '\n';

  TEST("Loaded collector size = saved collector size",
       collector.data_wrapper().size(), collector2.data_wrapper().size());

  mbl_data_wrapper<vnl_vector<double> > &w1 = collector.data_wrapper();
  mbl_data_wrapper<vnl_vector<double> > &w2 = collector2.data_wrapper();
  w1.reset();
  w2.reset();
  bool test_res = true;
  do
  {
    if (w1.current() != w2.current())
      test_res = false;
    w1.next();
  } while (w2.next());
  TEST("Loaded collector = saved collector", test_res, true);

  vcl_cout << "Loaded by pointer: "<<collector3<<'\n';
  delete collector3;

  vsl_delete_all_loaders();
}


TESTMAIN(test_stochastic_data_collector);
