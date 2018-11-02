// This is mul/msdi/tests/test_marked_images_from_file.cxx
//=======================================================================
//
//  Copyright: (C) 2017 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief Test msdi_marked_images_from_files

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <msdi/msdi_marked_images_from_files.h>
#include <vimt/vimt_image_2d_of.h>
#include <vil/vil_save.h>

//=======================================================================


inline bool is_reflected(const vimt_image_2d& im)
{
  return (static_cast<const vimt_image_2d_of<vxl_byte>&>(im).image()(0,0)!=100);
}

inline void test_n_samples(msdi_marked_images_from_files& data, unsigned n)
{
  std::cout<<"Testing state: "<<msdi_string_from_state(data.reflection_state())<<std::endl;
  TEST("N.Samples",data.size(),n);
  unsigned count=0;
  data.reset();
  do
  {
    count++;
    std::cout<<"Points: "<<data.points_name();
    if (std::fabs(data.points()[0].x()-1.0)<1e-5) std::cout<<" (Raw)";
    else                        std::cout<<" (Ref)";

//    std::cout<<" Image: "<<data.image_name();
//    if (is_reflected(data.image())) std::cout<<" (Ref) ";
//    else                          std::cout<<" (Raw)";

     std::cout<<std::endl;

  } while (data.next());
  TEST("Iterated N.Samples",count,n);
}

void test_marked_images_from_files()
{
  std::cout << "***********************\n"
           << " Testing msdi_marked_images_from_files\n"
           << "***********************\n";

  // Current test just checks the filename - doesn't actually load in the images/points.
  std::vector<std::string > image_names,points_names;

  vil_image_view<vxl_byte> image(5,5);
  image.fill(0);
  image(0,0)=100;

  vil_save(image,"image00.png");
  vil_save(image,"image01.png");
  vil_save(image,"image02.png");
  image_names.emplace_back("image00.png");
  image_names.emplace_back("image01.png");
  image_names.emplace_back("image02.png");

  msm_points points(2);
  points.set_point(0,1,0);
  points.set_point(1,3.5,0);
  points.write_text_file("image00.pts");
  points.write_text_file("image01.pts");
  points.write_text_file("image02.pts");
  points_names.emplace_back("image00.pts");
  points_names.emplace_back("image01.pts");
  points_names.emplace_back("image02.pts");

  points.write_text_file("ref-image00.pts");
  points.write_text_file("ref-image01.pts");
  points.write_text_file("ref-image02.pts");


  std::string image_dir("./"),points_dir("./");

  msdi_marked_images_from_files data;
  data.set(image_dir,image_names,points_dir,points_names);

  data.set_ref_prefix("ref-");

  data.set_state(Raw);
  test_n_samples(data,3);

  data.set_state(ReflectOnly);
  test_n_samples(data,3);

  data.set_state(OnlyReflectIm);
  test_n_samples(data,3);

  data.set_state(ReflectSym);
  test_n_samples(data,6);

  data.set_state(ReflectAsymRawPts);
  test_n_samples(data,6);

  data.set_state(ReflectAsymRefPts);
  test_n_samples(data,6);

}

TESTMAIN(test_marked_images_from_files);
