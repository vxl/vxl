// This is mul/mfpf/tests/test_patch_data.cxx
//=======================================================================
//
//  Copyright: (C) 2009 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mfpf_patch_data

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mfpf/mfpf_patch_data.h>
#include <mfpf/mfpf_add_all_loaders.h>

void test_patch_data()
{
  std::cout << "************************\n"
           << " Testing mfpf_patch_data\n"
           << "************************\n";

  mfpf_add_all_loaders();

  std::istringstream ss1(
          " { name: Test1 min_width: 8 max_width: 10\n"
          " region: mfpf_region_about_lineseg { i0: 5 i1: 31 rel_wi: 0.5 rel_wj: 0.2 form: box }\n"
          " builder: mfpf_norm_corr2d_builder {\n"
          "  search_nA: 5 search_dA: 0.06\n"
          "  search_ns: 3 search_ds: 1.1 overlap_f: 0.75 } }\n");

  mfpf_patch_data patch_data1;
  patch_data1.set_from_stream(ss1);
  std::cout<<patch_data1<<std::endl;

  TEST("name",patch_data1.name(),"Test1");
  TEST("min_width",patch_data1.min_width(),8);
  TEST("max_width",patch_data1.max_width(),10);
  TEST("region",patch_data1.definer().is_a(), "mfpf_region_about_lineseg");
  TEST("builder",patch_data1.builder().is_a(), "mfpf_norm_corr2d_builder");

  std::istringstream ss2(
          " { region: { name: Region1 min_width: 8 max_width: 10\n"
          "             region: mfpf_region_about_lineseg { i0: 5 i1: 31 rel_wi: 0.5 rel_wj: 0.2 form: box }\n"
          "   builder: mfpf_norm_corr2d_builder { } }\n"
          "   region: { name: Region2 min_width: 8 max_width: 10\n"
          "             region: mfpf_region_about_lineseg { i0: 5 i1: 31 rel_wi: 0.5 rel_wj: 0.2 form: box }\n"
          "   builder: mfpf_norm_corr2d_builder { } } }\n");

  std::vector<mfpf_patch_data> patch_list;
  mfpf_read_from_stream(ss2,patch_list);
  TEST("Loaded correct number of items",patch_list.size(),2);
  TEST("First name",patch_list[0].name(),"Region1");
  TEST("Second name",patch_list[1].name(),"Region2");
}

TESTMAIN(test_patch_data);
