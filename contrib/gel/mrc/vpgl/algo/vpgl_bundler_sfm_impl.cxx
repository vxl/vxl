#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>

#include <vpgl/vpgl_calibration_matrix.h>

#include <vpgl/algo/vpgl_bundler_sfm_impl.h>
#include <vpgl/algo/vpgl_bundler_inters.h>
#include <vpgl/algo/vpgl_bundler_utils.h>

#include <vpgl/algo/vpgl_em_compute_5_point.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_svd.h>

// Chooses two images from the set to create the initial reconstruction
void vpgl_bundler_sfm_impl_create_initial_recon::operator()(
    vpgl_bundler_inters_track_set &track_set,
    vpgl_bundler_inters_reconstruction &reconstruction)
{
    // First step: Find the two images to base the reconstruction on.
    const vpgl_bundler_inters_match_set *best_match = NULL;
    double lowest_inlier_percentage = 1.0;

    double thresh_sq = settings.inlier_threshold_homography * 
        settings.inlier_threshold_homography;

    vcl_vector<vpgl_bundler_inters_match_set>::const_iterator ii;
    for (ii = track_set.match_sets.begin();
         ii != track_set.match_sets.end(); ii++){

        // The pair must have a lot of matches, and have a focal length
        // from EXIF tags...
        if (ii->num_features()>=settings.min_number_of_matches_homography &&
            ii->image1.focal_length != VPGL_BUNDLER_NO_FOCAL_LEN &&
            ii->image1.focal_length != VPGL_BUNDLER_NO_FOCAL_LEN){
    
            double current_inlier_percentage =
                vpgl_bundler_utils_get_homography_inlier_percentage(
                    *ii, thresh_sq, 
                    settings.number_ransac_rounds_homography);

            // ...but may not be modeled well by a homography, since this
            // means that there isn't a lot of 3D structure.
            if (current_inlier_percentage < lowest_inlier_percentage) {
                best_match = &(*ii);
                lowest_inlier_percentage = current_inlier_percentage;
            }
        }
    }

    if (!best_match) {
        vcl_cerr<<
            "Unable to create an initial reconstruction!\n" <<
            "There is not a match set that both has an initial guess " <<
            "from EXIF tags and at least " <<
            settings.min_number_of_matches_homography << " matches." <<
            '\n';

        vcl_exit(EXIT_FAILURE);
    }


    //------------------------------------------------------------------
    // We have the best match, so create two calibration matrices for
    // the image pair.
    vgl_point_2d<double> principal_point_1, principal_point_2;

    principal_point_1.x() = best_match->image1.source->ni() / 2.0;
    principal_point_1.y() = best_match->image1.source->nj() / 2.0;

    principal_point_2.x() = best_match->image2.source->ni() / 2.0;
    principal_point_2.y() = best_match->image2.source->nj() / 2.0;

    vpgl_calibration_matrix<double> k1(
        best_match->image1.focal_length, principal_point_1);

    vpgl_calibration_matrix<double> k2(
        best_match->image2.focal_length, principal_point_2);


    //------------------------------------------------------------------
    // Use the five-point algorithm wrapped in RANSAC to find the
    // relative pose.
    vpgl_essential_matrix<double> best_em;
    int best_inlier_count = 0;

    vpgl_em_compute_5_point<double> five_point;

    int match_idxs[5];
    for (int r = 0; r < settings.number_ransac_rounds_e_matrix; r++){
        // Choose 5 random points, and use the 5-point algorithm on
        // these points to find the relative pose.
        vcl_vector<vgl_point_2d<double> > right_points, left_points;

        vpgl_bundler_utils_get_distinct_indices(
            5, match_idxs, best_match->num_features());
        for (int idx = 0; idx < 5; idx++) {
            right_points.push_back(best_match->side1[idx]->point);
            left_points.push_back(best_match->side2[idx]->point);
        }

        vcl_vector<vpgl_essential_matrix<double> > ems;
        five_point.compute(right_points, k1, left_points, k2, ems);


        // Now test all the essential matrices we've found, using them as
        // RANSAC hypotheses.
        vcl_vector<vpgl_essential_matrix<double> >::const_iterator i;
        for (i = ems.begin(); i != ems.end(); i++) {
            vpgl_fundamental_matrix<double> f(k1, k2, *i);

            vnl_matrix_fixed<double, 3, 1> point_r, point_l;

            // Count the number of inliers
            int inlier_count = 0;
            for (int j = 0; j < best_match->num_features(); j++) {
                point_r.put(0, 0, best_match->side1[j]->point.x());
                point_r.put(1, 0, best_match->side1[j]->point.y());
                point_r.put(2, 0, 1.0);

                point_l.put(0, 0, best_match->side2[j]->point.x());
                point_l.put(1, 0, best_match->side2[j]->point.y());
                point_l.put(2, 0, 1.0);

                if ( (point_l.transpose()*f.get_matrix()*point_r).get(0,0)
                    <= settings.inlier_threshold_e_matrix) {
                    inlier_count++;
                }
            }

            if (best_inlier_count < inlier_count) {
                best_em = *i;
                best_inlier_count = inlier_count;
            }
        }
    }

    //------------------------------------------------------------------
    // Get the two cameras.

    // Set the right camera to be have no translation or rotation.
    vgl_point_3d<double> camera_center_1(0, 0, 0);
    vgl_rotation_3d<double> rotation_1(0, 0, 0);
    vpgl_perspective_camera<double> right_camera(
        k1, camera_center_1, rotation_1);

    // Get two normalized (focal plane) coordinates so that we can 
    // choose the direction of the left camera.
    vgl_point_2d<double> pt_left =
        k2.map_to_focal_plane(best_match->side2[0]->point);

    vgl_point_2d<double> pt_right =
        k1.map_to_focal_plane(best_match->side1[0]->point);

    // Extract the left camera from the essential matrix
    vpgl_perspective_camera<double> left_camera;
    left_camera.set_calibration(k2);
    extract_left_camera(
        best_em,
        vgl_point_2d<double>(-pt_left.x(), -pt_left.y()),
        vgl_point_2d<double>(-pt_right.x(), -pt_right.y()),
        left_camera);

    //Add the new camera to the set.
    vpgl_bundler_inters_camera
        right_inters_camera(right_camera, best_match->image1, track_set),
        left_inters_camera(left_camera, best_match->image2, track_set);

    reconstruction.cameras.push_back(right_inters_camera);
    reconstruction.cameras.push_back(left_inters_camera);
    
    //------------------------------------------------------------------
    // Triangulate the points that both observe.
    for (int i = 0; i < best_match->num_features(); i++) {
        vpgl_bundler_inters_3d_point new_point;

        // Set that we have visited these features.
        best_match->side1[i]->visited = true;
        best_match->side2[i]->visited = true;

    
        //Add these two points to the origins vector.
        new_point.origins.push_back(best_match->side1[i]);
        new_point.origins.push_back(best_match->side2[i]);


        // Find the world point given these two image points
        vpgl_bundler_utils_triangulate_points(
            new_point, reconstruction.cameras);


        // Add the point to the reconstruction
        reconstruction.points.push_back(new_point);


        // Make sure the track knows of its corresponding 3d point
        assert(best_match->side1[i]->track == best_match->side2[i]->track);
        
        best_match->side1[i]->track->corresponding_point = 
            &reconstruction.points.back();
    }

    vcl_cout<<"Finished with the initial reconstruction!"<<vcl_endl;
}


