// This is mul/mil3d/tests/test_gaussian_pyramid_builder_3d.cxx
#include <testlib/testlib_test.h>
#include <mil3d/mil3d_image_3d_of.h>
#include <mil3d/mil3d_gaussian_pyramid_builder_3d.h>
#include <mil/mil_image_pyramid.h>
#include <vsl/vsl_binary_loader.h>
#include <vcl_iostream.h>

void test_gaussian_pyramid_builder_3d_build(mil3d_gaussian_pyramid_builder_3d<float>& builder)
{
  int nx = 20, ny = 20, nz = 20;
  vcl_cout<<"Filter Width: "<<builder.filter_width()<<vcl_endl;
  vcl_cout<<"Image Size: "<<nx<<" x "<<ny<<" z "<<nz<<vcl_endl;

  mil3d_image_3d_of<float> image0;
  image0.resize(nx,ny,nz);

  for (int z=0;z<image0.nz();++z)
    for (int y=0;y<image0.ny();++y)
      for (int x=0;x<image0.nx();++x)
        image0(x,y,z) = x*0.1f+y+z*10;

  int default_n_levels = builder.maxLevels();
  builder.setMaxLevels(2);
  mil_image_pyramid image_pyr;

  builder.build(image_pyr,image0);

//   vcl_cout<<"Result:\n";
//   image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 2);

  int nx2 = (nx+1)/2;
  int ny2 = (ny+1)/2;
  int nz2 = (nz+1)/2;
  const mil3d_image_3d_of<float>& image1 = (const mil3d_image_3d_of<float>&) image_pyr(1);
  TEST("Level 1 size x",image1.nx(),(nx+1)/2);
  TEST("Level 1 size y",image1.ny(),(ny+1)/2);
  TEST("Level 1 size z",image1.nz(),(nz+1)/2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-6);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,1,1),1e-6);
  TEST_NEAR("Pixel (2,3,3)",image1(2,3,3),66.4f,1e-6);
  TEST_NEAR("Corner pixel",image0(nx2*2-2,ny2*2-2,nz2*2-2),image1(nx2-1,ny2-1,nz2-1),1e-6);

  // restore maxLevels:
  builder.setMaxLevels(default_n_levels);

  vcl_cout<<"\n\nTesting builder.extend():\n";
  builder.extend(image_pyr);
//  image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 3);
}

// Check in-homogeneous smoothing option (ie only smooth in x,y but not z on some levels)
void test_gaussian_pyramid_builder_3d_build_xy(mil3d_gaussian_pyramid_builder_3d<float>& builder)
{
  int nx = 20, ny = 20, nz = 10;
  vcl_cout<<"Filter Width: "<<builder.filter_width()<<vcl_endl;
  vcl_cout<<"Image Size: "<<nx<<" x "<<ny<<" x "<<nz<<vcl_endl;

  mil3d_image_3d_of<float> image0;
  image0.resize(nx,ny,nz);

  for (int z=0;z<image0.nz();++z)
    for (int y=0;y<image0.ny();++y)
      for (int x=0;x<image0.nx();++x)
        image0(x,y,z) = x*0.1f+y+z*10;

  mil3d_transform_3d w2i;
  w2i.set_zoom_only(1,1,0.5,0,0,0);
  image0.setWorld2im(w2i);

  int default_n_levels = builder.maxLevels();
  builder.setMaxLevels(3);
  mil_image_pyramid image_pyr;

  builder.set_min_size(4,4,4);
  builder.build(image_pyr,image0);
  // restore maxLevels:
  builder.setMaxLevels(default_n_levels);

  vcl_cout<<"Resulting pyramid: "<<image_pyr<<vcl_endl;

//   vcl_cout<<"Result:\n";
//   image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 3);

  int nx2 = (nx+1)/2;
  int ny2 = (ny+1)/2;
  int nz2 = nz;  // Shouldn't change first level
  const mil3d_image_3d_of<float>& image1 = (const mil3d_image_3d_of<float>&) image_pyr(1);
  TEST("Level 1 size x",image1.nx(),nx2);
  TEST("Level 1 size y",image1.ny(),ny2);
  TEST("Level 1 size z",image1.nz(),nx2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-6);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,1,2),1e-6);
  TEST_NEAR("Pixel (2,3,3)",image1(2,3,3),36.4f,1e-6);
  TEST_NEAR("Corner pixel",image0(nx2*2-2,ny2*2-2,nz2-1),image1(nx2-1,ny2-1,nz2-1),1e-6);
}

