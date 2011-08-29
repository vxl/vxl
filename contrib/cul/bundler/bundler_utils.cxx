#include <bundler/bundler_utils.h>

#include <mrc/vpgl/algo/vpgl_triangulate_points.h>

#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>

#include <vgl/vgl_vector_3d.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_svd.h>

// Generally useful function used for RANSAC.
// Randomly chooses n distinct indices into the set
void bundler_utils_get_distinct_indices(
    int n, int *idxs, int number_entries)
{
    for (int i = 0; i < n; i++) {
        bool found;
        int idx;

        do {
            found = true;
            idx = vcl_rand() % number_entries;

            for (int j = 0; j < i; j++) {
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
    bundler_inters_3d_point &point,
    const vcl_vector<bundler_inters_camera> &cameras)
{
    // Get the cameras into a vector of perspective cameras 
    // for the triangulation function.
    vcl_vector<vpgl_perspective_camera<double> > persp_cameras;
    vcl_vector<bundler_inters_camera>::const_iterator i;
    for(i = cameras.begin(); i != cameras.end(); i++){
        persp_cameras.push_back(i->camera);
    }


    // Get the 2d points into a vector of vgl_points.
    vcl_vector<vgl_point_2d<double> > vgl_points;
    vcl_vector<bundler_inters_feature_sptr>::iterator j;
    for(j = point.origins.begin(); j != point.origins.end(); j++){
        vgl_points.push_back((*j)->point);
    }


    // Actually do the triangulation.
    vpgl_triangulate_points::triangulate(
        vgl_points, persp_cameras, point.point_3d);
}


// Estimates a homography and returns the percentage of inliers
double bundler_utils_get_homography_inlier_percentage(
    const bundler_inters_match_set &match,
    double threshold_squared, int num_rounds)
{
    int inlier_count = 0;

    // RANSAC!
    for (int round = 0; round < num_rounds; round++) {
        int match_idxs[4];
        bundler_utils_get_distinct_indices(
            4, match_idxs, match.num_features());

        // Fill these vectors with the points stored at the indices`
        vcl_vector<vgl_homg_point_2d<double> > rhs;
        vcl_vector<vgl_homg_point_2d<double> > lhs;

        for (int i = 0; i < 4; i++) {
            lhs.push_back(vgl_homg_point_2d<double>(match.side1[match_idxs[i]]->point));
            rhs.push_back(vgl_homg_point_2d<double>(match.side2[match_idxs[i]]->point));
        }

        // Get the homography for the points
        vgl_h_matrix_2d<double> homography(rhs, lhs);

        // Count the number of inliers
        vgl_homg_point_2d<double> lhs_pt, rhs_pt;

        int current_num_inliers = 0;

        vcl_vector<bundler_inters_feature_sptr>::const_iterator s1, s2;
        for (s1 = match.side1.begin(), s2 = match.side2.begin();
             s1 != match.side1.end(); s1++, s2++)
        {
            lhs_pt.set((*s1)->point.x(), (*s1)->point.y());

            rhs_pt = homography(lhs_pt);

            double dx = (rhs_pt.x() / rhs_pt.w()) - (*s2)->point.x();
            double dy = (rhs_pt.y() / rhs_pt.w()) - (*s2)->point.y();

            if (dx*dx + dy*dy <= threshold_squared) {
                current_num_inliers++;
            }
        }

        if (inlier_count < current_num_inliers) {
            inlier_count = current_num_inliers;
        }
    }

    return ((double) inlier_count) / match.side1.size();
}

