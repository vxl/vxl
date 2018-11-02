#include <string>
#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>

#include <bundler/bundler.h>

#include <bundler/tests/utils.h>


#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static const char* IMG_PATH = "contrib/cul/bundler/test/test_data";

static const int NUM_IMGS = 11;
// static const int NUM_IMGS = 4;
static const double FOCAL_LENGTH_MM = 5.4;
static const double SENSOR_WIDTH_MM = 5.312;

static void test_pipeline(int argc, char** argv)
{
    std::string filepath;

    if (argc < 2) {
        std::cerr << "Supply a test_data directory name (containing files"
                 << " kermit*.jpg) on the command line!\n";
        TEST("test_pipeline", true, false);

        filepath = IMG_PATH;
    }
    else {
        filepath = argv[1];
    }

    //-------------------- Load all the images.
    std::vector<vil_image_resource_sptr> imgs(NUM_IMGS);
    std::vector<double> exif_tags(NUM_IMGS);

    for (int i = 0; i < NUM_IMGS; ++i) {
        std::stringstream str;
        str << filepath << "/kermit"
            << std::setw(3) << std::setfill('0') << i << ".jpg";

        imgs[i] = vil_load_image_resource(str.str().c_str(), false);
        exif_tags[i] = imgs[i]->ni() * FOCAL_LENGTH_MM / SENSOR_WIDTH_MM;;
    }

    //-------------------- Run the pipeline
    std::vector<vpgl_perspective_camera<double> > cameras;
    std::vector<vgl_point_3d<double> > points;
    vnl_sparse_matrix<bool> visibility_graph;

    bundler_routines routines;

    bundler_driver(
        routines, imgs, exif_tags,
        cameras, points, visibility_graph);

    bundler_write_bundle_file("bundle.out", cameras, points);

    bundler_write_ply_file(
        "points.ply",
        points);

    std::cout<<"\n\n\n------------------------\n";

    for (const auto & camera : cameras) {
        std::cout<<"=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
                << camera
                <<"\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"<<std::endl;
    }
}


TESTMAIN_ARGS(test_pipeline);
