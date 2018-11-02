#include <iostream>
#include <cmath>
#include "utils.h"
//
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static const double TOL = 480 * .2;

void test_recon(const bundler_inters_reconstruction &recon,
                int expected_cameras)
{
    // Check that only tracks with contributing points are observed
    std::vector<bundler_inters_track_sptr>::const_iterator i;
    for (i = recon.tracks.begin(); i != recon.tracks.end(); i++){
        const bool has_cont_points =
            std::find(
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

    std::vector<bundler_inters_image_sptr>::const_iterator j;
    for (j = recon.feature_sets.begin(); j != recon.feature_sets.end(); j++)
    {
        bool has_cont_pt = false;

        for (auto & feature : (*j)->features) {
            if ( feature->is_contributing() ) {
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

    if (expected_cameras != -1) {
        TEST_EQUAL("Right number of cameras in the recon.",
                   expected_cameras,
                   num_observed_imgs);
    }

    // Check that the projection of every contributing point
    // is close its 3d point.
    for (i = recon.tracks.begin(); i != recon.tracks.end(); i++)
    {
        vgl_point_3d<double> &pt = (*i)->world_point;

        std::vector<bundler_inters_feature_sptr>::iterator f;
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

                if (std::fabs(u - (*f)->point.x()) > TOL ||
                    std::fabs(v - (*f)->point.y()) > TOL) {
                    std::cout << "----------------------------\n"
                             << "Point failed!!\n"
                             << "World point: " << pt << '\n'
                             << "Image point: " << (*f)->point << '\n'
                             << "Projection: (" << u << ", " << v << ")\n"
                             << "Camera:\n"
                             << cam
                             << "----------------------------" << std::endl;
                }
            }
        }
    }
}
