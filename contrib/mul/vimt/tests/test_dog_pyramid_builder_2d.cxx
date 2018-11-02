// This is mul/vimt/tests/test_dog_pyramid_builder_2d.cxx
#include <iostream>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <vimt/vimt_dog_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vsl/vsl_binary_loader.h>
#include <vimt/algo/vimt_find_peaks.h>
#include <vgl/vgl_point_2d.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

static void test_dog_pyramid_builder_2d_build(vimt_dog_pyramid_builder_2d<float>& builder)
{
  unsigned ni = 57, nj = 63;
  std::cout<<"Image Size: "<<ni<<" x "<<nj<<'\n';

  vimt_image_2d_of<float> image0;
  image0.image().set_size(ni,nj);
  image0.image().fill(0.0f);
  image0.image()(ni/5,nj/3) = 100.0f;

  int default_n_levels = builder.max_levels();
  TEST("Default max. number of levels is 99", default_n_levels, 99);
  builder.set_max_levels(6);
  vimt_image_pyramid dog_pyr,smooth_pyr;

  builder.build_dog(dog_pyr,smooth_pyr,image0,true);

  TEST("Found correct number of levels", dog_pyr.n_levels(), 6);
  const auto& v_image0 =
                 static_cast<const vimt_image_2d_of<float>&>(dog_pyr(0));
  TEST("Base width",v_image0.image().ni(),ni);
  TEST("Base height",v_image0.image().nj(),nj);

  for (int L=0;L<smooth_pyr.n_levels();++L)
  {
    const auto& imageL =
             static_cast<const vimt_image_2d_of<float>&>(smooth_pyr(L));

    vgl_point_2d<double> p = vimt_find_max(imageL);

    std::cout<<"Level "<<L<<" smoothed peak point is at "<<p<<std::endl;

    const auto& dogL =
             static_cast<const vimt_image_2d_of<float>&>(dog_pyr(L));

    vgl_point_2d<double> q = vimt_find_max(dogL);

    std::cout<<"Level "<<L<<" peak DoG response is at   "<<q<<std::endl;

    TEST("Peak DoG Response", p, q);

    TEST_NEAR("near real peak",(q-vgl_point_2d<double>(ni/5,nj/3)).sqr_length(),0,L*L);
  }
}

static void test_dog_pyramid_builder_2d()
{
  std::cout << "********************************************\n"
           << " Testing vimt_dog_pyramid_builder_2d (byte)\n"
           << "********************************************\n";

  vimt_dog_pyramid_builder_2d<float> builder;
  test_dog_pyramid_builder_2d_build(builder);

  std::cout<<"\n\n======== TESTING I/O ===========\n";

  vsl_add_to_binary_loader(vimt_dog_pyramid_builder_2d<float>());

  std::string test_path = "test_dog_pyramid_builder_2d.bvl.tmp";
  vsl_b_ofstream bfs_out(test_path);
  TEST(("Created " + test_path + " for writing").c_str(), (!bfs_out), false);
  vsl_b_write(bfs_out, builder);
  vsl_b_write(bfs_out, static_cast<vimt_image_pyramid_builder*>(&builder));
  bfs_out.close();

  vimt_dog_pyramid_builder_2d<float> builder_in;
  vimt_image_pyramid_builder* ptr_in=nullptr;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in), false);
  vsl_b_read(bfs_in, builder_in);
  vsl_b_read(bfs_in, ptr_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
#endif

  TEST("saved builder = loaded builder", builder_in.scale_step(), builder.scale_step());
  TEST("saved and loaded builder by base class ptr", ptr_in->is_a(), builder.is_a());
  delete ptr_in;

  vsl_delete_all_loaders();
}

TESTMAIN(test_dog_pyramid_builder_2d);
