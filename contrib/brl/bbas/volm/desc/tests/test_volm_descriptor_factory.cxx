// This is contrib/brl/bbas/volm/desc/tests/test_volm_descriptor.cxx
//:
// \file
// \brief Tests for volm_descriptor_factory
// \author Yi Dong
// \date   May 28, 2013
//
#include <testlib/testlib_test.h>
#include <volm/desc/volm_descriptor_factory.h>
#include <volm/desc/volm_existance_descriptor_factory.h>
#include <volm/desc/volm_upcount_descriptor_factory.h>
#include <vnl/vnl_random.h>

static void test_volm_descriptor_factory()
{
  // define the basic depth/height interval for the descriptor
  vcl_vector<double> radius;
  radius.push_back(1000);  radius.push_back(500);  radius.push_back(2000);
  vcl_vector<double> height;
  
  // define an object
  volm_object ob(2231.0, 6, depth_map_region::FRONT_PARALLEL, 2);
  vcl_cout << "Define an object: ";
  ob.print();

  // create an descriptor for existance of the objects
  volm_descriptor_factory_sptr vd_exist = volm_descriptor_factory::create_descriptor("existance");
  vd_exist->create(radius, height);
  for (unsigned i = 30; i < 100; i++)
    vd_exist->set_count(i,2);
  unsigned bin_idx = vd_exist->bin_index(ob);
  vcl_cout << "For defined object, associated bin is " << bin_idx << vcl_endl;
  
  // set the bin value associated with the object
  vd_exist->set_count(ob, 5);

  vd_exist->print();
  vd_exist->visualize("./test_exist.svg");

  // create an descriptor for upcount of the objects
  volm_descriptor_factory_sptr vd_upcount = volm_descriptor_factory::create_descriptor("upcount");
  vd_upcount->create(radius, height);
  for (unsigned i = 30; i < 100; i++)
    vd_upcount->set_count(i, 2);
  for (unsigned i = 30; i < 200; i++)
    vd_upcount->set_count(i, 3);
  // set the bin value associate with the object
  vd_upcount->set_count(ob, 5);
  vd_upcount->set_count(ob, 2);
  vd_upcount->print();
  vd_upcount->visualize("./test_upcount.svg");


  
  // binary IO test
  vsl_b_ofstream os("./volm_existance_descriptor.bin");
  vsl_b_write(os, vd_exist);
  os.close();
  vsl_b_ifstream is("./volm_existance_descriptor.bin");
  volm_descriptor_factory_sptr vd_exist_in = new volm_existance_descriptor_factory();
  vsl_b_read(is, vd_exist_in);
  vd_exist_in->visualize("./test_exist_readin.svg");
  is.close();

  vsl_b_ofstream os_upcount("./volm_upcount_descriptor.bin");
  vsl_b_write(os_upcount, vd_upcount);
  os_upcount.close();
  vsl_b_ifstream is_upcount("./volm_upcount_descriptor.bin");
  volm_descriptor_factory_sptr vd_upcount_in = new volm_upcount_descriptor_factory();
  vsl_b_read(is_upcount, vd_upcount_in);
  is_upcount.close();
  vd_upcount_in->visualize("./test_upcount_readin.svg");

  TEST("Given object attributes, return correct descriptor bin index", bin_idx, 904);
  TEST("The bin value in existance histogram for the object", (*vd_exist)[904], 5.0);
  TEST("The bin value in upcount histogram for the object", (*vd_upcount)[904], 7.0);
  TEST("Binary IO test", (vd_upcount->name() == vd_upcount_in->name() &&
                          vd_exist->name() == vd_exist_in->name() &&
                          (*vd_exist)[904] == (*vd_exist_in)[904] &&
                          (*vd_upcount)[904] == (*vd_upcount_in)[904]), true); 

}

TESTMAIN( test_volm_descriptor_factory );
