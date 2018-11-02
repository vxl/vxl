#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vil/algo/vil_blob.h>
#include <vil/vil_crop.h>
#include <vil/vil_print.h>



static vil_image_view<bool> generate_test_image()
{
  vil_image_view<bool> image(15,25);
  image.fill(false);
  image( 3, 10 ) = true;
  image( 3, 11 ) = true;
  image( 3, 12 ) = true;
  image( 3, 13 ) = true;
  image( 3, 14 ) = true;
  image( 3, 15 ) = true;
  image( 3, 16 ) = true;
  image( 4, 8 ) = true;
  image( 4, 9 ) = true;
  image( 4, 10 ) = true;
  image( 4, 11 ) = true;
  image( 4, 12 ) = true;
  image( 4, 13 ) = true;
  image( 4, 14 ) = true;
  image( 4, 15 ) = true;
  image( 4, 16 ) = true;
  image( 4, 17 ) = true;
  image( 4, 18 ) = true;
  image( 5, 7 ) = true;
  image( 5, 8 ) = true;
  image( 5, 9 ) = true;
  image( 5, 10 ) = true;
  image( 5, 11 ) = true;
  image( 5, 12 ) = true;
  image( 5, 13 ) = true;
  image( 5, 14 ) = true;
  image( 5, 15 ) = true;
  image( 5, 16 ) = true;
  image( 5, 17 ) = true;
  image( 5, 18 ) = true;
  image( 5, 19 ) = true;
  image( 6, 6 ) = true;
  image( 6, 7 ) = true;
  image( 6, 8 ) = true;
  image( 6, 9 ) = true;
  image( 6, 10 ) = true;
  image( 6, 11 ) = true;
  image( 6, 12 ) = true;
  image( 6, 13 ) = true;
  image( 6, 14 ) = true;
  image( 6, 15 ) = true;
  image( 6, 16 ) = true;
  image( 6, 17 ) = true;
  image( 6, 18 ) = true;
  image( 6, 19 ) = true;
  image( 7, 5 ) = true;
  image( 7, 6 ) = true;
  image( 7, 7 ) = true;
  image( 7, 8 ) = true;
  image( 7, 18 ) = true;
  image( 7, 19 ) = true;
  image( 7, 20 ) = true;
  image( 8, 5 ) = true;
  image( 8, 6 ) = true;
  image( 8, 7 ) = true;
  image( 8, 20 ) = true;
  image( 8, 21 ) = true;
  image( 9, 5 ) = true;
  image( 9, 6 ) = true;
  image( 9, 21 ) = true;
  image( 10, 4 ) = true;
  image( 10, 5 ) = true;
  image( 11, 4 ) = true;
  image( 12, 4 ) = true;
  return image;
}


