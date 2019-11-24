// This is mul/mfpf/tests/test_region_pdf.cxx
#include <iostream>
#include <sstream>
#include "testlib/testlib_test.h"
//:
// \file
// \author Tim Cootes
// \brief test mfpf_region_pdf
//=======================================================================
//
//  Copyright: (C) 2008 The University of Manchester
//
//=======================================================================

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vsl/vsl_binary_loader.h"
#include <mfpf/mfpf_add_all_loaders.h>
#include <mfpf/mfpf_region_pdf.h>
#include <mfpf/mfpf_region_pdf_builder.h>
#include "vil/vil_bilin_interp.h"
#include <vpdfl/vpdfl_axis_gaussian_builder.h>
#include "vgl/vgl_point_2d.h"
#include "vgl/vgl_vector_2d.h"

//=======================================================================

void test_region_pdf_search(mfpf_point_finder_builder& b)
{
  std::cout<<"Testing building and search."<<std::endl;

  mfpf_point_finder* pf = b.new_finder();

  // Create a test image
  vimt_image_2d_of<float> image(20,20);
  image.image().fill(0);

  // Add a point in the centre
  image.image()(10,10)=99;

  vgl_point_2d<double> p0(9.5,9.5), p1(7.5,8.5);
  vgl_vector_2d<double> u(1,0);

  b.clear(1);
  b.add_example(image,p0,u);
  b.build(*pf);

  std::cout<<"Built model: "<<pf<<std::endl;

  vgl_point_2d<double> new_p;
  vgl_vector_2d<double> new_u;

  std::cout<<"Value at true point ="<<pf->evaluate(image,p0,u)<<std::endl;

  pf->set_search_area(0,0);
  pf->search(image,p0,u,new_p,new_u);
  std::cout<<"search(): Found point: "<<new_p<<std::endl;
  TEST_NEAR("Correct location",(new_p-p0).length(),0.0,1e-6);

  pf->set_search_area(3,3);
  pf->search(image,p1,u,new_p,new_u);
  std::cout<<"Found point: "<<new_p<<std::endl
          <<"Should be : "<<p0<<std::endl;

  TEST_NEAR("Correct orientation",(new_u-u).length(),0.0,1e-6);
  TEST_NEAR("Correct location",(new_p-p0).length(),0.0,1e-6);

  vimt_image_2d_of<double> response;
  pf->evaluate_region(image,p1,u,response);
  TEST("Response ni",response.image().ni(),7);
  TEST("Response nj",response.image().nj(),7);
  std::cout<<"World2im: "<<response.world2im()<<std::endl;

  // Check that response has local minima in correct place
  vgl_point_2d<double> ip = response.world2im()(new_p);
  TEST("Best pt in image (i)", ip.x()>=0 && ip.x()<response.image().ni(),true);
  TEST("Best pt in image (j)", ip.y()>=0 && ip.y()<response.image().nj(),true);

  double r0 = vil_bilin_interp_safe(response.image(),ip.x(),ip.y());
  double r1 = vil_bilin_interp_safe(response.image(),ip.x()-1,ip.y());
  double r2 = vil_bilin_interp_safe(response.image(),ip.x()+1,ip.y());
  std::cout<<r0<<','<<r1<<','<<r2<<std::endl;
  TEST("Local minima 1",r0<r1,true);
  TEST("Local minima 2",r0<r2,true);

  delete pf;
}

