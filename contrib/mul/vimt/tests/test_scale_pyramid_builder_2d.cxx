// This is mul/vimt/tests/test_scale_pyramid_builder_2d.cxx
#include <testlib/testlib_test.h>

#include <vimt/vimt_scale_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_image_2d_of.h>
#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vsl/vsl_binary_loader.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

void test_scale_pyramid_builder_2d1(unsigned int nx, unsigned int ny)
{
  vcl_cout << "************************************************************\n"
           << " Testing vimt_scale_pyramid_builder_2d (byte)(nx="<<nx<<", ny="<<ny<<")\n"
           << "************************************************************\n";

  const double scale_step = 1.2;
  const unsigned int nx_scaled = (unsigned int)(nx/scale_step+0.5), // round
                     ny_scaled = (unsigned int)(ny/scale_step+0.5);

  vimt_image_2d_of<vxl_byte> image0;
  image0.image().set_size(nx,ny);

  // Use smooth plane pattern, otherwise the "compacted" image at level 1
  // will not resemble the original image well enough and that makes testing
  // more difficult.
  for (unsigned int y=0;y<image0.image().nj();++y)
    for (unsigned int x=0;x<image0.image().ni();++x)
      image0.image()(x,y) = x+y*2;

  vimt_scale_pyramid_builder_2d<vxl_byte> builder;
  int default_n_levels = builder.max_levels();
  builder.set_max_levels(2);
  builder.set_scale_step(scale_step);
  vimt_image_pyramid image_pyr;

  builder.build(image_pyr,image0);

  vcl_cout<<"Result:\n";
  image_pyr.print_all(vcl_cout);

  vcl_cout<<"image_pyr.n_levels= "<< image_pyr.n_levels()<<vcl_endl;

  TEST("Found correct number of levels", image_pyr.n_levels(), 2);

  const vimt_image_2d_of<vxl_byte>& image1 =
    static_cast<const vimt_image_2d_of<vxl_byte>&>(image_pyr(1));
  TEST("Level 1 size x",image1.image().ni(),nx_scaled);
  TEST("Level 1 size y",image1.image().nj(),ny_scaled);
  TEST("Pixel (0,0)",image1.image()(0,0),image0.image()(0,0));
  TEST("Central pixel", image0.image()(nx/2,(ny-1)/2),
       image1.image()(nx_scaled/2,(ny_scaled-1)/2));
  TEST("Lower left corner pixel", image0.image()(0,ny-1),
       image1.image()(0,ny_scaled-1));
  TEST("Upper right corner pixel", image0.image()(nx-1,0),
       image1.image()(nx_scaled-1,0));
  TEST("Lower right corner pixel", image0.image()(nx-1,ny-1),
       image1.image()(nx_scaled-1,ny_scaled-1));

  builder.set_max_levels(default_n_levels);
  builder.extend(image_pyr);
  vcl_cout<<"\n\n\nTesting builder.extend():\n";
  image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 9);

  vimt_image_2d_of<float> image2(200, 200, 1);
  image2.image().fill(255.0f);
  vimt_image_pyramid image_pyr2;
  vimt_scale_pyramid_builder_2d<float> builder2;
  builder2.set_scale_step(scale_step);
  builder2.build(image_pyr2, image2);
  bool all_less_than_256 = true;
  bool all_more_than_254 = true;
  for (unsigned int y=0;y<image0.image().nj();++y)
     for (unsigned int x=0;x<image0.image().ni();++x)
     {
       if (image0.image()(x,y) > 255.01) all_less_than_256 = false;
       if (image0.image()(x,y) < 254.99) all_more_than_254 = false;
     }

  TEST("No drift upwards in a float pyramid", all_less_than_256, true);
  TEST("No drift downwards in a float pyramid", all_more_than_254, false);

  vcl_cout<<"\n\n======== TESTING I/O ===========\n";

  vsl_add_to_binary_loader(vimt_scale_pyramid_builder_2d<vxl_byte>());

  vcl_string test_path = "test_scale_pyramid_builder_2d.bvl.tmp";
  vsl_b_ofstream bfs_out(test_path);
  TEST(("Created " + test_path + " for writing").c_str(), (!bfs_out), false);
  vsl_b_write(bfs_out, builder);
  vsl_b_write(bfs_out, static_cast<vimt_image_pyramid_builder*>(&builder));
  bfs_out.close();

  vimt_scale_pyramid_builder_2d<vxl_byte> builder_in;
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

  TEST("saved builder = loaded builder",
       builder.scale_step(), builder_in.scale_step());
  TEST("saved and loaded builder by base class ptr",
       ptr_in->is_a(), builder.is_a());

  delete ptr_in;
  vsl_delete_all_loaders();
}

MAIN( test_scale_pyramid_builder_2d )
{
  START( "test_scale_pyramid_builder_2d\n" );
  test_scale_pyramid_builder_2d1(20,30);

  SUMMARY();
}
