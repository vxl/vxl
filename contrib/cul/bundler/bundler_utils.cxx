#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "bundler_utils.h"
//
#include <vpgl/algo/vpgl_triangulate_points.h>
#include <vpgl/algo/vpgl_camera_compute.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

// Generally useful function used for RANSAC.
// Randomly chooses n distinct indices into the set
void bundler_utils_get_distinct_indices(
    int n, int *idxs, int number_entries)
{
    for (int i = 0; i < n; ++i) {
        bool found;
        int idx;

        do {
            found = true;
            idx = std::rand() % number_entries;

            for (int j = 0; j < i; ++j) {
                if (idxs[j] == idx) {
                    found = false;
                    break;
                }
            }
        } while (!found);

        idxs[i] = idx;
    }
}


//----------------------------------------------------------------------
// A wrapper for vpgl_triangulate_points
double bundler_utils_triangulate_points(
    const bundler_inters_track_sptr &track)
{
    std::vector<vpgl_perspective_camera<double> > persp_cameras;
    std::vector<vgl_point_2d<double> > vgl_points;

    for (unsigned int i = 0; i < track->points.size(); ++i) {
        if (track->contributing_points[i]){
            vgl_points.push_back(track->points[i]->point);
            persp_cameras.push_back(track->points[i]->image->camera);
        }
    }

    assert(persp_cameras.size() == vgl_points.size());
    assert(persp_cameras.size() >= 2);

    // Actually do the triangulation.
    return
        vpgl_triangulate_points::triangulate(
            vgl_points, persp_cameras, track->world_point);
}


//----------------------------------------------------------------------
// Estimates a homography and returns the percentage of inliers
double bundler_utils_get_homography_inlier_percentage(
    const bundler_inters_match_set &match,
    double threshold_squared, int num_rounds)
{
    int inlier_count = 0;

    // RANSAC!
    for (int round = 0; round < num_rounds; ++round) {
        int match_idxs[4];
        bundler_utils_get_distinct_indices(
            4, match_idxs, match.num_features());

        // Fill these vectors with the points stored at the indices
        std::vector<vgl_homg_point_2d<double> > rhs;
        std::vector<vgl_homg_point_2d<double> > lhs;

        for (int match_idx : match_idxs) {
            lhs.emplace_back(
                    match.matches[match_idx].first->point);

            rhs.emplace_back(
                    match.matches[match_idx].second->point);
        }

        // Get the homography for the points
        vgl_h_matrix_2d<double> homography(rhs, lhs);

        // Count the number of inliers
        vgl_homg_point_2d<double> lhs_pt, rhs_pt;

        int current_num_inliers = 0;

        std::vector<bundler_inters_feature_pair>::const_iterator m;
        for (m = match.matches.begin(); m != match.matches.end(); ++m)
        {
            lhs_pt.set(m->first->point.x(), m->first->point.y());

            rhs_pt = homography(lhs_pt);

            double dx = (rhs_pt.x() / rhs_pt.w()) - m->second->point.x();
            double dy = (rhs_pt.y() / rhs_pt.w()) - m->second->point.y();

            if (dx*dx + dy*dy <= threshold_squared) {
                ++current_num_inliers;
            }
        }

        if (inlier_count < current_num_inliers) {
            inlier_count = current_num_inliers;
        }
    }

    return ((double) inlier_count) / match.matches.size();
}


//----------------------------------------------------------------------
void bundler_utils_fill_persp_camera_ransac(
    const bundler_inters_image_sptr &image,
    int ransac_rounds,
    double inlier_threshold)
{
    const double thresh_sq = inlier_threshold * inlier_threshold;

    // Get a list of all corresponding 3d points
    std::vector< vgl_point_2d<double> > image_pts;
    std::vector< vgl_point_3d<double> > world_pts;

    // Look at every feature in the set
    std::vector<bundler_inters_feature_sptr>::const_iterator f;
    for (f = image->features.begin(); f != image->features.end(); ++f) {
        if ((*f)->track && (*f)->track->observed){
            //This is the image point.
            image_pts.push_back((*f)->point);

            //Now get the world point
            world_pts.push_back((*f)->track->world_point);
        }
    }

    assert(world_pts.size() == image_pts.size());
    assert(world_pts.size() >= 6);

    int best_inliers = 0;
    vpgl_perspective_camera<double> camera_estimate;

    for (int rnd = 0; rnd < ransac_rounds; ++rnd) {
        std::vector< vgl_point_2d<double> > curr_image_pts(6);
        std::vector< vgl_point_3d<double> > curr_world_pts(6);

        // Get the points to use in this RANSAC round
        int match_idxs[6] = {0};
        bundler_utils_get_distinct_indices(
            6, match_idxs, image_pts.size());

        for (int idx = 0; idx < 6; idx++) {
            curr_image_pts[idx] = image_pts[match_idxs[idx]];
            curr_world_pts[idx] = world_pts[match_idxs[idx]];
        }


        //Construct the camera from these correspondences.
        double err = 0;
        vpgl_perspective_camera<double> camera;

        assert(
            vpgl_perspective_camera_compute::compute_dlt(
                curr_image_pts, curr_world_pts, camera, err) );

        // Find the inlier percentage to evaulate how good this camera
        // is.
        int inlier_count = 0;
        for (unsigned int pt_ind = 0; pt_ind < image_pts.size(); ++pt_ind) {

            if (camera.is_behind_camera(vgl_homg_point_3d<double>(world_pts[pt_ind])))
                continue; // cheirality violation

            double u, v;
            camera.project(
                world_pts[pt_ind].x(),
                world_pts[pt_ind].y(),
                world_pts[pt_ind].z(),
                u, v);

            const double dx = u - image_pts[pt_ind].x();
            const double dy = v - image_pts[pt_ind].y();

            if (dx*dx + dy*dy <= thresh_sq) {
                ++inlier_count;
            }
        }

        // Now see if this is the best camera so far.
        if (inlier_count >= best_inliers) {
            camera_estimate = camera;
            best_inliers = inlier_count;
        }
    }

    std::cout << "add_image: best_inliers: " << best_inliers << " / " << image_pts.size() << '\n';

    //------------------------------------------------------------------
    // Now that we have an estimate for the camera, re-do it into a more
    // expected form.
    vgl_point_2d<double> principal_point;
    principal_point.x() = image->source->ni() / 2.0;
    principal_point.y() = image->source->nj() / 2.0;

#if 0 // unused...
    const vpgl_calibration_matrix<double> &k =
        camera_estimate.get_calibration();
    const double observed_focal_length = .5 *
        (k.get_matrix()(0, 0) +
         k.get_matrix()(1, 1));
#endif
    vpgl_calibration_matrix<double> calibration_mat(
        image->focal_length, principal_point);

    vpgl_perspective_camera<double> actual_camera(
        calibration_mat,
        camera_estimate.get_rotation(),
        camera_estimate.get_translation());

    image->camera = actual_camera;
}
