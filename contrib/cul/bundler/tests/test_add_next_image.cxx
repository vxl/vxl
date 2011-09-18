#include <testlib/testlib_test.h>

#include <bundler/bundler.h>
#include <bundler/bundler_sfm_impl.h>


#include <vil/vil_load.h>
#include <vcl_string.h>
#include <vcl_iomanip.h>

static const double FOCAL_LENGTH_MM = 5.4;
static const double SENSOR_WIDTH_MM = 5.312;
static const double TOL = 480 * .4;
static const int NUM_IMGS = 11;

static const char* IMG_PATH =
    "test_data";

static void test_add_next_image(int argc, char** argv)
{
    vcl_string filepath;

    if (argc < 2) {
        vcl_cerr<<"Supply a filename for the first two args!\n";
        filepath = IMG_PATH;
    }
    else {
        filepath = argv[1];
    }

    //-------------------- Load all the images.
    vcl_vector<vil_image_resource_sptr> imgs(NUM_IMGS);
    vcl_vector<double> exif_tags(NUM_IMGS);

    for (int i = 0; i < NUM_IMGS; i++) {
        vcl_stringstream str;
        str << filepath << "/kermit"
            << vcl_setw(3) << vcl_setfill('0') << i << ".jpg";

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

    vcl_vector<bundler_inters_image_sptr> to_add, added;


    bundler_sfm_impl_select_next_images select;
    Assert("There is an image to select.",
           select(recon, to_add));

    TEST_EQUAL("Vectors are the correct size", to_add.size(), 1);


    bundler_sfm_impl_add_next_images add;
    add(to_add, recon, added);

    //-------------------- Consistency checks

    Assert("Sizes match up", to_add.size() == added.size());

    bundler_inters_image_sptr img = added[0];

    for (unsigned int f = 0; f < img->features.size(); ++f) {
        if ( img->features[f]->track &&
             img->features[f]->track->contributing_points[f]) {
            double u, v;
            img->camera.project(
                img->features[f]->track->world_point.x(),
                img->features[f]->track->world_point.y(),
                img->features[f]->track->world_point.z(),
                u, v);

            TEST_NEAR("The camera projects it's contributing pts well. (x)",
                      img->features[f]->point.x(), u, TOL);

            TEST_NEAR("The camera projects it's contributing pts well. (y)",
                      img->features[f]->point.y(), v, TOL);
        }
    }
}

TESTMAIN_ARGS(test_add_next_image);

