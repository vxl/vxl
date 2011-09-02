#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

#include <mrc/vpgl/vpgl_calibration_matrix.h>

#include <bundler/bundler_sfm_impl.h>
#include <bundler/bundler_inters.h>
#include <bundler/bundler_utils.h>

#include <mrc/vpgl/algo/vpgl_em_compute_5_point.h>
#include <mrc/vpgl/algo/vpgl_bundle_adjust.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

#include <vnl/algo/vnl_svd.h>

//-------------------------------------------------------------------------
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

// A functor used to find the match set to base the
// initial reconstruction on.
class homog_inlier_comparator
{
  public:
    homog_inlier_comparator(
        bundler_settings_create_initial_recon const& s) : settings(s) { }

    bool can_be_initial_recon(const bundler_inters_match_set &a) const;

    bool operator()(
        const bundler_inters_match_set &a,
        const bundler_inters_match_set &b) const;

  private:
    const bundler_settings_create_initial_recon settings;
};


// Returns true if a matches the criteria for being in the
// initial reconstruction. a must have a focal length from exif
// tags for both images, and have more than the minimum number of
// features.
bool homog_inlier_comparator::can_be_initial_recon(
    const bundler_inters_match_set &a) const
{
    return
        a.num_features() >= int(settings.min_number_of_matches_homography) &&
        a.image1->focal_length != BUNDLER_NO_FOCAL_LEN &&
        a.image2->focal_length != BUNDLER_NO_FOCAL_LEN;
}


// Returns true if a has fewer inliers after estimating the
// homography than b does. If a does not have enough matches, or
// doesn't have any focal lengths, but b does, then b is considered
// less. If neither fit the criteria, a and b are equal.
bool homog_inlier_comparator::operator()(
        const bundler_inters_match_set &a,
        const bundler_inters_match_set &b) const
{
    const bool a_can_be_initial_recon = can_be_initial_recon(a);
    const bool b_can_be_initial_recon = can_be_initial_recon(b);

    // There are several cases, depending on whether a and b can and
    // cannot be in the initial reconstruction.
    if (a_can_be_initial_recon != b_can_be_initial_recon)
    {
        // If one can be in the initial reconstruction, and the other
        // cannot b, then a is less than be b only if a can be in the
        // initial recon.
        return a_can_be_initial_recon;
    }
    else if (!a_can_be_initial_recon) // Means !b_can_be_initial_recon
    {
        // Neither can be in the initial recon, so a is not less than b.
        return false;
    }
    else {
        // Both can be in the initial recon, so figure out
        // which is least well modeled by a homography.

        const double thresh_sq =
            settings.inlier_threshold_homography *
            settings.inlier_threshold_homography;

        const unsigned rnds = settings.number_ransac_rounds_homography;

        return
            a.get_homography_inlier_percentage(rnds, thresh_sq) <
            b.get_homography_inlier_percentage(rnds, thresh_sq);
    }
}

//-------------------------------------------------------------------------

// Chooses two images from the set to create the initial reconstruction
bool bundler_sfm_impl_create_initial_recon::operator()(
    bundler_inters_reconstruction &recon)
{
    // First step: Find the two images to base the reconstruction on.
    // We do this by selecting the match in the set that is modeled the
    // worst by a homography (i.e. has the fewest number of inliers).
    homog_inlier_comparator comp(settings);

    vcl_vector<bundler_inters_match_set>::const_iterator best_match =
        vcl_min_element(
            recon.match_sets.begin(),
            recon.match_sets.end(),
            comp);

    // If we didn't find anything that can be in the initial reconstruction,
    // panic.
    if (!comp.can_be_initial_recon(*best_match)) {
        vcl_cerr<< "Unable to create an initial reconstruction!\n"
                << "There is not a match set that both has an initial guess"
                << " from EXIF tags and at least "
                << settings.min_number_of_matches_homography
                << " matches.\n";
        return false;
    }


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

    vcl_vector<vgl_point_2d<double> > right_points(
        best_match->num_features());
    vcl_vector<vgl_point_2d<double> > left_points(
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
    vgl_point_3d<double> camera_center_1(0, 0, 0);
    vgl_rotation_3d<double> rotation_1(0, 0, 0);

    best_match->image1->camera =
        vpgl_perspective_camera<double>(k1, camera_center_1, rotation_1);


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
        vgl_point_2d<double>(-pt_left.x(), -pt_left.y()),
        vgl_point_2d<double>(-pt_right.x(), -pt_right.y()),
        best_match->image2->camera);


    //------------------------------------------------------------------
    // Triangulate the points that both observe.
    for (int i = 0; i < best_match->num_features(); i++) 
    {
        
        bundler_inters_feature_sptr f1 = best_match->matches[i].first;
        bundler_inters_feature_sptr f2 = best_match->matches[i].second;

        // Get the track these both belong to.
        bundler_inters_track_sptr &track = 
            best_match->matches[i].first->track;

        // Set that we have visited these features.
        f1->visited = true;
        f2->visited = true;

        // Tell the images that they have a feature used to triangulate
        // a world point.
        f1->image->in_recon = true;
        f2->image->in_recon = true;

        // Tell the track to use these two features as contributing pts.
        const unsigned side1_ind = f1->index_in_track;
        const unsigned side2_ind = f2->index_in_track;

        track->contributing_points[side1_ind] = true;
        track->contributing_points[side2_ind] = true;

        track->observed = true;

        // Find the world point given these two image points
        bundler_utils_triangulate_points(track);
    }

    vcl_cout<<"Finished with the initial reconstruction!"<<vcl_endl;
    return true;
}