// Check in-homogeneous smoothing option (ie only smooth in x,z but not y on some levels)
void test_gaussian_pyramid_builder_3d_build_xz(mil3d_gaussian_pyramid_builder_3d<float>& builder)
{
  int nx = 20, ny = 10, nz = 20;
  vcl_cout<<"Filter Width: "<<builder.filter_width()<<vcl_endl;
  vcl_cout<<"Image Size: "<<nx<<" x "<<ny<<" x "<<nz<<vcl_endl;

  mil3d_image_3d_of<float> image0;
  image0.resize(nx,ny,nz);

  for (int z=0;z<image0.nz();++z)
    for (int y=0;y<image0.ny();++y)
      for (int x=0;x<image0.nx();++x)
        image0(x,y,z) = x*0.1f+y+z*10;

  mil3d_transform_3d w2i;
  w2i.set_zoom_only(1,0.5,1,0,0,0);
  image0.setWorld2im(w2i);

  int default_n_levels = builder.maxLevels();
  builder.setMaxLevels(3);
  mil_image_pyramid image_pyr;

  builder.set_min_size(4,4,4);
  builder.build(image_pyr,image0);
  // restore maxLevels:
  builder.setMaxLevels(default_n_levels);

  vcl_cout<<"Resulting pyramid: "<<image_pyr<<vcl_endl;

//   vcl_cout<<"Result:\n";
//   image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 3);

  int nx2 = (nx+1)/2;
  int ny2 = ny;   // Shouldn't change first level
  int nz2 = (nz+1)/2;
  const mil3d_image_3d_of<float>& image1 = (const mil3d_image_3d_of<float>&) image_pyr(1);
  TEST("Level 1 size x",image1.nx(),nx2);
  TEST("Level 1 size y",image1.ny(),ny2);
  TEST("Level 1 size z",image1.nz(),nx2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-6);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,2,1),1e-6);
  TEST_NEAR("Pixel (2,3,3)",image1(2,3,3),63.4f,1e-6);
  TEST_NEAR("Corner pixel",image0(nx2*2-2,ny2-1,nz2*2-2),image1(nx2-1,ny2-1,nz2-1),1e-6);
}

// Check in-homogeneous smoothing option (ie only smooth in y,z but not x on some levels)
void test_gaussian_pyramid_builder_3d_build_yz(mil3d_gaussian_pyramid_builder_3d<float>& builder)
{
  int nx = 10, ny = 20, nz = 20;
  vcl_cout<<"Filter Width: "<<builder.filter_width()<<vcl_endl;
  vcl_cout<<"Image Size: "<<nx<<" x "<<ny<<" x "<<nz<<vcl_endl;

  mil3d_image_3d_of<float> image0;
  image0.resize(nx,ny,nz);

  for (int z=0;z<image0.nz();++z)
    for (int y=0;y<image0.ny();++y)
      for (int x=0;x<image0.nx();++x)
        image0(x,y,z) = x*0.1f+y+z*10;

  mil3d_transform_3d w2i;
  w2i.set_zoom_only(0.5,1,1,0,0,0);
  image0.setWorld2im(w2i);

  int default_n_levels = builder.maxLevels();
  builder.setMaxLevels(3);
  mil_image_pyramid image_pyr;

  builder.set_min_size(4,4,4);
  builder.build(image_pyr,image0);
  // restore maxLevels:
  builder.setMaxLevels(default_n_levels);

  vcl_cout<<"Resulting pyramid: "<<image_pyr<<vcl_endl;

//   vcl_cout<<"Result:\n";
//   image_pyr.print_all(vcl_cout);

  TEST("Found correct number of levels", image_pyr.n_levels(), 3);

  int nx2 = nx;    // Shouldn't change first level
  int ny2 = (ny+1)/2;
  int nz2 = (nz+1)/2;
  const mil3d_image_3d_of<float>& image1 = (const mil3d_image_3d_of<float>&) image_pyr(1);
  TEST("Level 1 size x",image1.nx(),nx2);
  TEST("Level 1 size y",image1.ny(),ny2);
  TEST("Level 1 size z",image1.nz(),nx2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-6);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(2,1,1),1e-6);
  TEST_NEAR("Pixel (2,3,3)",image1(2,3,3),66.2f,1e-6);
  TEST_NEAR("Corner pixel",image0(nx2-1,ny2*2-2,nz2*2-2),image1(nx2-1,ny2-1,nz2-1),1e-6);
}

void test_gaussian_pyramid_builder_3d()
{
  vcl_cout << "\n\n************************************************\n";
  vcl_cout << " Testing mil3d_gaussian_pyramid_builder_3d (float)\n";
  vcl_cout << "************************************************\n";

  mil3d_gaussian_pyramid_builder_3d<float> builder;
  builder.set_filter_width(5);
  test_gaussian_pyramid_builder_3d_build(builder);
  test_gaussian_pyramid_builder_3d_build_xy(builder);
  test_gaussian_pyramid_builder_3d_build_xz(builder);
  test_gaussian_pyramid_builder_3d_build_yz(builder);

  vcl_cout<<"\n\n======== TESTING I/O ===========\n";

  vsl_add_to_binary_loader(mil3d_gaussian_pyramid_builder_3d<float>());

  vcl_string test_path = "test_gaussian_pyramid_builder_3d.bvl.tmp";
  vsl_b_ofstream bfs_out(test_path);
  TEST (("Created " + test_path + " for writing").c_str(),
             (!bfs_out), false);
  vsl_b_write(bfs_out, builder);
  vsl_b_write(bfs_out, (mil_image_pyramid_builder*)(&builder));
  bfs_out.close();

  mil3d_gaussian_pyramid_builder_3d<float> builder_in;
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


TESTLIB_DEFINE_MAIN(test_gaussian_pyramid_builder_3d);
