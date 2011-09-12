#include <testlib/testlib_test.h>

#include <bundler/bundler.h>
#include <bundler/bundler_sfm_impl.h>


#include <vil/vil_load.h>
#include <vcl_string.h>
#include <vcl_iomanip.h>

static const double FOCAL_LENGTH_MM = 5.4;
static const double SENSOR_WIDTH_MM = 5.312;
static const double TOL = 10.0;
static const int NUM_IMGS = 11;

static const char* IMG_PATH =
    "contrib/cul/bundler/tests/test_data";

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
    vcl_vector<vil_image_resource_sptr> imgs(NUM_IMGS);
    vcl_vector<double> exif_tags(NUM_IMGS);

    for(int i = 0; i < NUM_IMGS; i++){
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

    

    //-------------------- Consistency checks

    // Check that only tracks with contributing points are observed
    vcl_vector<bundler_inters_track_sptr>::const_iterator i;
    for(i = recon.tracks.begin(); i != recon.tracks.end(); i++){
        const bool has_cont_points = 
            vcl_find(
                (*i)->contributing_points.begin(),
                (*i)->contributing_points.end(),
                true) 
            != (*i)->contributing_points.end();

        TEST_EQUAL("Only tracks with contributing points are observed.",
            (*i)->observed,
            has_cont_points);
    }

    // Check that only images that has features whose 3d points 
    // are observed are in the recon.
    int num_observed_imgs = 0;

    vcl_vector<bundler_inters_image_sptr>::const_iterator j;
    for(j = recon.feature_sets.begin(); j != recon.feature_sets.end(); j++){
        
        bool has_cont_pt = false;        

        for(int k = 0; k < (*j)->features.size(); k++){
            if( (*j)->features[k]->is_contributing() ){

                has_cont_pt = true;
                break;
            }
        }

        TEST_EQUAL("Only cameras in the recon have contributing points",
            has_cont_pt,
            (*j)->in_recon);

        if((*j)->in_recon){
            num_observed_imgs++;
        }
    }

    TEST_EQUAL("There are only two cameras in the reconstruction.",
        num_observed_imgs, 2);


    // Check that the projection of every contributing point 
    // is close its 3d point.
    for(i = recon.tracks.begin(); i != recon.tracks.end(); i++){

        vgl_point_3d<double> &pt = (*i)->world_point;

        vcl_vector<bundler_inters_feature_sptr>::iterator f;
        for(f = (*i)->points.begin(); f != (*i)->points.end(); f++){
            if( (*f)->is_contributing() ) {

                vpgl_perspective_camera<double> &cam = (*f)->image->camera;

                Assert("Image is in the recon",
                    (*f)->image->in_recon);
                Assert("Track is observed",
                    (*i)->observed);

                double u, v;

                cam.project(pt.x(), pt.y(), pt.z(), u, v);


                vcl_cout<<"--------------------------------------------" << vcl_endl;
                vcl_cout << cam << vcl_endl;
                vcl_cout<<"--------------------------------------------" << vcl_endl;


                TEST_NEAR("The proj. of every contributing pt is close to "
                    "its 3d world point.", 
                    u, (*f)->point.x(), TOL);


                TEST_NEAR("The proj. of every contributing pt is close to "
                    "its 3d world point.", 
                    v, (*f)->point.y(), TOL);
            }
        }
    }
}

TESTMAIN_ARGS(test_initial_recon);