/*-----------------------------------------------------------------------*/
static bool is_not_in_list(
    vpgl_bundler_inters_feature_set_sptr to_check,
    const vcl_vector<vpgl_bundler_inters_camera> &cameras)
{
    vcl_vector<vpgl_bundler_inters_camera>::const_iterator ii;
    for (ii = cameras.begin(); ii != cameras.end(); ii++) {
        if (ii->image == to_check->source_image) return false;
    }

    return true;
}

// to_check is a collection of features, and a source image. This represents
// an unprocessed camera. We want to see how many points which we have
// already processed this camera observes. We look at every 3d point in
// the list, and check if it has been visited.
static int count_observed_points(
    vpgl_bundler_inters_feature_set_sptr to_check)
{
    int num_observed_pts = 0;

    vcl_vector<vpgl_bundler_inters_feature_sptr>::const_iterator ii;
    for (ii = to_check->features.begin(); 
         ii != to_check->features.end(); ii++)
    {
        if((*ii)->visited) num_observed_pts++;
    }

    return num_observed_pts;
}

// Takes in reconstruction and track_set, fills to_add as a return val.
bool vpgl_bundler_sfm_default_select_next_images::operator()(
    vpgl_bundler_inters_reconstruction &reconstruction,
    vpgl_bundler_inters_track_set &track_set,
    vcl_vector<vpgl_bundler_inters_feature_set_sptr> &to_add)
{
    // Look at every image
    int most_observed_points = 0;
    vpgl_bundler_inters_feature_set_sptr next_image;

    vcl_vector<vpgl_bundler_inters_feature_set_sptr>::iterator ii;
    for (ii = track_set.feature_sets.begin();
         ii != track_set.feature_sets.end(); ii++) {
        // Check to see if we've added this set before
        if (is_not_in_list(*ii, reconstruction.cameras)) {
            int currently_observed_points =
                count_observed_points(*ii);

            if (currently_observed_points > most_observed_points) {
                most_observed_points = currently_observed_points;
                next_image = *ii;
            }
        }
    }

    // Because we wanted this class to be very general, we need to return
    // a vector. Put the next image to add into the vector.
    if(most_observed_points >= settings.min_number_observed_points){
        to_add.push_back(next_image);
        return true;
    
    }else{
        return false; 
    }
}

