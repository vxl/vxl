// This is mul/msdi/tests/test_reflected_marked_images.cxx
//=======================================================================
//
//  Copyright: (C) 2011 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test msdi_reflected_marked_images

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <msdi/msdi_reflected_marked_images.h>
#include <msdi/msdi_array_of_marked_images.h>
#include <vimt/vimt_image_2d_of.h>

//=======================================================================

void test_reflected_marked_images()
{
  std::cout << "***********************\n"
           << " Testing msdi_reflected_marked_images\n"
           << "***********************\n";

  // Create some points and images
  unsigned n_egs=3;
  std::vector<vil_image_view<vxl_byte> > images(n_egs);
  std::vector<msm_points> points(n_egs);

  for (unsigned i=0;i<n_egs;++i)
  {
    images[i].set_size(20,20);
    images[i].fill(vxl_byte(i));
    points[i].set_size(2);
    points[i].set_point(0,0,0);
    points[i].set_point(1,i,i);
  }

  msdi_array_of_marked_images array_data(images,points);

  TEST("Array has correct size",array_data.size(),n_egs);

  unsigned count=1;
  array_data.reset();
  while (array_data.next()) count++;
  TEST("Correct number of steps",count,n_egs);

  std::vector<unsigned> sym_pts(2);
  sym_pts[0]=1;
  sym_pts[1]=0;

  std::cout<<"Test only returning reflected examples."<<std::endl;
  msdi_reflected_marked_images ref_data1(array_data,sym_pts,true);
  TEST("Reflected array has correct size (1)",ref_data1.size(),n_egs);
  count=1;
  ref_data1.reset();
  while (ref_data1.next()) count++;
  TEST("Correct number of steps",count,n_egs);

  ref_data1.reset();
  for (unsigned i=0;i<n_egs;++i,ref_data1.next())
  {
    TEST_NEAR("Point 0 reflected",ref_data1.points()[0].x(),19-i,1e-6);
    TEST_NEAR("Point 1 reflected",ref_data1.points()[1].x(),19,1e-6);
    const auto& image
       = static_cast<const vimt_image_2d_of<vxl_byte>&>(ref_data1.image());
    TEST("Image value",image.image()(0,0),i);
  }

  std::cout<<"Test  returning reflected examples and originals"<<std::endl;
  msdi_reflected_marked_images ref_data2(array_data,sym_pts,false);
  TEST("Reflected array has correct size (2)",ref_data2.size(),2*n_egs);
  count=1;
  ref_data2.reset();
  while (ref_data2.next()) count++;
  TEST("Correct number of steps",count,2*n_egs);

  ref_data2.reset();
  for (unsigned i=0;i<n_egs;++i,ref_data2.next())
  {
    TEST_NEAR("Point 0 reflected",ref_data2.points()[0].x(),19-i,1e-6);
    TEST_NEAR("Point 1 reflected",ref_data2.points()[1].x(),19,1e-6);
    const auto& image
       = static_cast<const vimt_image_2d_of<vxl_byte>&>(ref_data2.image());
    TEST("Image value",image.image()(0,0),i);
  }
  for (unsigned i=0;i<n_egs;++i,ref_data2.next())
  {
    TEST_NEAR("Point 0 not reflected",ref_data2.points()[0].x(),0,1e-6);
    TEST_NEAR("Point 1 not reflected",ref_data2.points()[1].x(),i,1e-6);
  }

}

TESTMAIN(test_reflected_marked_images);
