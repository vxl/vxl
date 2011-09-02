#include <testlib/testlib_test.h>

#include <bundler/bundler.h>
#include <bundler/bundler_sfm_impl.h>


#include <vil/vil_load.h>
#include <vcl_string.h>
#include <vcl_iomanip.h>

static const char* IMG_PATH =
    "/home/anjruu/vxl/contrib/cul/bundler/tests/test_data";

static const double FOCAL_LENGTH_MM = 5.4;
static const double SENSOR_WIDTH_MM = 5.312;

static void test_initial_recon(int argc, char** argv)
{
    vcl_string filepath;

    if (argc < 2) {
        vcl_cerr<<"Supply a filename for the first two args!\n";
        TEST("test_tracks", true, false);

        filepath = IMG_PATH;

    } else {
        filepath = argv[1];
    }

    //-------------------- Load all the images.
    vcl_vector<vil_image_resource_sptr> imgs(11);
    vcl_vector<double> exif_tags(11);

    for(int i = 0; i <= 10; i++){
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

    init(recon);

    

    //-------------------- Consistency checks TODO
    // Check that only tracks with contributing points are observed
    vcl_vector<bundler_inters_track_sptr>::const_iterator i;
    for(i = recon.tracks.begin(); i != recon.tracks.end(); i++){
        
    }

    // Check that there are only two cameras in the reconstruction

    // Check that the projection of every contributing point 
    // is close its 3d point.

}

TESTMAIN_ARGS(test_initial_recon);
