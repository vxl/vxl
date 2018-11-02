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
static const double TOL = 10.0;
static const int NUM_IMGS = 11;

static const char* IMG_PATH =
    "contrib/cul/bundler/test/test_data";

static void test_initial_recon(int argc, char** argv)
{
    std::string filepath;

    if (argc < 2) {
        std::cerr<<"Supply a filename for the first two args!\n";
        TEST("test_tracks", true, false);

        filepath = IMG_PATH;
    }
    else {
        filepath = argv[1];
    }

    //-------------------- Load all the images.
    std::vector<vil_image_resource_sptr> imgs(NUM_IMGS);
    std::vector<double> exif_tags(NUM_IMGS);

    for (int i = 0; i < NUM_IMGS; i++){
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

    init(recon);

    //-------------------- Consistency checks

    test_recon(recon);

    int num_observed_imgs = 0;
    std::vector<bundler_inters_image_sptr>::const_iterator j;
    for (j = recon.feature_sets.begin(); j != recon.feature_sets.end(); j++)
    {
        if ((*j)->in_recon) {
            num_observed_imgs++;
        }
    }

    TEST_EQUAL("There are only two cameras in the reconstruction.",
               num_observed_imgs, 2);
}

TESTMAIN_ARGS(test_initial_recon);
