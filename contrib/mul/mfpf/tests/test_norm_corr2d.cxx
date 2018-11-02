// This is mul/mfpf/tests/test_norm_corr2d.cxx
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mfpf_norm_corr2d
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
#include <mfpf/mfpf_norm_corr2d.h>
#include <mfpf/mfpf_norm_corr2d_builder.h>
#include <vil/vil_bilin_interp.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//=======================================================================

void test_norm_corr2d_search(mfpf_point_finder_builder& b)
{
  std::cout<<"Testing building and search."<<std::endl;

  mfpf_point_finder* pf = b.new_finder();

  // Create a test image
  vimt_image_2d_of<float> image(20,20);
  image.image().fill(0);

  // Add a point in the centre
  image.image()(10,10)=99;

  vgl_point_2d<double> p0(9.5,9.5), p1(7.5,8.5), p2(7.8,8.2);
  vgl_vector_2d<double> u(1,0);

  b.clear(1);
  b.add_example(image,p0,u);
  b.build(*pf);

  std::cout<<"Built model: "<<pf<<std::endl;

  vgl_point_2d<double> new_p;
  vgl_vector_2d<double> new_u;

  TEST_NEAR("Evaluate at true point",pf->evaluate(image,p0,u),0.0,1e-6);

  pf->set_search_area(0,0);
  double v0 = pf->search(image,p0,u,new_p,new_u);
  std::cout<<"Found point: "<<new_p<<std::endl;
  TEST_NEAR("Search with one point",v0,0.0,1e-6);
  TEST_NEAR("Correct location",(new_p-p0).length(),0.0,1e-6);

  pf->set_search_area(3,3);
  pf->search(image,p1,u,new_p,new_u);
  std::cout<<"Found point: "<<new_p<<std::endl;

  TEST_NEAR("Correct orientation",(new_u-u).length(),0.0,1e-6);
  TEST_NEAR("Correct location",(new_p-p0).length(),0.0,1e-6);

  std::cout<<"Without parabolic optimisation: "<<std::endl;
  pf->search(image,p2,u,new_p,new_u);
  std::cout<<"Found point (no opt): "<<new_p<<std::endl
          <<"Testing parabolic optimisation."<<std::endl;
  pf->set_search_area(3,3);
  pf->search_with_opt(image,p2,u,new_p,new_u);
  std::cout<<"Found point ( + opt): "<<new_p<<std::endl;

  TEST_NEAR("Correct orientation",(new_u-u).length(),0.0,1e-6);
  TEST_NEAR("Correct location",(new_p-p0).length(),0.0,0.25);

  vimt_image_2d_of<double> response;
  pf->evaluate_region(image,p1,u,response);
  TEST("Response ni",response.image().ni(),7);
  TEST("Response nj",response.image().nj(),7);
  std::cout<<"World2im: "<<response.world2im()<<std::endl;

  // Check that response has local minima in correct place
  vgl_point_2d<double> ip = response.world2im()(new_p);
  std::cout<<"Best pt: "<<ip.x()<<','<<ip.y()<<std::endl;
  TEST("Best pt in image (i)", ip.x()>=0 && ip.x()<response.image().ni(),true);
  TEST("Best pt in image (j)", ip.y()>=0 && ip.y()<response.image().nj(),true);

  double r0 = vil_bilin_interp_safe(response.image(),ip.x(),ip.y());
  double r1 = vil_bilin_interp_safe(response.image(),ip.x()-0.5,ip.y());
  double r2 = vil_bilin_interp_safe(response.image(),ip.x()+0.5,ip.y());
  std::cout<<r0<<','<<r1<<','<<r2<<std::endl;
  TEST("Local minima 1",r0<r1,true);
  TEST("Local minima 2",r0<r2,true);

  delete pf;
}

