#include <cstdlib>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "bundler_sfm_impl.h"
//
#include <bundler/bundler_inters.h>
#include <bundler/bundler_utils.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/algo/vpgl_em_compute_5_point.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>


//------------------------------------------------------------------------
// Generic helper function to create a calibration matrix.
// Assumes the camera center is the center of the image.

static inline vpgl_calibration_matrix<double> create_k(
    bundler_inters_image_sptr const& img)
{
    vgl_point_2d<double> principal_point;
    principal_point.x() = img->source->ni() / 2.0;
    principal_point.y() = img->source->nj() / 2.0;

    return
        vpgl_calibration_matrix<double>(img->focal_length, principal_point);
}

//-------------------------------------------------------------------------
// Returns true if a matches the criteria for being in the
// initial reconstruction. a must have a focal length from exif
// tags for both images, and have more than the minimum number of
// features.
bool bundler_sfm_impl_create_initial_recon::can_be_initial_recon(
    const bundler_inters_match_set &a)
{
    return
        a.num_features() >= settings.min_number_of_matches_homography &&
        a.image1->focal_length != BUNDLER_NO_FOCAL_LEN &&
        a.image2->focal_length != BUNDLER_NO_FOCAL_LEN;
}

// Chooses two images from the set to create the initial reconstruction
bool bundler_sfm_impl_create_initial_recon::operator()(
    bundler_inters_reconstruction &recon)
{
    const int t = (int) (
        settings.inlier_threshold_homography *
        settings.inlier_threshold_homography);

    const int r = settings.number_ransac_rounds_homography;

    // First step: Find the two images to base the reconstruction on.
    // We do this by selecting the match in the set that is modeled the
    // worst by a homography (i.e. has the fewest number of inliers).
    bundler_inters_match_set *best_match = nullptr;
    double best_inlier_p = 1.0;

    std::vector<bundler_inters_match_set>::iterator i;
    for (i = recon.match_sets.begin(); i != recon.match_sets.end(); ++i)
    {
        const bool replace_best = can_be_initial_recon(*i) &&
            best_inlier_p > i->get_homography_inlier_percentage(t, r);

        if ( replace_best ) {
            best_match = &(*i);
            best_inlier_p = best_match->get_homography_inlier_percentage(t, r);
        }
    }

    // If we didn't find anything that can be in the initial
    // reconstruction, panic.
    if (! best_match) {
        std::cerr<< "Unable to create an initial reconstruction!\n"
                << "There is not a match set that both has an initial guess from EXIF tags and at least "
                << settings.min_number_of_matches_homography
                << " matches.\n";
        return false;
    }

    assert(can_be_initial_recon(*best_match));

    //------------------------------------------------------------------
    // We have the best match, so create two calibration matrices for
    // the image pair.
    vpgl_calibration_matrix<double> k1 = create_k(best_match->image1);
    vpgl_calibration_matrix<double> k2 = create_k(best_match->image2);


    //------------------------------------------------------------------
    // Use the five-point algorithm wrapped in RANSAC to find the
    // relative pose.
    vpgl_em_compute_5_point_ransac<double> five_point_ransac(
        settings.number_ransac_rounds_e_matrix,
        settings.inlier_threshold_e_matrix,
        false);

    std::vector<vgl_point_2d<double> > right_points(
        best_match->num_features());
    std::vector<vgl_point_2d<double> > left_points(
        best_match->num_features());

    for (int i = 0; i < best_match->num_features(); ++i) {
        right_points[i] = best_match->matches[i].first->point;
        left_points[i] = best_match->matches[i].second->point;
    }

    vpgl_essential_matrix<double> best_em;
    five_point_ransac.compute(
        right_points, k1,
        left_points, k2,
        best_em);


    //------------------------------------------------------------------
    // Get the two cameras.

    // Set the right camera to be have no translation or rotation.
    best_match->image1->camera =
        vpgl_perspective_camera<double>();

    best_match->image1->camera.set_calibration(k1);


    // Get two normalized (focal plane) coordinates so that we can
    // choose the direction of the left camera.
    vgl_point_2d<double> pt_left =
        k2.map_to_focal_plane(best_match->matches[0].second->point);

    vgl_point_2d<double> pt_right =
        k1.map_to_focal_plane(best_match->matches[0].first->point);

    // Extract the left camera from the essential matrix
    best_match->image2->camera.set_calibration(k2);

    extract_left_camera(
        best_em,
        vgl_point_2d<double>(pt_left.x(), pt_left.y()),
        vgl_point_2d<double>(pt_right.x(), pt_right.y()),
        best_match->image2->camera);


    //------------------------------------------------------------------
    // Triangulate the points that both observe.
    for (int i = 0; i < best_match->num_features(); ++i)
    {
        bundler_inters_feature_sptr f1 = best_match->matches[i].first;
        bundler_inters_feature_sptr f2 = best_match->matches[i].second;

        assert(f1->image != f2->image);

        // Get the track these both belong to.
        bundler_inters_track_sptr &track =
            best_match->matches[i].first->track;

        // Tell the images that they have a feature used to triangulate
        // a world point.
        f1->image->in_recon = true;
        f2->image->in_recon = true;

        // Tell the track to use these two features as contributing pts.
        const int side1_ind = f1->index_in_track;
        const int side2_ind = f2->index_in_track;

        track->contributing_points[side1_ind] = true;
        track->contributing_points[side2_ind] = true;

        track->observed = true;

        // Find the world point given these two image points
        track->world_point = triangulate_3d_point<double>(
            f1->image->camera,
            f1->point,
            f2->image->camera,
            f2->point);
    }

    return true;
}