void test_region_pdf()
{
  std::cout << "*************************\n"
           << " Testing mfpf_region_pdf\n"
           << "*************************\n";

  mfpf_add_all_loaders();

  vpdfl_axis_gaussian_builder pdf_builder;

  mfpf_region_pdf_builder r_builder;
  r_builder.set_as_box(4,5,pdf_builder);
  TEST("Number of pixels",r_builder.n_pixels(),20);
  std::cout<<r_builder<<std::endl;
  r_builder.print_shape(std::cout);

  test_region_pdf_search(r_builder);

  r_builder.set_as_ellipse(4,3,pdf_builder);
  TEST("Number of pixels",r_builder.n_pixels(),35);
  std::cout<<r_builder<<std::endl;
  r_builder.print_shape(std::cout);

  test_region_pdf_search(r_builder);

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------

  {
    std::cout<<"Testing initialisation as box"<<std::endl;
    std::istringstream ss(
          "mfpf_region_pdf_builder\n"
          "{\n"
          "  shape: box { ni: 5 nj: 3 ref_x: 2.5 ref_y: 1.5 }\n"
          "  search_ni: 17\n"
          "  search_nj: 15\n"
          "}\n");

    std::unique_ptr<mfpf_point_finder_builder>
            pf = mfpf_point_finder_builder::create_from_stream(ss);

    TEST("Correct Point Finder Builder", pf->is_a(),"mfpf_region_pdf_builder");
    if (pf->is_a()=="mfpf_region_pdf_builder")
    {
      auto &a_pf = static_cast<mfpf_region_pdf_builder&>(*pf);
      std::cout<<a_pf<<std::endl;
      TEST("search_ni configured",a_pf.search_ni(),17);
      TEST("search_nj configured",a_pf.search_nj(),15);
      TEST("shape",a_pf.shape(),"box");
      TEST("n_pixels configured",a_pf.n_pixels(),15);
    }
  }

  {
    std::cout<<"Testing initialisation as ellipse"<<std::endl;
    std::istringstream ss(
          "mfpf_region_pdf_builder\n"
          "{\n"
          "  shape: ellipse { ri: 5 rj: 3 }\n"
          "  search_ni: 17\n"
          "  search_nj: 15\n"
          "  search_nA: 2\n"
          "  search_dA: 0.1\n"
          "  step_size: 1.01\n"
          "}\n");

    std::unique_ptr<mfpf_point_finder_builder>
            pf = mfpf_point_finder_builder::create_from_stream(ss);

    TEST("Correct Point Finder Builder", pf->is_a(),"mfpf_region_pdf_builder");
    if (pf->is_a()=="mfpf_region_pdf_builder")
    {
      auto &a_pf = static_cast<mfpf_region_pdf_builder&>(*pf);
      std::cout<<a_pf<<std::endl;
      TEST("search_ni configured",a_pf.search_ni(),17);
      TEST("search_nj configured",a_pf.search_nj(),15);
      TEST("shape",a_pf.shape(),"ellipse");
      TEST("n_pixels configured",a_pf.n_pixels(),45);
    }
  }

  {
    // Test builder returns correct type of object
    mfpf_region_pdf_builder b;
    mfpf_point_finder* pf = b.new_finder();
    TEST("Builder: Correct Finder",pf->is_a(),"mfpf_region_pdf");
    delete pf;
  }

  {
    mfpf_region_pdf region_pdf;
    region_pdf.set_search_area(13,14);

    // Test binary load and save
    mfpf_point_finder * base_ptr = &region_pdf;

    vsl_b_ofstream bfs_out("test_region_pdf.bvl.tmp");
    TEST ("Created test_region_pdf.bvl.tmp for writing", (!bfs_out), false);
    vsl_b_write(bfs_out, region_pdf);
    vsl_b_write(bfs_out, base_ptr);
    bfs_out.close();

    mfpf_region_pdf region_pdf_in;
#ifdef POINT_FINDER_WORKS // currently causes stack corruption...
    mfpf_point_finder *base_ptr_in = 0;
#endif

    vsl_b_ifstream bfs_in("test_region_pdf.bvl.tmp");
    TEST ("Opened test_region_pdf.bvl.tmp for reading", (!bfs_in), false);
    vsl_b_read(bfs_in, region_pdf_in);
#ifdef POINT_FINDER_WORKS
    vsl_b_read(bfs_in, base_ptr_in);
#endif
    TEST ("Finished reading file successfully", (!bfs_in), false);
    bfs_in.close();
    std::cout<<region_pdf<<std::endl
            <<region_pdf_in<<std::endl;
    TEST("Loaded==Saved",region_pdf_in,region_pdf);
#ifdef POINT_FINDER_WORKS
    TEST("Load region_pdf by base ptr (type)",
         base_ptr_in->is_a(), region_pdf.is_a());

    delete base_ptr_in;
#endif
  }

  vsl_delete_all_loaders();
}

TESTMAIN(test_region_pdf);
