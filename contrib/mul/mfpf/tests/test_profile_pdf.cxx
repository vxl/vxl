// This is mul/mfpf/tests/test_profile_pdf.cxx
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mfpf_profile_pdf
//=======================================================================
//
//  Copyright: (C) 2008 The University of Manchester
//
//=======================================================================

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <mfpf/mfpf_add_all_loaders.h>
#include <mfpf/mfpf_profile_pdf.h>
#include <mfpf/mfpf_profile_pdf_builder.h>
#include <vil/vil_bilin_interp.h>
#include <vpdfl/vpdfl_axis_gaussian_builder.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//=======================================================================

void test_profile_pdf_search(mfpf_point_finder_builder& b)
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

  std::cout<<"Value at p0-u: "<<pf->evaluate(image,p0-u,u)<<std::endl
          <<"Value at p0  : "<<pf->evaluate(image,p0,u)<<std::endl
          <<"Value at p0+u: "<<pf->evaluate(image,p0+u,u)<<std::endl;

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

void test_profile_pdf()
{
  std::cout << "**************************\n"
           << " Testing mfpf_profile_pdf\n"
           << "**************************\n";

  mfpf_add_all_loaders();

  mfpf_profile_pdf_builder nc_builder;

  vpdfl_axis_gaussian_builder pdf_builder;
  nc_builder.set(-3,4,pdf_builder);
  test_profile_pdf_search(nc_builder);

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    std::istringstream ss(
          "mfpf_profile_pdf_builder\n"
          "{\n"
          "  ilo: -3 ihi: 4\n"
          "  search_ni: 17\n"
          "  pdf_builder: vpdfl_axis_gaussian_builder { }\n"
          "}\n");

    std::unique_ptr<mfpf_point_finder_builder>
            pf = mfpf_point_finder_builder::create_from_stream(ss);

    TEST("Correct Point Finder Builder", pf->is_a(),"mfpf_profile_pdf_builder");
    if (pf->is_a()=="mfpf_profile_pdf_builder")
    {
      auto &a_pf = static_cast<mfpf_profile_pdf_builder&>(*pf);
      std::cout<<a_pf<<std::endl;
      TEST("search_ni configured",a_pf.search_ni(),17);
      TEST("ilo configured",a_pf.ilo(),-3);
      TEST("ihi configured",a_pf.ihi(),4);
      TEST("pdf builder configured",a_pf.pdf_builder().is_a(),
           "vpdfl_axis_gaussian_builder");
    }
  }

  {
    // Test builder returns correct type of object
    mfpf_profile_pdf_builder b;
    mfpf_point_finder* pf = b.new_finder();
    TEST("Builder: Correct Finder",pf->is_a(),"mfpf_profile_pdf");
    delete pf;
  }

  {
    mfpf_profile_pdf profile_pdf;
    profile_pdf.set_search_area(13,0);

    // Test binary load and save
    mfpf_point_finder * base_ptr = &profile_pdf;

    vsl_b_ofstream bfs_out("test_profile_pdf.bvl.tmp");
    TEST ("Created test_profile_pdf.bvl.tmp for writing", (!bfs_out), false);
    vsl_b_write(bfs_out, profile_pdf);
    vsl_b_write(bfs_out, base_ptr);
    bfs_out.close();

    mfpf_profile_pdf profile_pdf_in;
    mfpf_point_finder *base_ptr_in = nullptr;

    vsl_b_ifstream bfs_in("test_profile_pdf.bvl.tmp");
    TEST ("Opened test_profile_pdf.bvl.tmp for reading", (!bfs_in), false);
    vsl_b_read(bfs_in, profile_pdf_in);
    vsl_b_read(bfs_in, base_ptr_in);
    TEST ("Finished reading file successfully", (!bfs_in), false);
    bfs_in.close();

    TEST("Loaded==Saved",profile_pdf_in,profile_pdf);
    TEST("Load profile_pdf by base ptr (type)",
         base_ptr_in->is_a()==profile_pdf.is_a(),true);

    delete base_ptr_in;
  }

  vsl_delete_all_loaders();
}

TESTMAIN(test_profile_pdf);
