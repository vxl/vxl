#ifndef VPGL_BUNDLER_INTERS_H
#define VPGL_BUNDLER_INTERS_H

#include <vcl_vector.h>

#include <vbl/vbl_smart_ptr.txx>
#include <vbl/vbl_ref_count.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_sparse_matrix.txx>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_resource.h>

#include <vpgl/vpgl_perspective_camera.h>

// A magic number that indicates an image either has no exif tag, or
// the exif tag does not have a focal length
const int VPGL_BUNDLER_NO_FOCAL_LEN = -1;

/*-----------------------------------------------------------------------*/
struct vpgl_bundler_inters_image
{
    vil_image_resource_sptr source;
    double focal_length;

    vpgl_bundler_inters_image& operator=(
      const vpgl_bundler_inters_image &that) {
        this->source = that.source;
        this->focal_length = that.focal_length;
        return *this;
    }

    bool operator== (
      const vpgl_bundler_inters_image &that) const {
        return this->source == that.source;
    }
};

/*-----------------------------------------------------------------------*/
// A single feature. Contains a (row, col), which is the location in the
// image of the feature, and a vector of doubles, the feature descriptor.
struct vpgl_bundler_inters_feature : public vbl_ref_count
{
    vgl_point_2d<double> point; // TODO: Template the vpgl_fm_compute.
    vnl_vector<double> descriptor;
    vpgl_bundler_inters_image source_image;

    // General purpose boolean. Useful in a variety of situations, such as
    // depth-first search. TODO: Is this ENTIRELY necessary?!
    bool visited;

    vpgl_bundler_inters_feature() {}
    vpgl_bundler_inters_feature(int row, int col):
        point(col, row), visited(false) { }

    // Accessors to avoid confusion with the row == y, col == x problem.
    double& row() {return point.y();}
    double& col() {return point.x();}

    double row() const {return point.y();}
    double col() const {return point.x();}
};


// A smart pointer typedef, to ease up on memory management.
typedef vbl_smart_ptr<vpgl_bundler_inters_feature>
    vpgl_bundler_inters_feature_sptr;


/*-----------------------------------------------------------------------*/
// A set of features that all come from the same image
struct vpgl_bundler_inters_feature_set : public vbl_ref_count
{
    vcl_vector<vpgl_bundler_inters_feature_sptr> features;
    vpgl_bundler_inters_image source_image;
};

// A smart pointer typedef, to ease up on memory management.
typedef vbl_smart_ptr<vpgl_bundler_inters_feature_set>
    vpgl_bundler_inters_feature_set_sptr;


/*-----------------------------------------------------------------------*/
//  This type is a pairs of sets of images, and the features for
// those images. An instance of this type represents a container that
// holds two images whose features should be matched.
struct vpgl_bundler_inters_feature_set_pair
{
    vpgl_bundler_inters_feature_set_sptr f1;
    vpgl_bundler_inters_feature_set_sptr f2;
};


/*-----------------------------------------------------------------------*/

// A set of matched features between two images.
struct vpgl_bundler_inters_match_set
{
    vpgl_bundler_inters_image image1, image2;
    vcl_vector< vpgl_bundler_inters_feature_sptr > side1, side2;

    void add_match(vpgl_bundler_inters_feature_sptr f1,
                   vpgl_bundler_inters_feature_sptr f2) {
        side1.push_back(f1);
        side2.push_back(f2);
    }

    int num_features() const {
        return side1.size();
    }

    void remove_feature(int index) {
        side1.erase(side1.begin() + index);
        side2.erase(side2.begin() + index);
    }

    void clear() {
        side1.clear();
        side2.clear();
    }
};


/*-----------------------------------------------------------------------*/
// A list of camera, feature pairs
struct vpgl_bundler_inters_track
{
    vcl_vector<vpgl_bundler_inters_feature_sptr> points;
};

// A list of feature sets, match sets, and tracks
struct vpgl_bundler_inters_track_set
{
    vcl_vector<vpgl_bundler_inters_feature_set_sptr> feature_sets;
    vcl_vector<vpgl_bundler_inters_match_set> match_sets;
    vcl_vector<vpgl_bundler_inters_track> tracks;
};


/*-----------------------------------------------------------------------*/
struct vpgl_bundler_inters_camera
{
    vpgl_perspective_camera<double> camera;
    vpgl_bundler_inters_image image;

    vpgl_bundler_inters_camera(vpgl_perspective_camera<double> &c,
                               vpgl_bundler_inters_image i)
    : camera(c), image(i) {}
};

// This holds the current estimate of the 3d point, as well as
// all the features that are the projections of this point in the
// image.
struct vpgl_bundler_inters_3d_point
{
    vgl_point_3d<double> point_3d;
    vcl_vector<vpgl_bundler_inters_feature_sptr> origins;
};

struct vpgl_bundler_inters_reconstruction
{
    vcl_vector<vpgl_bundler_inters_camera> cameras;
    vcl_vector<vpgl_bundler_inters_3d_point> points;
    vnl_sparse_matrix<bool> visibility_graph;
};

#endif // VPGL_BUNDLER_INTERS_H
