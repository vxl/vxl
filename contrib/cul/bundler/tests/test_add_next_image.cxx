#include <string>
#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>

#include <bundler/bundler.h>
#include <bundler/bundler_sfm_impl.h>

#include <bundler/tests/utils.h>

#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static const double FOCAL_LENGTH_MM = 5.4;
static const double SENSOR_WIDTH_MM = 5.312;
static const double TOL = 480 * .4;
static const int NUM_IMGS = 11;

static const char* IMG_PATH =
    "contrib/cul/bundler/test/test_data";

static void test_add_next_image(int argc, char** argv)
{
    std::string filepath;

    if (argc < 2) {
        std::cerr<<"Supply a filename for the first two args!\n";
        filepath = IMG_PATH;
    }
    else {
        filepath = argv[1];
    }

    //-------------------- Load all the images.
    std::vector<vil_image_resource_sptr> imgs(NUM_IMGS);
    std::vector<double> exif_tags(NUM_IMGS);

    for (int i = 0; i < NUM_IMGS; i++) {
        std::stringstream str;
        str << filepath << "/kermit"
            << std::setw(3) << std::setfill('0') << i << ".jpg";

        imgs[i] = vil_load_image_resource(str.str().c_str(), false);
        exif_tags[i] = imgs[i]->ni() * FOCAL_LENGTH_MM / SENSOR_WIDTH_MM;
    }

    //-------------------- Run the tracks stage.
    bundler_inters_reconstruction recon;

    bundler_tracks tracks_stage;
    tracks_stage.run_feature_stage(imgs, exif_tags, recon);


    //-------------------- Create the initial reconstruction
    bundler_sfm_impl_create_initial_recon init;

    const bool initial_recon_worked = init(recon);
    Assert("The initial reconstruction succeeded.",
           initial_recon_worked);

    if (!initial_recon_worked) {
        return;
    }

    std::vector<bundler_inters_image_sptr> to_add, added;


    bundler_sfm_impl_select_next_images select;
    Assert("There is an image to select.",
           select(recon, to_add));

    TEST_EQUAL("Vectors are the correct size", to_add.size(), 1);


    bundler_sfm_impl_add_next_images add;
    add(to_add, recon, added);

    bundler_sfm_impl_add_new_points add_pts;
    add_pts(recon, added);

    bundler_sfm_impl_bundle_adjust adjust;
    adjust(recon);
    //-------------------- Consistency checks

    test_recon(recon, 3);
}

TESTMAIN_ARGS(test_add_next_image);