/*-----------------------------------------------------------------------*/
// to_check is a collection of features, and a source image. This represents
// an unprocessed camera. We want to see how many points which we have
// already processed this camera observes.
static unsigned int count_observed_points(
    const bundler_inters_image_sptr &to_check)
{
    unsigned int num_observed_pts = 0;

    vcl_vector<bundler_inters_feature_sptr>::const_iterator ii;
    for (ii = to_check->features.begin();
         ii != to_check->features.end(); ++ii)
    {
        if ((*ii)->track->observed) {
            ++num_observed_pts;
        }
    }

    return num_observed_pts;
}

// Takes in reconstruction and track_set, fills to_add as a return val.
bool bundler_sfm_default_select_next_images::operator()(
    bundler_inters_reconstruction &recon,
    vcl_vector<bundler_inters_image_sptr> &to_add)
{
    // Look at every image
    unsigned int most_observed_points = 0;
    bundler_inters_image_sptr next_image;

    vcl_vector<bundler_inters_image_sptr>::iterator i;
    for (i = recon.feature_sets.begin(); i != recon.feature_sets.end(); ++i)
    {
        // Only look at images not in the reconstruction.
        if (!(*i)->in_recon) {
            // Find the number of points that this camera views,
            // which are also viewed by a camera already added.
            const unsigned int num_observed_points = count_observed_points(*i);

            if (num_observed_points > most_observed_points) {
                next_image = *i;
                most_observed_points = num_observed_points;
            }
        }
    }

    // Because we wanted this class to be very general, we need to return
    // a vector. Put the next image to add into the vector.
    if (most_observed_points >= settings.min_number_observed_points) {
        to_add.push_back(next_image);
        return true;
    }
    else {
        return false;
    }
}

/*------------------------------------------------------------------------*/
class image_adder
{
  public:
    image_adder(
        vcl_vector<bundler_inters_image_sptr> *added_cameras,
        unsigned r,
        double t) :
        added_cameras(added_cameras),
        rounds(r),
        thresh(t) { }

    void operator()(bundler_inters_image_sptr &img) {
        // Get the associated vpgl camera using RANSAC
        bundler_utils_fill_persp_camera_ransac(
            img, rounds, thresh);

        img->in_recon = true;

        added_cameras->push_back(img);
    }

  private:
    vcl_vector<bundler_inters_image_sptr> *added_cameras;
    unsigned rounds;
    double thresh;
};


// Adds to_add_set to the reconstruction.
void bundler_sfm_default_add_next_images::operator()(
    const vcl_vector<bundler_inters_image_sptr> &to_add,

    bundler_inters_reconstruction &reconstruction,
    vcl_vector<bundler_inters_image_sptr> &added_cameras)
{
    // Create an image adder
    image_adder adder(
        &added_cameras,
        settings.number_ransac_rounds,
        settings.inlier_thresh);

    // Add every image in the set.
    vcl_for_each(
        added_cameras.begin(),
        added_cameras.end(),
        adder);
}


