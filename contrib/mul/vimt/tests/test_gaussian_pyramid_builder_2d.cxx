// This is mul/vimt/tests/test_gaussian_pyramid_builder_2d.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vpl/vpl.h> // vpl_unlink()
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vsl/vsl_binary_loader.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

static void test_gaussian_pyramid_builder_2d_build(vimt_gaussian_pyramid_builder_2d<vxl_byte>& builder)
{
  unsigned ni = 20, nj = 20;
  vcl_cout<<"Filter Width: "<<builder.filter_width()<<'\n'
          <<"Image Size: "<<ni<<" x "<<nj<<'\n';

  vimt_image_2d_of<vxl_byte> image0;
  image0.image().set_size(ni,nj);

  for (unsigned y=0;y<image0.image().nj();++y)
    for (unsigned x=0;x<image0.image().ni();++x)
      image0.image()(x,y) = x+y*10;

  int default_n_levels = builder.max_levels();
  builder.set_max_levels(2);
  vimt_image_pyramid image_pyr;

  builder.build(image_pyr,image0);

  vcl_cout<<"Result:\n";
  image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 2);
  const vimt_image_2d_of<vxl_byte>& v_image0 = static_cast<const vimt_image_2d_of<vxl_byte>&>(image_pyr(0));
  TEST("Base width",v_image0.image().ni(),ni);
  TEST("Base height",v_image0.image().nj(),nj);

  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  const vimt_image_2d_of<vxl_byte>& v_image1 = static_cast<const vimt_image_2d_of<vxl_byte>&>( image_pyr(1));
  const vil_image_view<vxl_byte>& image1 = v_image1.image();
  TEST("Level 1 size x",image1.ni(),(ni+1)/2);
  TEST("Level 1 size y",image1.nj(),(nj+1)/2);
  TEST("Pixel (0,0)",image0.image()(0,0),image1(0,0));
  TEST("Pixel (1,1)",image0.image()(2,2),image1(1,1));
  TEST("Corner pixel",image0.image()(ni2*2-2,nj2*2-2),image1(ni2-1,nj2-1));

  builder.set_max_levels(default_n_levels);
  builder.extend(image_pyr);
  vcl_cout<<"\n\n\nTesting builder.extend():\n";
  image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 3);
}

static void test_gaussian_pyramid_builder_2d()
{
  vcl_cout << "*************************************************\n"
           << " Testing vimt_gaussian_pyramid_builder_2d (byte)\n"
           << "*************************************************\n";

  vimt_gaussian_pyramid_builder_2d<vxl_byte> builder;
  builder.set_filter_width(3);
  test_gaussian_pyramid_builder_2d_build(builder);
  builder.set_filter_width(5);
  test_gaussian_pyramid_builder_2d_build(builder);

  vcl_cout<<"\n\n======== TESTING I/O ===========\n";

  vsl_add_to_binary_loader(vimt_gaussian_pyramid_builder_2d<vxl_byte>());

  vcl_string test_path = "test_gaussian_pyramid_builder_2d.bvl.tmp";
  vsl_b_ofstream bfs_out(test_path);
  TEST(("Created " + test_path + " for writing").c_str(), (!bfs_out), false);
  vsl_b_write(bfs_out, builder);
  vsl_b_write(bfs_out, static_cast<vimt_image_pyramid_builder*>(&builder));
  bfs_out.close();

  vimt_gaussian_pyramid_builder_2d<vxl_byte> builder_in;
  vimt_image_pyramid_builder* ptr_in=0;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in), false);
  vsl_b_read(bfs_in, builder_in);
  vsl_b_read(bfs_in, ptr_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
#endif

  TEST("saved builder = loaded builder", builder.scale_step(), 2.0);
  TEST("saved and loaded builder by base class ptr", ptr_in->is_a(), builder.is_a());
  delete ptr_in;

  vsl_delete_all_loaders();
}

TESTMAIN(test_gaussian_pyramid_builder_2d);