void test_norm_corr2d_refine()
{
  std::cout<<"Testing mfpf_norm_corr2d::refine_match"<<std::endl;

  // Create a test image
  vimt_image_2d_of<float> image(40,40);
  image.image().fill(0);

  // Create a square in the centre
  for (unsigned j=20;j<26;++j)
    for (unsigned i=20;i<26;++i)
      image.image()(i,j)=99;

  vgl_point_2d<double> p0(23,23), p1(22.5,25.7), p;
  vgl_vector_2d<double> u0(1,0),u1(0.9,0.4), u;

  mfpf_norm_corr2d_builder nc_builder;
  mfpf_norm_corr2d nc;
  nc_builder.set_kernel_size(10,10);
  nc_builder.clear(1);
  nc_builder.add_example(image,p0,u0);
  nc_builder.build(nc);

  nc.set_search_area(5,5);
  std::cout<<nc<<std::endl;

  double f0 = nc.evaluate(image,p0,u0);
  std::cout<<"Fit at correct position: "<<f0<<std::endl;

  std::cout<<"Search at wrong scale/angle."<<std::endl;
  double f = nc.search(image,p1,u1,p,u);
  std::cout<<"Position: "<<p<<" Basis: "<<u<<" fit: "<<f<<std::endl;

  std::cout<<"Refine solution."<<std::endl;
  nc.refine_match(image,p,u,f);
  std::cout<<"Position: "<<p<<" Basis: "<<u<<" fit: "<<f<<std::endl;

  TEST("Refined position",(p-p0).length()<0.1,true);
  TEST("Refined basis",(u-u0).length()<0.1,true);
}

void test_norm_corr2d()
{
  std::cout << "**************************\n"
           << " Testing mfpf_norm_corr2d\n"
           << "**************************\n";

  mfpf_add_all_loaders();

  mfpf_norm_corr2d_builder nc_builder;
  nc_builder.set_kernel_size(4,4);
  test_norm_corr2d_search(nc_builder);
  test_norm_corr2d_refine();


  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    std::istringstream ss(
          "mfpf_norm_corr2d_builder\n"
          "{\n"
          "  ni: 4 nj: 5\n"
          "  search_ni: 17\n"
          "  search_nj: 15\n"
          "}\n");

    std::unique_ptr<mfpf_point_finder_builder>
            pf = mfpf_point_finder_builder::create_from_stream(ss);

    TEST("Correct Point Finder Builder", pf->is_a(),"mfpf_norm_corr2d_builder");
    if (pf->is_a()=="mfpf_norm_corr2d_builder")
    {
      auto &a_pf = static_cast<mfpf_norm_corr2d_builder&>(*pf);
      std::cout<<a_pf<<std::endl;
      TEST("search_ni configured",a_pf.search_ni(),17);
      TEST("search_nj configured",a_pf.search_nj(),15);
      TEST("ni configured",a_pf.ni(),4);
      TEST("nj configured",a_pf.nj(),5);
    }
  }

  {
    // Test builder returns correct type of object
    mfpf_norm_corr2d_builder b;
    mfpf_point_finder* pf = b.new_finder();
    TEST("Builder: Correct Finder",pf->is_a(),"mfpf_norm_corr2d");
    delete pf;
  }

  {
    mfpf_norm_corr2d norm_corr2d;
    norm_corr2d.set_search_area(13,14);

    // Test binary load and save
    mfpf_point_finder * base_ptr = &norm_corr2d;

    vsl_b_ofstream bfs_out("test_norm_corr2d.bvl.tmp");
    TEST ("Created test_norm_corr2d.bvl.tmp for writing", (!bfs_out), false);
    vsl_b_write(bfs_out, norm_corr2d);
    vsl_b_write(bfs_out, base_ptr);
    bfs_out.close();

    mfpf_norm_corr2d norm_corr2d_in;
    mfpf_point_finder *base_ptr_in = nullptr;

    vsl_b_ifstream bfs_in("test_norm_corr2d.bvl.tmp");
    TEST ("Opened test_norm_corr2d.bvl.tmp for reading", (!bfs_in), false);
    vsl_b_read(bfs_in, norm_corr2d_in);
    vsl_b_read(bfs_in, base_ptr_in);
    TEST ("Finished reading file successfully", (!bfs_in), false);
    bfs_in.close();
    std::cout<<norm_corr2d<<std::endl
            <<norm_corr2d_in<<std::endl;
    TEST("Loaded==Saved",norm_corr2d_in,norm_corr2d);
    TEST("Load norm_corr2d by base ptr (type)",
         base_ptr_in->is_a()==norm_corr2d.is_a(),true);

    delete base_ptr_in;
  }

  vsl_delete_all_loaders();
}

TESTMAIN(test_norm_corr2d);