//-----------------------------------------------------------------------
// to_check is a collection of features, and a source image. This represents
// an unprocessed camera. We want to see how many points which we have
// already processed this camera observes.
static int count_observed_points(
    const bundler_inters_image_sptr &to_check)
{
    int num_observed_pts = 0;

    assert( ! to_check->in_recon );

    std::vector<bundler_inters_feature_sptr>::const_iterator ii;
    for (ii = to_check->features.begin();
         ii != to_check->features.end(); ++ii)
    {
        if ((*ii)->track && (*ii)->track->observed) {
            ++num_observed_pts;
        }
    }

    return num_observed_pts;
}

// Takes in reconstruction and track_set, fills to_add as a return val.
bool bundler_sfm_impl_select_next_images::operator()(
    bundler_inters_reconstruction &recon,
    std::vector<bundler_inters_image_sptr> &to_add)
{
    // Look at every image
    int most_observed_points = 0;
    bundler_inters_image_sptr next_image;

    std::vector<bundler_inters_image_sptr>::const_iterator i;
    for (i = recon.feature_sets.begin(); i != recon.feature_sets.end(); ++i)
    {
        // Only look at images not in the reconstruction.
        if (! (*i)->in_recon ) {
            // Find the number of points that this camera views,
            // which are also viewed by a camera already added.
            const int num_observed_points =
                count_observed_points(*i);

            if (num_observed_points > most_observed_points) {
                next_image = *i;
                most_observed_points = num_observed_points;
            }
        }
    }


    // Because we wanted this class to be very general, we need to return
    // a vector. Put the next image to add into the vector.
    if (most_observed_points >= settings.min_number_observed_points) {
        assert(next_image);

        to_add.push_back(next_image);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------
class image_adder
{
  public:
    image_adder(
        std::vector<bundler_inters_image_sptr> *added_cameras,
        int r,
        double t) :
        added_cameras(added_cameras),
        rounds(r),
        thresh(t) { }

    void operator()(const bundler_inters_image_sptr &img) {
        // Get the associated vpgl camera using RANSAC
        bundler_utils_fill_persp_camera_ransac(
            img, rounds, thresh);

        img->in_recon = true;

        added_cameras->push_back(img);
    }

  private:
    std::vector<bundler_inters_image_sptr> *added_cameras;
    int rounds;
    double thresh;
};


// Adds to_add_set to the reconstruction.
void bundler_sfm_impl_add_next_images::operator()(
    const std::vector<bundler_inters_image_sptr> &to_add,

    bundler_inters_reconstruction & /*reconstruction*/,
    std::vector<bundler_inters_image_sptr> &added_cameras)
{
    // Create an image adder
    image_adder adder(
        &added_cameras,
        settings.number_ransac_rounds,
        settings.inlier_size_multiplier);

    // Add every image in the set.
    std::for_each(to_add.begin(), to_add.end(), adder);

    assert(added_cameras.size() == to_add.size());
}


//------------------------------------------------------------------------
// Returns true if the track has the right number of points that are
// observed by cameras in the reconstruction, and the maximum angle is
// no less than angle_thresh
static bool can_be_added(
    const bundler_inters_track_sptr &track,
    int num_observed_thresh,
    double angle_thresh)
{
    static const double max_angle = 180;
    int num_observed=0;

    //TODO: Check angle
    std::vector<bundler_inters_feature_sptr>::iterator f;
    for (f = track->points.begin(); f != track->points.end(); ++f) {
        if ( (*f)->image->in_recon ) { ++num_observed; }
    }

    return max_angle >= angle_thresh && num_observed >= num_observed_thresh;
}


static void add_new_track(
    bundler_inters_track_sptr &track)
{
    assert(!track->observed);

    // Count the number of points this track now observes.
    unsigned int num_observing_points = 0;

    std::vector<bundler_inters_feature_sptr>::const_iterator p;
    for (p = track->points.begin(); p != track->points.end(); ++p) {
        if ((*p)->image->in_recon) {
            ++num_observing_points;
        }
    }

    // If there are enough points to triangulate, add this track to the recon.
    if (num_observing_points >= 2)
    {
        // Mark all the points whose image is in the reconstruction
        // as contributing.
        for (p = track->points.begin(); p != track->points.end(); ++p) {
            if ((*p)->image->in_recon) {
                const int ind = p - track->points.begin();

                track->contributing_points[ind] = true;
            }
        }

        // Find the world point for this track by triangulating
        // all the contributing points.
        bundler_utils_triangulate_points(track);

        // Set this track as observed.
        track->observed = true;
    }
}


void bundler_sfm_impl_add_new_points::operator()(
    bundler_inters_reconstruction &recon,
    const std::vector<bundler_inters_image_sptr> &added)
{
    int num_points_added = 0;

    //Look at every image that was added in the last round.
    std::vector<bundler_inters_image_sptr>::const_iterator i;
    for (i = added.begin(); i != added.end(); ++i)
    {
        // Look at every feature in this image. If the track that
        // it is part of is already observed, mark the point as
        // contributing in it's track so that during bundle adjustment,
        // the new point will be considered. If the track is not observed,
        // we need to check if we should add it. We add the track if it
        // has at least two features that are part of an added image, and
        // if the triangulation is well-defined.
        std::vector<bundler_inters_feature_sptr>::iterator f;
        for (f = (*i)->features.begin(); f != (*i)->features.end(); ++f)
        {
            if ( ! (*f)->track ) {
                // Some points aren't in tracks (for instance, a view of a
                // 3D point that is only visible in a single image). Skip
                // these, since single point can't be triangulated.
                continue;
            }
            else if ( (*f)->track->observed ) {
                (*f)->mark_as_contributing();
            }
            else if (can_be_added(
                        (*f)->track,
                        settings.min_observing_images,
                        settings.min_ray_angle)) {
                add_new_track((*f)->track);
                num_points_added++;
            }
        }
    }

    std::vector<bundler_inters_track_sptr>::const_iterator t;
    for (t = recon.tracks.begin(); t != recon.tracks.end(); ++t) {
        if ( (*t)->observed) {
            bundler_utils_triangulate_points(*t);
        }
    }

    std::cout << "Added " << num_points_added << " points\n";
}


//------------------------------------------------------------------------

class track_membership_tester
{
  public:
    track_membership_tester(const bundler_inters_track_sptr &t) :
        track(t) { }

    bool operator()(const bundler_inters_feature_sptr &f) {
        return f->track == track;
    }

  private:
   bundler_inters_track_sptr track;
};

// Returns true if img contains a feature which is the projection
// of world_point, false otherwise. If such a feature exists, pt
// is filled with the feature's location in the image.
static bool find_point_in_image(
    const bundler_inters_image_sptr &img,
    const bundler_inters_track_sptr &world_point,
    vgl_point_2d<double> &pt)
{
    // Try and find the world_point in img.
    track_membership_tester pred(world_point);

    auto found =
        std::find_if (img->features.begin(), img->features.end(), pred);


    // If we found a feature, fill pt with its location in the image.
    if ( found != img->features.end()) {
        pt = (*found)->point;
    }

    return found != img->features.end();
}


// Adjusts the reconstruction using nonlinear least squares
void bundler_sfm_impl_bundle_adjust::operator()(
    bundler_inters_reconstruction &recon)
{
    //------------------------------------------------------------------
    // Get the reconstruction into something the bundle adjust routine
    // will use.

    // Get a list of the perspective cameras currently in
    // the reconstruction
    std::vector<vpgl_perspective_camera<double> > cameras;

    std::vector<bundler_inters_image_sptr>::const_iterator i;
    for (i = recon.feature_sets.begin(); i != recon.feature_sets.end(); ++i)
    {
        if ( (*i)->in_recon ) {
            cameras.push_back( (*i)->camera );
        }
    }


    // Get a list of all the currently estimated 3D points.
    std::vector<vgl_point_3d<double> > world_points;

    std::vector<bundler_inters_track_sptr>::const_iterator t;
    for (t = recon.tracks.begin(); t != recon.tracks.end(); ++t)
    {
        if ( (*t)->observed ) {
            world_points.push_back( (*t)->world_point );
        }
    }


    // Get a mask which holds visibility information and the 2d points
    //
    // mask[i][j] is true if world point j is visible in image i, false o.w.
    //
    // mask= [[is point 0 visible in cam 0, is point 0 visible in cam 1],
    //     is point 1 visible in cam 0, is point 1 visible in cam 1]]
    std::vector< vgl_point_2d<double> > image_points;

    std::vector<std::vector<bool> > mask( cameras.size(),
                                        std::vector<bool>(world_points.size(), false) );

    unsigned int camera_ind = 0; // Which entry in cameras are we on.
    for (i = recon.feature_sets.begin(); i != recon.feature_sets.end(); ++i)
    {
        // If this image is not in the reconstruction, then its
        // camera is not in the cameras list. If this is the case,
        // skip this camera.
        if ( ! (*i)->in_recon) { continue; }

        unsigned int point_ind = 0; // Which entry in world_points are we on.
        for (t = recon.tracks.begin(); t != recon.tracks.end(); ++t)
        {
            // If this track is not observed, then it is not in
            // world_points, so skip it.
            if ( ! (*t)->observed) { continue; }

            vgl_point_2d<double> image_pt;
            if ( find_point_in_image(*i, *t, image_pt) )
            {
                mask[camera_ind][point_ind] = true;
                image_points.push_back(image_pt);
            }

            // If we skipped the image, this won't get updated, since
            // we're still looking for the bundler_track that holds
            // world_points[point_ind]. But, since we're at this point in
            // the code, we've found that point, so move on.
            ++point_ind;
        }

        assert(point_ind == world_points.size());

        ++camera_ind;
    }

    assert(camera_ind == cameras.size());

    //------------------------------------------------------------------
    // Perform the bundle adjustment
    vpgl_bundle_adjust bundle_adjust;
    bundle_adjust.set_verbose(true);
    bundle_adjust.set_normalize_data(false);
    bundle_adjust.write_vrml("bundle.vrml", cameras, world_points);

    assert(bundle_adjust.optimize(cameras, world_points, image_points, mask));

    //------------------------------------------------------------------
    // Extract the information from the bundle adjustment process.
    unsigned int camera_index = 0;
    for (i = recon.feature_sets.begin(); i != recon.feature_sets.end(); ++i)
    {
        if ( (*i)->in_recon ) {
            (*i)->camera = cameras[camera_index];
            ++camera_index;
        }
    }

    assert(camera_index == cameras.size());

    unsigned int point_index = 0;
    for (t = recon.tracks.begin(); t != recon.tracks.end(); ++t)
    {
        if ( (*t)->observed ) {
            (*t)->world_point = world_points[point_index];
            ++point_index;
        }
    }

    assert(point_index == world_points.size());
}