/*------------------------------------------------------------------------*/
// Adds to_add_set to the reconstruction.
void vpgl_bundler_sfm_default_add_next_images::operator()(
    vpgl_bundler_inters_reconstruction reconstruction,
    vcl_vector<vpgl_bundler_inters_camera> &added_cameras,
    const vcl_vector<vpgl_bundler_inters_feature_set_sptr> &to_add_set) 
{

    const double thresh_sq = 
        settings.inlier_thresh * settings.inlier_thresh;
    
    vcl_vector<vpgl_bundler_inters_feature_set_sptr>::const_iterator to_add;
    for(to_add = to_add_set.begin(); to_add != to_add_set.end(); to_add++){

        //Get a list of all corresponding 3d points
        vcl_vector< vgl_point_2d<double> > image_pts;
        vcl_vector< vgl_point_3d<double> > world_pts;

        vcl_vector<vpgl_bundler_inters_track_sptr> ts;


        //Look at every feature in the set
        vcl_vector<vpgl_bundler_inters_feature_sptr>::const_iterator f;
        for(f = (*to_add)->features.begin(); 
            f != (*to_add)->features.end(); f++)
        {
            //This is the image point.
            image_pts.push_back((*f)->point);
            
            //Now get the world point
            world_pts.push_back((*f)->track->corresponding_point->point_3d);

            ts.push_back((*f)->track);
        }


        //Finally, get the associated vpgl camera using RANSAC
        int best_inliers = 0;
        vpgl_perspective_camera<double> best_camera;

        for(int rnd = 0; rnd < settings.number_ransac_rounds; rnd++){
            vcl_vector< vgl_point_2d<double> > curr_image_pts;
            vcl_vector< vgl_point_3d<double> > curr_world_pts; 


            //Get the points to use in this RANSAC round
            int match_idxs[6];
            vpgl_bundler_utils_get_distinct_indices(
                6, match_idxs, image_pts.size());

            for (int idx = 0; idx < 5; idx++) {
                curr_image_pts.push_back(image_pts[idx]);
                curr_world_pts.push_back(world_pts[idx]);
            }


            //Construct the camera from these correspondences.
            double err;
            vpgl_perspective_camera<double> camera;

            vpgl_perspective_camera_compute::compute_dlt(
                curr_image_pts, curr_world_pts, camera, err);


            // Find the inlier percentage to evaulate how good this camera
            // is.
            double inlier_count;
            for(int pt_ind = 0; pt_ind < image_pts.size(); pt_ind++){

                double u,v; 
                camera.project(world_pts[pt_ind].x(), world_pts[pt_ind].y(),
                    world_pts[pt_ind].z(), u, v);

                double dx = u - image_pts[pt_ind].x();
                double dy = v - image_pts[pt_ind].y();

                if(dx*dx - dy*dy <= thresh_sq){
                    inlier_count++;
                }
            }

            // Now see if this is the best camera so far.
            if(inlier_count > best_inliers){
                best_camera = camera;
                best_inliers = inlier_count;
            }
        }


        // From this, create the bundler camera
        vpgl_bundler_inters_camera bundler_camera(
            best_camera, (*to_add)->source_image, ts);

        reconstruction.cameras.push_back(bundler_camera);

        added_cameras.push_back(bundler_camera);
    }
}


