// This is mul/mfpf/tests/test_edge_finder.cxx
//=======================================================================
//
//  Copyright: (C) 2007 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mfpf_edge_finder

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mfpf/mfpf_add_all_loaders.h>
#include <mfpf/mfpf_edge_finder.h>
#include <mfpf/mfpf_edge_finder_builder.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vil/vil_bilin_interp.h>

//=======================================================================

void test_edge_finder_search(mfpf_point_finder_builder& b)
{
  std::cout<<"Testing building and search."<<std::endl;

  mfpf_point_finder* pf = b.new_finder();

  // Create a test image
  vimt_image_2d_of<float> image(20,10);
  image.image().fill(0);

  // Fill half of image
  for (int y=0;y<10;++y)
    for (int x=0;x<10;++x)
      image.image()(x,y)=99;

  vgl_point_2d<double> p0(9.5,5), p1(7.5,5);
  vgl_vector_2d<double> u(1,0);

  b.clear(1);
  b.add_example(image,p0,u);
  b.build(*pf);

  std::cout<<"Built model: "<<pf<<std::endl;

  vgl_point_2d<double> new_p;
  vgl_vector_2d<double> new_u;

  pf->set_search_area(3,0);

  pf->search(image,p1,u,new_p,new_u);
  std::cout<<"Found point: "<<new_p<<std::endl;

  TEST_NEAR("Correct orientation",(new_u-u).length(),0.0,1e-6);
  TEST_NEAR("Correct location",(new_p-p0).length(),0.0,1e-6);

  vimt_image_2d_of<double> response;
  pf->evaluate_region(image,p1,u,response);
  TEST("Response ni",response.image().ni(),7);
  TEST("Response nj",response.image().nj(),1);
  std::cout<<"World2im: "<<response.world2im()<<std::endl;

  // Check that response has local minima in correct place
  vgl_point_2d<double> ip = response.world2im()(new_p);
  TEST("Best pt in image (i)",
       ip.x()>=0 && ip.x()<response.image().ni(),true);
  TEST_NEAR("Best pt in image (j)",ip.y(),0,1e-6);

  double r0 = vil_bilin_interp_safe(response.image(),ip.x(),ip.y());
  double r1 = vil_bilin_interp_safe(response.image(),ip.x()-1,ip.y());
  double r2 = vil_bilin_interp_safe(response.image(),ip.x()+1,ip.y());
  std::cout<<r0<<','<<r1<<','<<r2<<std::endl;
  TEST("Local minima 1",r0<r1,true);
  TEST("Local minima 2",r0<r2,true);

  delete pf;
}

void test_edge_finder()
{
  std::cout << "***********************\n"
           << " Testing mfpf_edge_finder\n"
           << "***********************\n";

  vsl_add_to_binary_loader(mfpf_edge_finder());

  mfpf_edge_finder_builder edge_builder;
  test_edge_finder_search(edge_builder);

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_edge_finder_builder());

    std::istringstream ss(
          "mfpf_edge_finder_builder\n"
          "{\n"
          "  search_ni: 17\n"
          "}\n");

    std::unique_ptr<mfpf_point_finder_builder>
            pfb = mfpf_point_finder_builder::create_from_stream(ss);

    TEST("Correct Builder",pfb->is_a(),"mfpf_edge_finder_builder");
    if (pfb->is_a()=="mfpf_edge_finder_builder")
    {
      auto &a_pfb = static_cast<mfpf_edge_finder_builder&>(*pfb);
      std::cout<<a_pfb<<std::endl;
      TEST("search_ni configured",a_pfb.search_ni(),17);
    }
  }

  {
    // Test builder returns correct type of object
    mfpf_edge_finder_builder b;
    mfpf_point_finder* pf = b.new_finder();
    TEST("Builder: Correct Finder",pf->is_a(),"mfpf_edge_finder");
    delete pf;
  }

  {
    mfpf_edge_finder edge_finder;
    edge_finder.set_search_area(13,0);

    // Test binary load and save
    mfpf_point_finder * base_ptr = &edge_finder;

    vsl_b_ofstream bfs_out("test_edge_finder.bvl.tmp");
    TEST ("Created test_edge_finder.bvl.tmp for writing",
          (!bfs_out), false);
    vsl_b_write(bfs_out, edge_finder);
    vsl_b_write(bfs_out, base_ptr);
    bfs_out.close();

    mfpf_edge_finder edge_finder_in;
    mfpf_point_finder *base_ptr_in = nullptr;

    vsl_b_ifstream bfs_in("test_edge_finder.bvl.tmp");
    TEST ("Opened test_edge_finder.bvl.tmp for reading",
          (!bfs_in), false);
    vsl_b_read(bfs_in, edge_finder_in);
    vsl_b_read(bfs_in, base_ptr_in);
    TEST ("Finished reading file successfully", (!bfs_in), false);
    bfs_in.close();

    TEST("Loaded: search_ni",
         edge_finder_in.search_ni(),edge_finder.search_ni());
    TEST("Load edge_finder by base ptr (type)",
         base_ptr_in->is_a()==edge_finder.is_a(),true);

    delete base_ptr_in;
  }

  vsl_delete_all_loaders();
}

TESTMAIN(test_edge_finder);
