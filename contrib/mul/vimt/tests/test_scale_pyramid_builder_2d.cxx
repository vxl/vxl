// This is mul/vimt/tests/test_scale_pyramid_builder_2d.cxx
#include <vcl_iostream.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_scale_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vxl_config.h>
#include <vsl/vsl_binary_loader.h>
#include <testlib/testlib_test.h>

void test_scale_pyramid_builder_2d1()
{
  int nx = 20, ny = 20;
  vcl_cout << "\n\n**************************************************** \n";
  vcl_cout << " Testing vimt_scale_pyramid_builder_2d (byte)(nx="<<nx<<")\n";
  vcl_cout << "*********************************************************\n";


  vimt_image_2d_of<vxl_byte> image0;
  image0.image().set_size(nx,ny);

  // Use smooth plane pattern
  for (int y=0;y<image0.image().nj();++y)
     for (int x=0;x<image0.image().ni();++x)
     {
       image0.image()(x,y) = x+y*10;
     }

  vimt_scale_pyramid_builder_2d<vxl_byte> builder;
  int default_n_levels = builder.max_levels();
  builder.set_max_levels(2);
  builder.set_scale_step(1.2);
  vimt_image_pyramid image_pyr;

  builder.build(image_pyr,image0);

  vcl_cout<<"Result:\n";
  image_pyr.print_all(vcl_cout);

  vcl_cout<<"image_pyr.n_levels= "<< image_pyr.n_levels()<<vcl_endl;

  TEST("Found correct number of levels", image_pyr.n_levels(), 2);

  const vimt_image_2d_of<vxl_byte>& image1 = (const vimt_image_2d_of<vxl_byte>&) image_pyr(1);
  TEST("Level 1 size x",image1.image().ni(),(int)(nx/1.2+0.5));
  TEST("Level 1 size y",image1.image().nj(),(int)(ny/1.2+0.5));
  TEST("Pixel (0,0)",image1.image()(0,0),0);
  TEST("Pyramid(0) (12,12) = Pyramid(1) (11,10)",image0.image()(12,12), image1.image()(11,10));
  TEST("Corner pixel",image1.image()(16,16),209);

  builder.set_max_levels(default_n_levels);
  builder.extend(image_pyr);
  vcl_cout<<"\n\n\nTesting builder.extend():\n";
  image_pyr.print_all(vcl_cout);


  TEST("Found correct number of levels", image_pyr.n_levels(), 9);

  vimt_image_2d_of<float> image2(200, 200, 1);
  image2.image().fill(255.0f);
  vimt_image_pyramid image_pyr2;
  vimt_scale_pyramid_builder_2d<float> builder2;
  builder2.set_scale_step(1.2);
  builder2.build(image_pyr2, image2);
  bool all_less_than_256 = true;
  bool all_more_than_254 = true;
  for (int y=0;y<image0.image().nj();++y)
     for (int x=0;x<image0.image().ni();++x)
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
  TEST (("Created " + test_path + " for writing").c_str(),
             (!bfs_out), false);
  vsl_b_write(bfs_out, builder);
  vsl_b_write(bfs_out, (vimt_image_pyramid_builder*)(&builder));
  bfs_out.close();

  vimt_scale_pyramid_builder_2d<vxl_byte> builder_in;
  vimt_image_pyramid_builder* ptr_in=0;

  vsl_b_ifstream bfs_in(test_path);
  TEST (("Opened " + test_path + " for reading").c_str(),
           (!bfs_in), false);
  vsl_b_read(bfs_in, builder_in);
  vsl_b_read(bfs_in, ptr_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("saved builder = loaded builder",
    builder.scale_step() == builder_in.scale_step(), true);
  TEST("saved and loaded builder by base class ptr",
    ptr_in->is_a() == builder.is_a(), true);

  delete ptr_in;
  vsl_delete_all_loaders();
}

MAIN( test_scale_pyramid_builder_2d )
{
  START( "test_scale_pyramid_builder_2d" );
  test_scale_pyramid_builder_2d1();

  SUMMARY();
}