/*------------------------------------------------------------------------*/
//Add points that are observed by at least
void bundler_sfm_default_add_new_points::operator()(
    bundler_inters_reconstruction &reconstruction,
    const vcl_vector<bundler_inters_image_sptr> &added)
{
#if 0
    //Look at every camera that was added in the last round.
    vcl_vector<bundler_inters_camera>::const_iterator cam;
    for (cam = added.begin(); cam != added.end(); ++cam)
    {
        //Now look at every track observed by this camera. We find points
        // in the track that have not yet been added, and either create
        // a new point for them, or add them to the existing point, so that
        // the world point will be adjusted in the bundle adjust procedure.
        // We only add a new point (create a new world point) if it is
        // observed by at least two cameras, and triangulation is well-
        // conditioned.
        vcl_vector<bundler_inters_track_sptr>::const_iterator trk;
        for (trk = cam->observed_tracks.begin();
             trk != cam->observed_tracks.end(); ++trk)
        {
            //Look at every image point in this track.
            vcl_vector<bundler_inters_feature_sptr>::const_iterator pt;
            for (pt=(*trk)->points.begin(); pt!=(*trk)->points.end(); ++pt) {
                //If this point has not been visited and the track has
                // already been added, then add it to the origins for the
                // 3d point.
                if (! (*pt)->visited && (*trk)->corresponding_point) {
                    (*trk)->corresponding_point->origins.push_back(*pt);
                }
                //Otherwise, we have to check to see if we should create
                //a new 3d point for this image point
                else if (! (*pt)->visited) {
                    //TODO
                }
            }
        }
    }
#endif // 0
}


/*------------------------------------------------------------------------*/
// Adjusts the reconstruction using nonlinear least squares
void bundler_sfm_default_bundle_adjust::operator()(
    bundler_inters_reconstruction &recon)
{
#if 0
    //------------------------------------------------------------------
    // Get the reconstruction into something the bundle adjust routine
    // will use.
    //
    // mask[i*m+j] equals true if point i is visible
    // in image j , false otherwise. m := # of cameras/images
    // mask = [is point 0 visible in cam 0, is point 0 visible in cam 1,
    //     is point 1 visible in cam 0, is point 1 visible in cam 1...]
    vcl_vector<vpgl_perspective_camera<double> > cameras;
    vcl_vector<vgl_point_3d<double> > world_points;
    vcl_vector<vgl_point_2d<double> > image_points;
    vcl_vector<vcl_vector<bool> > mask;

    // Fill the cameras vector.
    vcl_vector<bundler_inters_camera>::const_iterator cam_it;
    for (cam_it = recon.cameras.begin();
         cam_it != recon.cameras.end(); ++cam_it)
    {
        cameras.push_back(cam_it->camera);
    }

    // Fill the world points, image points and mask vectors.
    vcl_vector<bundler_inters_3d_point>::const_iterator pt_it;
    for (pt_it = recon.points.begin();
         pt_it != recon.points.end(); ++pt_it)
    {
        world_points.push_back(pt_it->point_3d);
        vcl_vector<bool> pt_mask;

        // Find all cameras that observe this world point, and fill mask
        // with true or false for each camera. mask needs to be in the
        // same order as the cameras. Then, for every true in mask,
        // add the image point to image_points.
        for (cam_it = recon.cameras.begin();
             cam_it != recon.cameras.end(); ++cam_it)
        {
            // Look at every track this camera observes, push true onto
            // mask. Otherwise, push false.
            bool observes = false;

            vcl_vector<bundler_inters_track_sptr>::const_iterator trk;
            for (trk = cam_it->observed_tracks.begin();
                 trk != cam_it->observed_tracks.end(); ++trk)
            {
                //Check if this is the correct point
                if ((*trk)->corresponding_point != NULL &&
                    (*trk)->corresponding_point->point_3d == pt_it->point_3d)
                {
                    observes = true;

                    vcl_vector<bundler_inters_feature_sptr> &origins =
                        (*trk)->corresponding_point->origins;

                    // If this is the track for the point, we now need to
                    // find the image point that is the projection of the
                    // world pt.
                    for (unsigned int i = 0; i < origins.size(); ++i) {
                        if (origins[i]->source_image == cam_it->image) {
                            image_points.push_back(origins[i]->point);
                            break;
                        }
                    }
                    break;
                }
            }

            pt_mask.push_back(observes);
        }

        mask.push_back(pt_mask);
    }


#ifdef BUNDLER_DEBUG
    assert(cameras.size() == recon.cameras.size());
    assert(world_points.size() == recon.points.size());
#endif

    //------------------------------------------------------------------
    // Perform the bundle adjustment
    vpgl_bundle_adjust bundle_adjust;
    bundle_adjust.optimize(cameras, world_points, image_points, mask);

    //------------------------------------------------------------------
    // Extract the information from the bundle adjustment process.
    for (unsigned int cam_ind = 0; cam_ind < cameras.size(); ++cam_ind) {
        recon.cameras[cam_ind].camera = cameras[cam_ind];
    }

    for (unsigned int pt_ind = 0; pt_ind < world_points.size(); ++pt_ind) {
        recon.points[pt_ind].point_3d = world_points[pt_ind];
    }
#endif // 0
}