/*------------------------------------------------------------------------*/
//Add points that are observed by at least 
void vpgl_bundler_sfm_default_add_new_points::operator()(
    vpgl_bundler_inters_reconstruction &reconstruction,
    vpgl_bundler_inters_track_set &track_set,
    const vcl_vector<vpgl_bundler_inters_camera> &added) 
{
    //Look at every camera that was added in the last round.
    vcl_vector<vpgl_bundler_inters_camera>::const_iterator cam;
    for(cam = added.begin(); cam != added.end(); cam++){

        //Now look at every track observed by this camera. We find points
        // in the track that have not yet been added, and either create
        // a new point for them, or add them to the existing point, so that
        // the world point will be adjusted in the bundle adjust procedure.
        // We only add a new point (create a new world point) if it is 
        // observed by at least two cameras, and triangulation is well-
        // conditioned.
        vcl_vector<vpgl_bundler_inters_track_sptr>::const_iterator trk;
        for(trk = cam->observed_tracks.begin(); 
            trk != cam->observed_tracks.end(); trk++)
        {
            //Look at every image point in this track.
            vcl_vector<vpgl_bundler_inters_feature_sptr>::const_iterator pt;
            for(pt=(*trk)->points.begin(); pt!=(*trk)->points.end(); pt++){
                //If this point has not been visited and the track has 
                // already been added, then add it to the origins for the
                // 3d point.
                if(not (*pt)->visited and (*trk)->corresponding_point){
                    (*trk)->corresponding_point->origins.push_back(*pt);

                }//Otherwise, we have to check to see if we should create
                //a new 3d point for this image point
                else if(not (*pt)->visited){
                    //TODO
                }
            }
        }
    }
}


/*------------------------------------------------------------------------*/
// Adjusts the reconstruction using nonlinear least squares
void vpgl_bundler_sfm_default_bundle_adjust::operator()(
    vpgl_bundler_inters_reconstruction recon) 
{
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
    vcl_vector<vpgl_bundler_inters_camera>::const_iterator cam_it;
    for(cam_it = recon.cameras.begin(); 
        cam_it != recon.cameras.end(); cam_it++)
    {
        cameras.push_back(cam_it->camera);
    }

    // Fill the world points, image points and mask vectors.
    vcl_vector<vpgl_bundler_inters_3d_point>::const_iterator pt_it;
    for(pt_it = recon.points.begin(); 
        pt_it != recon.points.end(); pt_it++)
    {
        world_points.push_back(pt_it->point_3d);
        vcl_vector<bool> pt_mask;

        // Find all cameras that observe this world point, and fill mask
        // with true or false for each camera. mask needs to be in the 
        // same order as the cameras. Then, for every true in mask,
        // add the image point to image_points.
        for(cam_it = recon.cameras.begin(); 
            cam_it != recon.cameras.end(); cam_it++)
        {
            // Look at every track this camera observes, push true onto 
            // mask. Otherwise, push false.
            bool observes = false;

            vcl_vector<vpgl_bundler_inters_track_sptr>::const_iterator trk;
            for(trk = cam_it->observed_tracks.begin(); 
                trk != cam_it->observed_tracks.end(); trk++)
            {
                //Check if this is the correct point
                if((*trk)->corresponding_point != NULL && 
                    ((*trk)->corresponding_point)->point_3d == 
                        pt_it->point_3d)
                {
                    observes = true;

                    vcl_vector<vpgl_bundler_inters_feature_sptr> &origins = 
                        (*trk)->corresponding_point->origins;
    
                    // If this is the track for the point, we now need to
                    // find the image point that is the projection of the 
                    // world pt.
                    for(int i = 0; i < origins.size(); i++){
                        if(origins[i]->source_image == cam_it->image){
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


    #ifdef VPGL_BUNDLER_DEBUG
        assert(cameras.size() == recon.cameras.size());
        assert(world_points.size() == recon.points.size());
    #endif

    //------------------------------------------------------------------
    // Perform the bundle adjustment
    vpgl_bundle_adjust bundle_adjust;
    bundle_adjust.optimize(cameras, world_points, image_points, mask);

    
    //------------------------------------------------------------------
    // Extract the information from the bundle adjustment process.
    for(int cam_ind = 0; cam_ind < cameras.size(); cam_ind++){
        recon.cameras[cam_ind].camera = cameras[cam_ind];
    }

    for(int pt_ind = 0; pt_ind < world_points.size(); pt_ind++){
        recon.points[pt_ind].point_3d = world_points[pt_ind];
    }
}
