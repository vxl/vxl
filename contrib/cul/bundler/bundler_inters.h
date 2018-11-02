#ifndef BUNDLER_INTERS_H
#define BUNDLER_INTERS_H

#define BUNDLER_DEBUG

#include <vector>
#include <iostream>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vbl/vbl_smart_ptr.hxx>
#include <vbl/vbl_ref_count.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_sparse_matrix.hxx>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_resource.h>

#include <vpgl/vpgl_perspective_camera.h>

// A magic number that indicates an image either has no exif tag, or
// the exif tag does not have a focal length
const int BUNDLER_NO_FOCAL_LEN = -1;

struct bundler_inters_track;
typedef vbl_smart_ptr<bundler_inters_track> bundler_inters_track_sptr;

struct bundler_inters_image;
typedef vbl_smart_ptr<bundler_inters_image> bundler_inters_image_sptr;


/*-----------------------------------------------------------------------*/
// A single feature. Contains a (row, col), which is the location in the
// image of the feature, and a vector of doubles, the feature descriptor.
struct bundler_inters_feature : public vbl_ref_count
{
    vgl_point_2d<double> point;
    vnl_vector<double> descriptor;

    // Pointers to other classes that hold this feature.

    // Not all features are in tracks (for example, if 3D point
    // is only visible in one image, or there is a unmatched feature).
    // Be sure to check if track is valid.
    bundler_inters_track_sptr track; // Other views of same 3D pt
    int index_in_track;

    bundler_inters_image_sptr image; // Others features from same image
    int index_in_image;

    // General purpose boolean marking this feature as seen.
    // Useful in a variety of situations, such as depth-first search.
    bool visited;


    // Constructors
    bundler_inters_feature();
    bundler_inters_feature(
        double row,
        double col,
        const vnl_vector<double> &d,
        const bundler_inters_image_sptr &img,
        int index_in_image);

    void mark_as_contributing();
    bool is_contributing() const;
};


// A smart pointer typedef, to ease up on memory management.
typedef vbl_smart_ptr<bundler_inters_feature>
    bundler_inters_feature_sptr;


/*-----------------------------------------------------------------------*/
// A set of features that all come from the same image
struct bundler_inters_image : public vbl_ref_count
{
    // Information about the actual image.
    vil_image_resource_sptr source;
    unsigned int ni;
    unsigned int nj;
    double focal_length;
    int img_id ;
    std::string img_name ;
    // All the features in this image. They will be in different tracks.
    std::vector<bundler_inters_feature_sptr> features;

    // True if at least one feature in the set is
    // been used to triangulate a track.
    bool in_recon;

    // The camera that was used to take this image.
    vpgl_perspective_camera<double> camera;

    bundler_inters_image() :
        in_recon(false),ni(0),nj(0) { }

    void remove_if_present(bundler_inters_feature_sptr const& f);
};


/*-----------------------------------------------------------------------*/
// A pair of images, and the features for
// those images. An instance of this type represents a container that
// holds two images whose features should be matched.
struct bundler_inters_image_pair
{
    bundler_inters_image_sptr f1;
    bundler_inters_image_sptr f2;
};


/*-----------------------------------------------------------------------*/
typedef
    std::pair<bundler_inters_feature_sptr, bundler_inters_feature_sptr>
    bundler_inters_feature_pair;

// A set of matched features between two images.
struct bundler_inters_match_set
{
    //--------------Public Fields
    bundler_inters_image_sptr image1, image2;
    std::vector< bundler_inters_feature_pair > matches;


    //--------------Public Functions
    bundler_inters_match_set();


    // Returns the number of features in the match.
    int num_features() const;


    // Adds a new match to the list
    void add_match(bundler_inters_feature_sptr f1,
                   bundler_inters_feature_sptr f2);

    // Deletes a match from the list
    void remove_feature(int index);
    void remove_if_present(bundler_inters_feature_sptr const& f);

    // Removes all matches from the list
    void clear();


    // Estimates a homography, and returns the percentage of inliers
    // for the estimated homography. This is cached until the match set
    // is changed by adding or removing a match.
    double get_homography_inlier_percentage(
        int num_ransac_rounds, double thresh_sq) const;


  private:
    void reset_inlier_percentage();
    mutable double inlier_percentage;
};


/*-----------------------------------------------------------------------*/
// This represents a 3-d point. It holds the features that view it,
// the location of the 3-d point, and which features have been incorporated
// into the location.
struct bundler_inters_track : public vbl_ref_count
{
    // All these points will be from different images.
    std::vector<bundler_inters_feature_sptr> points;
    std::vector<bool> contributing_points;

    vgl_point_3d<double> world_point;

    bool observed;

    bundler_inters_track() :
        observed(false) { }

    // Returns the place the feature was added.
    int add_feature(bundler_inters_feature_sptr const& f){
        points.push_back(f);
        contributing_points.push_back(false);

        return points.size() - 1;
    }

    void remove_if_present(bundler_inters_feature_sptr const& f);
};


/*-----------------------------------------------------------------------*/
// A list of feature sets, match sets, and tracks
struct bundler_inters_reconstruction
{
    std::vector<bundler_inters_image_sptr> feature_sets;
    std::vector<bundler_inters_track_sptr> tracks;

    std::vector<bundler_inters_match_set> match_sets;

    vnl_sparse_matrix<bool> visibility_matrix;
};

#endif // BUNDLER_INTERS_H
