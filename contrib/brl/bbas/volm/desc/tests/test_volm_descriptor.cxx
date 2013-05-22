// This is contrib/brl/bbas/volm/desc/tests/test_volm_descriptor.cxx
//:
// \file
// \brief Tests for volm_descriptor
// \author Yi Dong
// \date   May 16, 2013
//
#include <testlib/testlib_test.h>
#include <volm/desc/volm_descriptor.h>
#include <volm/desc/volm_descriptor_sptr.h>
#include <vnl/vnl_random.h>

static void test_volm_descriptor()
{

  // create a volm_descriptor
  vcl_vector<double> radius;
  radius.push_back(1000);  radius.push_back(500);  radius.push_back(2000);

  vcl_vector<double> height;
  //height.push_back(10);  height.push_back(20);  height.push_back(5);

  volm_descriptor_sptr vd = new volm_descriptor(radius, height);

  vnl_random values(9667566);
  for (unsigned i = 0; i < vd->nbins(); i++)
    vd->set_count(i, (unsigned char)(values.lrand32(1)));

  vd->print();

  // define an object
  volm_object ob(2231.0, 6, depth_map_region::FRONT_PARALLEL, 2);
  vcl_cout << "Define an object: ";
  ob.print();
  // test the bin_index method
  unsigned bin_idx = vd->bin_index(ob);
  vcl_cout << "For defined object, associated bin is " << bin_idx << vcl_endl;
  TEST("Given object attributes, return correct descriptor bin index", bin_idx, 1109);
  
  // test the set_count method and operate []
  vd->set_count(bin_idx, 1);
  vcl_cout << "Set the value of this object bin to vd[" << bin_idx << "] = " << (int)(*vd)[bin_idx] << vcl_endl;
  TEST("Set the descriptor value for given object types", (*vd)[bin_idx], 1);

  // test the upcount method
  vd->upcount(ob, 3);
  vcl_cout << "Upcount the value of this object bin by 31, give vd[" << bin_idx << "] = " << (int)(*vd)[bin_idx] << vcl_endl;
  TEST("Upcount the descriptor value for given object types", (*vd)[bin_idx], 4);

  // binary io test
  vsl_b_ofstream os("./volm_descriptor.bin");
  vsl_b_write(os, vd);
  os.close();

  vsl_b_ifstream is("./volm_descriptor.bin");
  volm_descriptor_sptr vd_in = new volm_descriptor;
  vsl_b_read(is, vd_in);
  is.close();
  vcl_cout << "Loaded volm_descriptor has value vd_in[" << bin_idx << "] = " << (int)((*vd_in)[bin_idx]) << vcl_endl;
  bool is_good = true;
  is_good = is_good && 
            ( vd_in->nbins() == vd->nbins() ) &&
            ( vd_in->ndepths() == vd->ndepths() ) &&
            ( vd_in->nheights() == vd->nheights() ) && 
            ( vd_in->norients() == vd->norients() ) &&
            ( vd_in->nlands() == vd->nlands() );

  if (is_good) {
    for (unsigned r_idx = 0; r_idx < vd->radius().size(); r_idx++) 
      is_good = is_good && (vd_in->radius()[r_idx] == vd->radius()[r_idx]);
    for (unsigned h_idx = 0; h_idx < vd->height().size(); h_idx++)
      is_good = is_good && (vd_in->height()[h_idx] == vd->height()[h_idx]);
    for (unsigned bin_id = 0; bin_id < vd->nbins(); bin_id++)
      is_good = is_good && ((*vd_in)[bin_id] == (*vd)[bin_id]); 
  }
  TEST("binary io test", is_good, true);
  vcl_cout << "Loaded volm_descriptor: \n";
  vd_in->print();

  // visualize the histogram
  vd_in->visualize("./test.svg", 5);
}

TESTMAIN( test_volm_descriptor );
