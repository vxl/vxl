#include <testlib/testlib_test.h>

#include <bundler/bundler.h>
#include <bundler/bundler_sfm_impl.h>

#include <vgl/vgl_point_3d.h>

static const int IMAGE_WIDTH = 320;
static const int IMAGE_HEIGHT = 640;

static const double TOL = .001;

// --------------World Points---------------------------------------------
static const vgl_point_3d<double> WORLD_POINTS[] = {
    vgl_point_3d<double>(1, 1, 3),
    vgl_point_3d<double>(3, 4, 1),
    vgl_point_3d<double>(5, 2, 2),
    vgl_point_3d<double>(4, 3, 1),
    vgl_point_3d<double>(1, 6, 1),
    vgl_point_3d<double>(3, 4, 4),
    vgl_point_3d<double>(3, 1, 1),
    vgl_point_3d<double>(0, 2, 2)
};

static const int NUM_WORLD_POINTS = 8;


// --------------Cameras -------------------------------------------------
static const vpgl_perspective_camera<double> CAMERAS[] = {
    vpgl_perspective_camera<double>(
        vpgl_calibration_matrix<double>(64, vgl_point_2d<double>(160, 320)),
        vgl_point_3d<double>(1, 1, 1),
        vgl_rotation_3d<double>(0, 1, 0)),

     vpgl_perspective_camera<double>(
        vpgl_calibration_matrix<double>(64, vgl_point_2d<double>(160, 320)),
        vgl_point_3d<double>(0, 0, 0),
        vgl_rotation_3d<double>(0, 0, 0)),

     vpgl_perspective_camera<double>(
        vpgl_calibration_matrix<double>(64, vgl_point_2d<double>(160, 320)),
        vgl_point_3d<double>(0, -1, 0),
        vgl_rotation_3d<double>(0, -1, 0))
};

static const int NUM_CAMERAS = 3;


// -----------------------------------------------------------------------
static void test_bundle_adjust()
{
    //-------------------------------------------------------------------
    bundler_inters_reconstruction recon;

    // Creates all the images
    for (const auto & i : CAMERAS) {
        bundler_inters_image_sptr img(new bundler_inters_image());
        img->camera = i;
        img->in_recon = true;

        recon.feature_sets.push_back(img);
    }


    // Creates all the world points
    for (const auto & i : WORLD_POINTS) {
        bundler_inters_track_sptr track(new bundler_inters_track());
        track->world_point = i;
        track->observed = true;

        for (int j = 0; j < NUM_CAMERAS; j++) {

            // Creates a feature, and puts it in the correct places
            double row, col;
            vnl_vector<double> d;
            int index = recon.feature_sets[j]->features.size();

            CAMERAS[j].project(
                i.x() + TOL,
                i.y() - TOL,
                i.z() + TOL/2.0f,
                row, col);

            if ((row == 0 && col == 0) || row < 0 || col < 0) {
                continue;
            }

            bundler_inters_feature_sptr f(
                new bundler_inters_feature(
                    row, col,
                    d, recon.feature_sets[j], index));

            recon.feature_sets[j]->features.push_back(f);

            f->track = track;
            f->index_in_track = track->add_feature(f);

            f->mark_as_contributing();
        }

        recon.tracks.push_back(track);
    }


    //-------------------------------------------------------------------
    bundler_sfm_impl_bundle_adjust adjust;
    adjust(recon);


    for (int i = 0; i < NUM_WORLD_POINTS; i++) {
        TEST_NEAR("The points are close (x)",
                  WORLD_POINTS[i].x(), recon.tracks[i]->world_point.x(),
                  TOL);

        TEST_NEAR("The points are close (y)",
                  WORLD_POINTS[i].y(), recon.tracks[i]->world_point.y(),
                  TOL);

        TEST_NEAR("The points are close (z)",
                  WORLD_POINTS[i].z(), recon.tracks[i]->world_point.z(),
                  TOL);
    }

    for (int i = 0; i < NUM_CAMERAS; i++) {
        // ---------------- Calibrations are similar
        TEST_NEAR("Focal length is similar",
                  recon.feature_sets[i]->camera.get_calibration().focal_length(),
                  CAMERAS[i].get_calibration().focal_length(), TOL);

        TEST_NEAR("Skew is similar",
                  recon.feature_sets[i]->camera.get_calibration().skew(),
                  CAMERAS[i].get_calibration().skew(), TOL);

        TEST_NEAR("Camera center (x) is similar",
                  recon.feature_sets[i]->camera.get_calibration().principal_point().x(),
                  CAMERAS[i].get_calibration().principal_point().x(), TOL);

        TEST_NEAR("Camera center (y) is similar",
                  recon.feature_sets[i]->camera.get_calibration().principal_point().y(),
                  CAMERAS[i].get_calibration().principal_point().y(), TOL);

        // ----------------- Camera centers are similar
        TEST_NEAR("Camera center is similar (x)",
                  recon.feature_sets[i]->camera.get_camera_center().x(),
                  CAMERAS[i].get_camera_center().x(), TOL);

        TEST_NEAR("Camera center is similar (y)",
                  recon.feature_sets[i]->camera.get_camera_center().y(),
                  CAMERAS[i].get_camera_center().y(), TOL);

        TEST_NEAR("Camera center is similar (z)",
                  recon.feature_sets[i]->camera.get_camera_center().z(),
                  CAMERAS[i].get_camera_center().z(), TOL);
    }
}


TESTMAIN(test_bundle_adjust);
