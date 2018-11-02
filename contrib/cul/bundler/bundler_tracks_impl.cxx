#include <vector>
#include <stack>
#include <iostream>
#include "bundler_tracks_impl.h"
//
#include <bundler/bundler_inters.h>

#include <vpgl/vpgl_fundamental_matrix.h>
#include <bpgl/algo/bpgl_fm_compute_ransac.h>

#include <bapl/bapl_lowe_keypoint_sptr.h>
#include <bapl/bapl_keypoint_extractor.h>
#include <bapl/bapl_dense_sift.h>

#include <rsdl/rsdl_kd_tree.h>
#include <rsdl/rsdl_point.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static const double TOL = .01;

/*-----------------------Detect Implementation----------------------------*/

class feature_adder
{
  public:
    feature_adder(bundler_inters_image_sptr const& image) :
        image(image) { }

    void operator()(bapl_keypoint_sptr const& kp)
    {
        // Cast the key point to a bapl_lowe_keypoint. Normal keypoints
        // don't have a way of getting the location of the feature, and
        // what comes out of the extractors are all lowe kps anyway.
        bapl_lowe_keypoint_sptr lkp;
        lkp.vertical_cast(kp);

        // Create our feature
        double row = lkp->location_i();
        double col = lkp->location_j();

        bundler_inters_feature_sptr f(
            new bundler_inters_feature(
                row,
                col,
                lkp->descriptor().as_vector(),
                image,
                image->features.size()
            )
        );

        // Insert this feature into the feature set.
        image->features.push_back(f);
    }

  private:
    bundler_inters_image_sptr image;
};


static bool is_close(double a1, double a2)
{
    return a1 - a2 < TOL && a2 - a1 < TOL;
}

static bool kps_in_same_place(
    bapl_keypoint_sptr const& kp1,
    bapl_keypoint_sptr const& kp2)
{
    bapl_lowe_keypoint_sptr lkp1;
    lkp1.vertical_cast(kp1);

    bapl_lowe_keypoint_sptr lkp2;
    lkp2.vertical_cast(kp2);

    return is_close(lkp1->location_i(), lkp2->location_i()) &&
           is_close(lkp1->location_j(), lkp2->location_j());
}


// Features detect implementation. Uses SIFT to find the features and
// descriptors.
bundler_inters_image_sptr bundler_tracks_impl_detect_sift::operator ()(
    const vil_image_resource_sptr &source_image,
    const double exif_focal_len)
{
    // We are going to be using the Lowe keypoint and SIFT implementation
    // in brl/bseg/babl.
    std::vector<bapl_keypoint_sptr> keypoints;

    // First, extract all the keypoints in the image.
    bapl_keypoint_extractor(
        source_image,
        keypoints,
        settings.keypoint_curve_ratio,
        false);

    keypoints.erase(
        std::unique(keypoints.begin(), keypoints.end(), kps_in_same_place),
        keypoints.end());

    // Then we are going to convert from the bapl_lowe_keypoint
    // format to the bundler representation
    bundler_inters_image_sptr image(new bundler_inters_image);

    image->source = source_image;
    image->focal_length = exif_focal_len;

    // Add all the features to the image.
    feature_adder adder(image);
    std::for_each(keypoints.begin(), keypoints.end(), adder);

    return image;
}


