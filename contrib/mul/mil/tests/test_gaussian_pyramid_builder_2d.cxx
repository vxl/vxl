#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <testlib/testlib_test.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_gaussian_pyramid_builder_2d.h>
#include <mil/mil_image_pyramid.h>
#include <vil/vil_byte.h>

#include <vsl/vsl_binary_loader.h>

void test_gaussian_pyramid_builder_2d_build(mil_gaussian_pyramid_builder_2d<vil_byte>& builder)
{
  int nx = 20, ny = 20;
  vcl_cout<<"Filter Width: "<<builder.filter_width()<<vcl_endl;
  vcl_cout<<"Image Size: "<<nx<<" x "<<ny<<vcl_endl;

  mil_image_2d_of<vil_byte> image0;
  image0.resize(nx,ny);

  for (int y=0;y<image0.ny();++y)
    for (int x=0;x<image0.nx();++x)
      image0(x,y) = x+y*10;

  int default_n_levels = builder.maxLevels();
  builder.setMaxLevels(2);
  mil_image_pyramid image_pyr;

  builder.build(image_pyr,image0);

  vcl_cout<<"Result:\n";
  image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 2);

  int nx2 = (nx+1)/2;
  int ny2 = (ny+1)/2;
  const mil_image_2d_of<vil_byte>& image1 = (const mil_image_2d_of<vil_byte>&) image_pyr(1);
  TEST("Level 1 size x",image1.nx(),(nx+1)/2);
  TEST("Level 1 size y",image1.ny(),(ny+1)/2);
  TEST("Pixel (0,0)",image0(0,0),image1(0,0));
  TEST("Pixel (1,1)",image0(2,2),image1(1,1));
  TEST("Corner pixel",image0(nx2*2-2,ny2*2-2),image1(nx2-1,ny2-1));

  builder.setMaxLevels(default_n_levels);
  builder.extend(image_pyr);
  vcl_cout<<"\n\n\nTesting builder.extend():\n";
  image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 3);
}

void test_gaussian_pyramid_builder_2d()
{
  vcl_cout << "\n\n************************************************\n";
  vcl_cout << " Testing mil_gaussian_pyramid_builder_2d (byte)\n";
  vcl_cout << "************************************************\n";

  mil_gaussian_pyramid_builder_2d<vil_byte> builder;
  builder.set_filter_width(3);
  test_gaussian_pyramid_builder_2d_build(builder);
  builder.set_filter_width(5);
  test_gaussian_pyramid_builder_2d_build(builder);

  vcl_cout<<"\n\n======== TESTING I/O ===========\n";

  vsl_add_to_binary_loader(mil_gaussian_pyramid_builder_2d<vil_byte>());

  vcl_string test_path = "test_gaussian_pyramid_builder_2d.bvl.tmp";
  vsl_b_ofstream bfs_out(test_path);
  TEST (("Created " + test_path + " for writing").c_str(),
             (!bfs_out), false);
  vsl_b_write(bfs_out, builder);
  vsl_b_write(bfs_out, (mil_image_pyramid_builder*)(&builder));
  bfs_out.close();

  mil_gaussian_pyramid_builder_2d<vil_byte> builder_in;
  mil_image_pyramid_builder* ptr_in=0;

  vsl_b_ifstream bfs_in(test_path);
  TEST (("Opened " + test_path + " for reading").c_str(),
           (!bfs_in), false);
  vsl_b_read(bfs_in, builder_in);
  vsl_b_read(bfs_in, ptr_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("saved builder = loaded builder",
    builder.scale_step() == 2.0, true);
  TEST("saved and loaded builder by base class ptr",
    ptr_in->is_a() == builder.is_a(), true);
  delete ptr_in;

  vsl_delete_all_loaders();
}


TESTLIB_DEFINE_MAIN(test_gaussian_pyramid_builder_2d);
