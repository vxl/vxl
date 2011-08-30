#include <bundler/bundler_tracks_impl.h>
#include <bundler/bundler_inters.h>

#include <mrc/vpgl/vpgl_fundamental_matrix.h>
#include <mrc/vpgl/algo/vpgl_fm_compute_ransac.h>

#include <bapl/bapl_lowe_keypoint_sptr.h>
#include <bapl/bapl_keypoint_extractor.h>
#include <bapl/bapl_dense_sift.h>

#include <rsdl/rsdl_kd_tree.h>
#include <rsdl/rsdl_point.h>

#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_stack.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

/*-----------------------Detect Implementation----------------------------*/
// Features detect implementation. Uses SIFT to find the features and
// descriptors.
//
// \todo Need to correctly allow for rgb images. Possibly involves
//   modifying bapl instead of this routine.
bundler_inters_feature_set_sptr
bundler_tracks_impl_detect_sift::operator ()(
    const vil_image_resource_sptr &image,
    const double exif_focal_len)
{
    // We are going to be using the Lowe keypoint and SIFT implementation
    // in brl/bseg/babl.
    vcl_vector<bapl_keypoint_sptr> keypoints;

    // First, extract all the keypoints in the image.
    bapl_keypoint_extractor(image, keypoints,
                            settings.keypoint_curve_ratio, false);

    // Then we are going to convert from the bapl_lowe_keypoint
    // format to the bundler representation
    bundler_inters_feature_set_sptr feature_set(
        new bundler_inters_feature_set);

    feature_set->source_image.source = image;
    feature_set->source_image.focal_length = exif_focal_len;

    vcl_vector<bapl_keypoint_sptr>::const_iterator kp;
    for (kp = keypoints.begin(); kp != keypoints.end(); ++kp)
    {
        // Cast the key point to a bapl_lowe_keypoint. Normal keypoints
        // don't have a way of getting the location of the feature, and
        // what comes out of the extractors are all lowe kps anyway.
        bapl_lowe_keypoint_sptr lkp;
        lkp.vertical_cast(*kp);

        // Create our feature
        int row = int(lkp->location_i()+0.5);
        int col = int(lkp->location_j()+0.5);

        bundler_inters_feature_sptr f(
            new bundler_inters_feature(row, col));
        f->descriptor = lkp->descriptor().as_vector();
        f->source_image = feature_set->source_image;
        f->feature_set = feature_set;

        // Insert this feature into the feature set.
        feature_set->features.push_back(f);
    }

    return feature_set;
}


/*------------------------Match List Implementation-----------------------*/
// Match list implementation. Proposes all possible pairings.
void bundler_tracks_impl_propose_matches_all::operator ()(
    const vcl_vector<bundler_inters_feature_set_sptr> &features,
    vcl_vector<bundler_inters_feature_set_pair> &matches)
{
    // Add every possible pairing to the match list. Make sure an image is
    // not paired with itself, and also make sure that if there is
    // (a, b) in the set, there is not (b, a).
    vcl_vector<bundler_inters_feature_set_sptr>::const_iterator i, j;

    for (i = features.begin(); i != features.end(); i++) {
        for (j = i; j != features.end(); j++) {
            if (i != j) {
                bundler_inters_feature_set_pair pair;

                pair.f1 = *i;
                pair.f2 = *j;

                matches.push_back(pair);
            }
        }
    }
}


/*-----------------------Match Implementation-----------------------------*/
// Internal function used in the matching implementation. Converts
// a set of features to a vector of rsdl_points to use in the
// rsdl_kd_tree approximate nearest neighbours function.
static void to_rsdl_pt_vector(
    const vcl_vector<bundler_inters_feature_sptr> &f,
    vcl_vector<rsdl_point> &v)
{
    vcl_vector<bundler_inters_feature_sptr>::const_iterator i;
    for (i = f.begin(); i != f.end(); ++i)
    {
        rsdl_point pt((*i)->descriptor.size());
        pt.set_cartesian((*i)->descriptor);

        v.push_back(pt);
    }
}


// Returns the squared distance from vector 1 to vector 2.
static double squared_distance(const rsdl_point &v1,
                               const rsdl_point &v2)
{
    assert(v1.num_cartesian() == v2.num_cartesian());

    double dist = 0.0f;

    for (int i = 0; i < v1.num_cartesian(); i++) {
        double tmp = (v1.cartesian(i) - v2.cartesian(i));
        dist += tmp*tmp;
    }

    return dist;
}


// Match implementation. Uses approximate nearest neighbours
// implementation (kd tree) in rpl/rsdl.
void bundler_tracks_impl_match_ann::operator ()(
    const bundler_inters_feature_set_pair &to_match,
    bundler_inters_match_set &matches)
{
    // Add the image information to the match set.
    matches.image1 = to_match.f1->source_image;
    matches.image2 = to_match.f2->source_image;

    // Create the KD-tree
    vcl_vector<rsdl_point> desc_f2;
    to_rsdl_pt_vector(to_match.f2->features, desc_f2);

    rsdl_kd_tree tree(desc_f2);


    // Now do the matching
    vcl_vector<bundler_inters_feature_sptr>::const_iterator i;
    for (i = to_match.f1->features.begin();
         i != to_match.f1->features.end(); ++i)
    {
        // Convert to rsdl point
        rsdl_point pt((*i)->descriptor.size());
        pt.set_cartesian((*i)->descriptor);

        // Get the two closest points
        vcl_vector<rsdl_point> closest_pts;
        vcl_vector<int> indices;
        tree.n_nearest(pt, 2, closest_pts, indices);

        // Conditionally add the match to the set.
        double sqrd_ratio_dist =
            squared_distance(pt, closest_pts[0]) /
            squared_distance(pt, closest_pts[1]);

        if (sqrd_ratio_dist <=
            settings.min_dists_ratio*settings.min_dists_ratio)
        {
            // indices of 0 will hold the index of the closest point
            matches.add_match(*i, to_match.f2->features[indices[0]]);
        }
    }
}