/*------------------------Match List Implementation-----------------------*/
// Match list implementation. Proposes all possible pairings.
void bundler_tracks_impl_propose_matches_all::operator ()(
    const std::vector<bundler_inters_image_sptr> &features,
    std::vector<bundler_inters_image_pair> &matches)
{
    // Add every possible pairing to the match list. Make sure an image is
    // not paired with itself, and also make sure that if there is
    // (a, b) in the set, there is not (b, a).
    std::vector<bundler_inters_image_sptr>::const_iterator i, j;

    for (i = features.begin(); i != features.end(); ++i) {
        for (j = i; j != features.end(); ++j) {
            if (i != j) {
                bundler_inters_image_pair pair;

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
    const std::vector<bundler_inters_feature_sptr> &f,
    std::vector<rsdl_point> &v)
{
    std::vector<bundler_inters_feature_sptr>::const_iterator i;
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

    for (unsigned int i = 0; i < v1.num_cartesian(); ++i) {
        double tmp = (v1.cartesian(i) - v2.cartesian(i));
        dist += tmp*tmp;
    }

    return dist;
}


// Match implementation. Uses approximate nearest neighbours
// implementation (kd tree) in rpl/rsdl.
void bundler_tracks_impl_match_ann::operator ()(
    const bundler_inters_image_pair &to_match,
    bundler_inters_match_set &matches)
{
    // Add the image information to the match set.
    matches.image1 = to_match.f1;
    matches.image2 = to_match.f2;

    // Create the KD-tree
    std::vector<rsdl_point> desc_f2;
    to_rsdl_pt_vector(to_match.f2->features, desc_f2);

    rsdl_kd_tree tree(desc_f2);


    // Now do the matching
    std::vector<bundler_inters_feature_sptr>::const_iterator i;
    for (i = to_match.f1->features.begin();
         i != to_match.f1->features.end(); ++i)
    {
        // Convert to rsdl point
        rsdl_point pt((*i)->descriptor.size());
        pt.set_cartesian((*i)->descriptor);

        // Get the two closest points
        std::vector<rsdl_point> closest_pts;
        std::vector<int> indices;
        tree.n_nearest(pt, 2, closest_pts, indices);

        // Conditionally add the match to the set.
        const double sqrd_ratio_dist =
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
    std::vector<bool> &checks,
    bundler_inters_match_set &to_prune)
{
    assert(checks.size() == (unsigned int)to_prune.num_features());

    unsigned int num_removed = 0;
    for (unsigned int i = 0; i < checks.size(); ++i) {
        // If this is an outlier, remove it.
        if (checks[i]) {
            to_prune.remove_feature(i + num_removed);
            --num_removed;
        }
    }
}

// Make sure each feature only appears once in the list. Remove any
// pair that has a duplicate.
static inline void remove_all_duplicates(
    bundler_inters_match_set &matches)
{
    std::vector<bool> checks;

    for (int i = 0; i < matches.num_features(); ++i) {
        bool to_kill = false;

        for (int j = 0; j < matches.num_features(); ++j) {
            // If we are not looking at the same feature pair,
            // then say we have (a,b) and (c,d). If a is the same
            // as either c or d, or b is the same as either c or d,
            // then we need to remove i.
            if (i != j &&
                (matches.matches[i].first == matches.matches[j].first ||
                matches.matches[i].second == matches.matches[j].second ||
                matches.matches[i].first == matches.matches[j].second ||
                matches.matches[i].second == matches.matches[j].first))
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
    bpgl_fm_compute_ransac ransac;
    ransac.set_desired_prob_good(settings.probability_good_f_mat);
    ransac.set_outlier_threshold(settings.outlier_threshold);
    ransac.set_max_outlier_frac(settings.max_outlier_frac);

    // Run the ransac
    std::vector<vgl_point_2d<double> > rhs, lhs;
    for (int i = 0; i < matches.num_features(); ++i) {
        rhs.push_back(matches.matches[i].first->point);
        lhs.push_back(matches.matches[i].second->point);
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
    const std::vector<bundler_inters_match_set> &matches,
    const bundler_inters_feature_sptr &f1,
    const bundler_inters_feature_sptr &f2,
    bundler_inters_track_sptr &new_track)
{
    assert(new_track != nullptr);
    assert(!f1->visited);
    assert(!f2->visited);

    // This stack will hold all the features that have been explored
    // but whose neighbours have not been found.
    std::stack<bundler_inters_feature_sptr> feature_stack;

    // Mark both the input features as explored, and add them to the stack
    f1->visited = true;
    feature_stack.push(f1);

    f2->visited = true;
    feature_stack.push(f2);

    // Start the search
    while (! feature_stack.empty()) {
        // Get the current element from the stack
        bundler_inters_feature_sptr curr = feature_stack.top();
        feature_stack.pop();

        assert(curr->visited);

        // Add it to the track
        curr->index_in_track = new_track->add_feature(curr);
        curr->track = new_track;

        // Find neighbours with this match.
        std::vector<bundler_inters_match_set>::const_iterator match;
        for (match = matches.begin(); match != matches.end(); match++) {
            for (int i = 0; i < match->num_features(); i++) {
                if (match->matches[i].first == curr &&
                    !match->matches[i].second->visited)
                {
                    match->matches[i].second->visited = true;
                    feature_stack.push(match->matches[i].second);
                }
                else if (match->matches[i].second == curr &&
                         !match->matches[i].first->visited)
                {
                    match->matches[i].first->visited = true;
                    feature_stack.push(match->matches[i].first);
                }
            }
        }
    }
}

static void remove_all(
    const std::vector<bundler_inters_feature_sptr> &to_remove,
    std::vector<bundler_inters_match_set> &matches,
    std::vector<bundler_inters_image_sptr> &images,
    std::vector<bundler_inters_track_sptr> &tracks)
{
    std::vector<bundler_inters_feature_sptr>::const_iterator i;
    for (i = to_remove.begin(); i != to_remove.end(); ++i)
    {
        // Remove the feature from the match list.
        std::vector<bundler_inters_match_set>::iterator m;
        for (m = matches.begin(); m != matches.end(); ++m) {
            m->remove_if_present(*i);
        }

        // Remove the feature from the images
        std::vector<bundler_inters_image_sptr>::iterator img;
        for (img = images.begin(); img != images.end(); ++img) {
            (*img)->remove_if_present(*i);
        }

        // Remove the feature from the tracks.
        std::vector<bundler_inters_track_sptr>::iterator t;
        for (t = tracks.begin(); t != tracks.end(); ++t) {
            (*t)->remove_if_present(*i);
        }
    }
}

// Chain matches implementation
void bundler_tracks_impl_chain_matches::operator ()(
    std::vector<bundler_inters_match_set> &matches,
    std::vector<bundler_inters_image_sptr> &images,
    std::vector<bundler_inters_track_sptr> &tracks)
{
    std::vector<bundler_inters_match_set>::const_iterator match;
    for (match = matches.begin(); match != matches.end(); match++) {
        for (int i = 0; i < match->num_features(); i++)
        {
            assert(match->matches[i].first->visited ==
                   match->matches[i].second->visited);

            assert(match->matches[i].first->image !=
                   match->matches[i].second->image);

            // If we have not visited one of the sides, then we have
            // found a part of a new connected component, so we should
            // start the DFS search here.
            if ( ! match->matches[i].first->visited )
            {
                bundler_inters_track_sptr new_track(
                    new bundler_inters_track);

                create_new_track(
                    matches,
                    match->matches[i].first,
                    match->matches[i].second,
                    new_track);

                tracks.push_back(new_track);
            }
        }
    }


    // --------------------Clean up tracks.
    // If a track has two features from the same image, remove
    // both of those features from the track and the image that they
    // come from.

    // Keep a list of all the features to remove.
    std::vector<bundler_inters_feature_sptr> to_remove;

    std::vector<bundler_inters_track_sptr>::iterator t;
    for (t = tracks.begin(); t != tracks.end(); ++t)
    {
        // Look at every pair of points.
        for (unsigned int i = 0; i < (*t)->points.size(); ++i) {
            bool remove_i = false;

            for (unsigned int j = i+1; j < (*t)->points.size(); ++j) {
                // If we find a pair in a track that comes from the same
                // image, remove them both.
                if ((*t)->points[i]->image == (*t)->points[j]->image) {
                    to_remove.push_back( (*t)->points[j] );
                    remove_i = true;
                }
            }

            // If we've found at least one other bad feature,
            // remove the current feature too.
            if ( remove_i ) {
                to_remove.push_back((*t)->points[i]);
            }
        }
    }

    remove_all(to_remove, matches, images, tracks);


    // Make sure that the indices from the features into the tracks
    // are consistent.
    for (t = tracks.begin(); t != tracks.end(); t++) {
        for (unsigned int i = 0; i < (*t)->points.size(); i++) {
            (*t)->points[i]->index_in_track = i;
        }
    }

    // Make sure that the indices from the features into the images
    // are consistent.
    std::vector<bundler_inters_image_sptr>::iterator img;
    for (img = images.begin(); img != images.end(); ++img ) {
        for (unsigned int i = 0; i < (*img)->features.size(); ++i) {
            (*img)->features[i]->index_in_image = i;
        }
    }
}
