#include <testlib/testlib_test.h>

#include <bundler/tests/utils.h>

static const double TOL = 480 * .2;

void test_recon(const bundler_inters_reconstruction &recon,
    int expected_cameras){

    // Check that only tracks with contributing points are observed
    vcl_vector<bundler_inters_track_sptr>::const_iterator i;
    for (i = recon.tracks.begin(); i != recon.tracks.end(); i++){
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
    for (j = recon.feature_sets.begin(); j != recon.feature_sets.end(); j++)
    {
        bool has_cont_pt = false;

        for (unsigned int k = 0; k < (*j)->features.size(); k++) {
            if ( (*j)->features[k]->is_contributing() ) {
                has_cont_pt = true;
                break;
            }
        }

        TEST_EQUAL("Only cameras in the recon have contributing points",
                   has_cont_pt,
                   (*j)->in_recon);

        if ((*j)->in_recon) {
            num_observed_imgs++;
        }
    }


    if(expected_cameras != -1){
        TEST_EQUAL("Right number of cameras in the recon.",
            expected_cameras,
            num_observed_imgs);
    }

    // Check that the projection of every contributing point
    // is close its 3d point.
    for (i = recon.tracks.begin(); i != recon.tracks.end(); i++)
    {
        vgl_point_3d<double> &pt = (*i)->world_point;

        vcl_vector<bundler_inters_feature_sptr>::iterator f;
        for (f = (*i)->points.begin(); f != (*i)->points.end(); f++){
            if ( (*f)->is_contributing() ) {
                vpgl_perspective_camera<double> &cam = (*f)->image->camera;

                Assert("Image is in the recon",
                       (*f)->image->in_recon);
                Assert("Track is observed",
                       (*i)->observed);

                double u, v;

                cam.project(pt.x(), pt.y(), pt.z(), u, v);


                TEST_NEAR("The proj. of every contributing pt is close to "
                          "its 3d world point.",
                          u, (*f)->point.x(), TOL);


                TEST_NEAR("The proj. of every contributing pt is close to "
                          "its 3d world point.",
                          v, (*f)->point.y(), TOL);

                if( abs(u - (*f)->point.x()) > TOL || 
                    abs(v - (*f)->point.y()) > TOL){
                    vcl_cout << "----------------------------" << vcl_endl
                             << "Point failed!!"<<vcl_endl
                             << "World point: " << pt << vcl_endl
                             << "Image point: " << (*f)->point << vcl_endl
                             << "Projection: (" << u << ", " << v << ")\n"
                             << "Camera: " << vcl_endl
                             << cam 
                             << "----------------------------" << vcl_endl;
                }
            }
        }
    }

}