static void test_algo_blob()
{
  std::cout<<"=== Testing vil_blob ===\n";
  vil_image_view<bool> image(10,11);
  vil_image_view<unsigned> labels;
  vil_image_view<unsigned> edge_labels;
  std::vector<vil_blob_region > regions;
  std::vector<vil_blob_pixel_list > edge_lists;

  // Create 3 x 3 square
  image.fill(false);
  vil_crop(image, 4,3, 5,3).fill(true);

  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_blob_labels_to_edge_labels(labels, vil_blob_4_conn, edge_labels);
  vil_blob_labels_to_pixel_lists(edge_labels, edge_lists);
  vil_print_all(std::cout, labels, 1);
  vil_print_all(std::cout, edge_labels, 1);

  TEST("Count (3x3)", regions.size(), 1);
  TEST("Area (3x3)", vil_area(regions[0]), 9);
  TEST("Count edge(3x3)", edge_lists.size(), 1);
  TEST("Area edge(3x3)", edge_lists[0].size(), 8);

  // Create 1 x 5 line
  image.fill(false);
  vil_crop(image, 5,1, 3,5).fill(true);

  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_blob_labels_to_edge_labels(labels, vil_blob_4_conn, edge_labels);
  vil_blob_labels_to_pixel_lists(edge_labels, edge_lists);
  vil_print_all(std::cout, labels, 1);
  vil_print_all(std::cout, edge_labels, 1);

  TEST("Count (1x5)", regions.size(), 1);
  TEST("Area (1x5)", vil_area(regions[0]), 5);
  TEST("Count edge (1x5)", edge_lists.size(), 1);
  TEST("Area edge (1x5)", edge_lists[0].size(), 5);


  // Make an L shape
  image(6,3)=true;
  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);

  vil_print_all(std::cout, labels, 1);

  TEST("Count (L shape)", regions.size(), 1);
  TEST("Area (L shape)", vil_area(regions[0]), 6);

  // Make a T shape
  image.fill(false);
  vil_crop(image, 5,1, 3,5).fill(true);
  image(6,5)=true;
  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);

  vil_print_all(std::cout, labels, 1);

  TEST("Count (T shape)", regions.size(), 1);
  TEST("Area (T shape)", vil_area(regions[0]), 6);


  // Make a U shape
  image.fill(false);
  vil_crop(image, 3,5, 3,5).fill(true);
  vil_crop(image, 4,3, 3,3).fill(false);
  image(3,7)=false;
  image(7,7)=false;
  image(5,6)=false;

  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_print_all(std::cout, labels, 1);

  TEST("Count (U shape)", regions.size(), 1);
  TEST("Area (U shape)", vil_area(regions[0]), 13);




  // Make a V shape
  image.fill(false);
  image(6,5)=true;
  image(5,4)=true;
  image(7,4)=true;

  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_print_all(std::cout, labels, 1);

  TEST("Count (V shape 4-conn)", regions.size(), 3);
  TEST("Area (V shape 4-conn - 1)", vil_area(regions[0]), 1);
  TEST("Area (V shape 4-conn - 2)", vil_area(regions[1]), 1);
  TEST("Area (V shape 4-conn - 3)", vil_area(regions[2]), 1);

  vil_blob_labels(image, vil_blob_8_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_print_all(std::cout, labels, 1);

  TEST("Count (V shape 8-conn)", regions.size(), 1);
  TEST("Area (V shape 8-conn - 1)", vil_area(regions[0]), 3);

  // Make a Cross shape
  image.fill(false);
  vil_crop(image, 5,1, 3,5).fill(true);
  vil_crop(image, 3,5, 5,1).fill(true);

  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_blob_labels_to_edge_labels(labels, vil_blob_4_conn, edge_labels);
  vil_blob_labels_to_pixel_lists(edge_labels, edge_lists);
  vil_print_all(std::cout, labels, 1);
  vil_print_all(std::cout, edge_labels, 1);

  TEST("Count (Cross shape 4-conn)", regions.size(), 1);
  TEST("Area (Cross shape 4-conn)", vil_area(regions[0]), 9);
  TEST("Count edge(Cross shape 4-conn)", edge_lists.size(), 1);
  TEST("Area edge(Cross shape 4-conn)", edge_lists[0].size(), 9);

  vil_blob_labels(image, vil_blob_8_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_blob_labels_to_edge_labels(labels, vil_blob_8_conn, edge_labels);
  vil_blob_labels_to_pixel_lists(edge_labels, edge_lists);
  vil_print_all(std::cout, labels, 1);
  vil_print_all(std::cout, edge_labels, 1);

  TEST("Count (Cross shape 8-conn)", regions.size(), 1);
  TEST("Area (Cross shape 8-conn)", vil_area(regions[0]), 9);
  TEST("Count edge(Cross shape 8-conn)", edge_lists.size(), 1);
  TEST("Area edge(Cross shape 8-conn)", edge_lists[0].size(), 8);

  // Line up to edge
  image.fill(false);
  vil_crop(image, 5,1, 0,10).fill(true);
  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_print_all(std::cout, labels, 1);

  TEST("Count (vertical line)", regions.size(), 1);
  TEST("Area (vertical line)", vil_area(regions[0]), 10);

  // Line up to edge
  image.fill(false);
  vil_crop(image, 0,10, 5,1).fill(true);
  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_print_all(std::cout, labels, 1);

  TEST("Count (Horizontal line)", regions.size(), 1);
  TEST("Area (Horizontal line)", vil_area(regions[0]), 10);
  TEST("N chords (Horizontal line)", regions[0].size(), 1);

  std::cout<<"Test nested blobs."<<std::endl;
  image.fill(false);
  vil_crop(image, 3,6, 3,6).fill(true);
  vil_crop(image, 4,4, 4,4).fill(false);
  vil_crop(image, 5,2, 5,2).fill(true);

  vil_blob_labels(image, vil_blob_4_conn, labels);
  vil_blob_labels_to_regions(labels, regions);
  vil_print_all(std::cout, labels, 1);

  TEST("Count (nested blocks)", regions.size(), 2);
  // Assume that first blob is outer one - but API gives no guarantee.
  TEST("Area (outer of nested blocks)", vil_area(regions[0]), 20);
  TEST("Area (inner of nested blocks)", vil_area(regions[1]), 4);

  // Generic image
  {
    vil_image_view<bool> c_shape_image = generate_test_image();
    vil_print_all(std::cout, c_shape_image, 1);

    vil_blob_labels(c_shape_image, vil_blob_8_conn, labels);
    vil_blob_labels_to_regions(labels, regions);
    vil_blob_labels_to_edge_labels(labels, vil_blob_8_conn, edge_labels);
    vil_blob_labels_to_pixel_lists(edge_labels, edge_lists);
    vil_print_all(std::cout, labels, 1);
    vil_print_all(std::cout, edge_labels, 1);

    TEST("Number of blobs", regions.size(), 1 );

    TEST("Size of blob", vil_area(regions[0]), 64 );

    TEST("Number of blob edge lists", edge_lists.size(), 1 );
    TEST("Size of blob 8-conn edge ", edge_lists[0].size(), 40 );

  }
}

TESTMAIN(test_algo_blob);