/*--------------------Refine Implementation-------------------------------*/

// This templated class takes in a vector of bools, and a vector of Ts.
// It deleted every element in the Ts vector whose corresponding element
// in the bool vector is true. Checks that the vectors are the same size.
static inline void remove_if_true(
    vcl_vector<bool> &checks,
    bundler_inters_match_set &to_prune)
{
    assert(checks.size() == to_prune.num_features());

    int num_removed = 0;
    for (int i = 0; i < checks.size(); i++) {
        // If this is an outlier, remove it.
        if (checks[i]) {
            to_prune.remove_feature(i + num_removed);
            num_removed++;
        }
    }
}

// Make sure each feature only appears once in the list. Remove any
// pair that has a duplicate.
static inline void remove_all_duplicates(
    bundler_inters_match_set &matches)
{
    vcl_vector<bool> checks;

    for (int i = 0; i < matches.num_features(); i++)
    {
        bool to_kill = false;
        for (int j = 0; j < matches.num_features(); j++)
        {
            // If we are not looking at the same feature pair,
            // then say we have (a,b) and (c,d). If a is the same
            // as either c or d, or b is the same as either c or d,
            // then we need to remove i.
            if (i != j && (
                matches.side1[i] == matches.side1[j] ||
                matches.side1[i] == matches.side2[j] ||
                matches.side2[i] == matches.side1[j] ||
                matches.side2[i] == matches.side2[j]))
            {
                to_kill = true;
                break;
            }
        }

        checks.push_back(to_kill);
    }

    remove_if_true(checks, matches);
}

// Refine implementation.
void bundler_tracks_impl_refine_epipolar::operator ()(
    bundler_inters_match_set &matches)
{
    // First, remove any matches where feature a in image 1 matches to
    // both feature b and c in image 2. In other words, a feature
    // may only appear once in this list.
     remove_all_duplicates(matches);

    if (matches.num_features() < settings.min_inliers) {
        matches.clear();
        return;
    }

    // Now, we will estimate an F matrix, and use it to get rid of spurious
    // matches. There is a function to do it for us, which rocks.

    // Get the rhs and lhs vectors
    // Create the ransac problem solver
    vpgl_fm_compute_ransac ransac;
    ransac.set_desired_prob_good(settings.probability_good_f_mat);
    ransac.set_outlier_threshold(settings.outlier_threshold);
    ransac.set_max_outlier_frac(settings.max_outlier_frac);

    // Run the ransac
    vcl_vector<vgl_point_2d<double> > rhs, lhs;
    for (int i = 0; i < matches.num_features(); i++) {
        rhs.push_back(matches.side1[i]->point);
        lhs.push_back(matches.side2[i]->point);
    }

    vpgl_fundamental_matrix<double> fm;
    ransac.compute(rhs, lhs, fm);

    // We'll ignore the fundamental matrix, and just look at the outliers.
    remove_if_true(ransac.outliers, matches);

    // Finally, remove everything if there are fewer than the minimum number
    // of inliers.
    if (matches.num_features() < settings.min_inliers) {
        matches.clear();
    }
}


/*------------------------------------------------------------------------*/

// Use DFS to find all features in this track
static void create_new_track(
    const vcl_vector<bundler_inters_match_set> &matches,
    bundler_inters_feature_sptr f1,
    bundler_inters_feature_sptr f2,
    bundler_inters_track_sptr &new_track)
{
    // This stack will hold all the features that have been explored
    // but whose neighbours have not been found.
    vcl_stack<bundler_inters_feature_sptr> feature_stack;

    // Mark both the input features as explored, and add them to the stack
    f1->visited = true;
    feature_stack.push(f1);

    f2->visited = true;
    feature_stack.push(f2);

    // Start the search
    while (! feature_stack.empty()) {
        // Get the current element from the stack
        bundler_inters_feature_sptr curr;
        curr = feature_stack.top();
        feature_stack.pop();

        // Add it to the track
        new_track->points.push_back(curr);
        curr->track = new_track;

        vcl_vector<bundler_inters_match_set>::const_iterator match;
        for (match = matches.begin(); match != matches.end(); match++)
        {
            for (int i = 0; i < match->num_features(); i++) {
                if (match->side1[i] == curr &&
                    ! match->side2[i]->visited)
                {
                    match->side2[i]->visited = true;
                    feature_stack.push(match->side2[i]);
                }
                else if (match->side2[i] == curr &&
                         ! match->side1[i]->visited)
                {
                    match->side1[i]->visited = true;
                    feature_stack.push(match->side1[i]);
                }
            }
        }
    }
}

// Chain matches implementation
void bundler_tracks_default_chain_matches::operator ()(
    const vcl_vector<bundler_inters_match_set> &matches,
    vcl_vector<bundler_inters_track_sptr> &tracks)
{
    vcl_vector<bundler_inters_match_set>::const_iterator match;
    for (match = matches.begin(); match != matches.end(); match++)
    {
        for (int i = 0; i < match->num_features(); i++)
        {
            // If we have not visited one of the sides, then we have
            // found a part of a new connected component, so we should
            // start the DFS search here.
            if (! match->side1[i]->visited) {
                bundler_inters_track_sptr new_track(
                    new bundler_inters_track);
                create_new_track(matches, match->side1[i],
                                 match->side2[i], new_track);

                tracks.push_back(new_track);
            }
        }
    }
}
