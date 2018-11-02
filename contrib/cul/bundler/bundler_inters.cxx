#include <iostream>
#include <algorithm>
#include "bundler_inters.h"
//
#include <bundler/bundler_utils.h>
#include <cassert>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

const double INLIER_PERCENT_NOT_SET = -1.0;

//----------Bundler Feature Impls -------------------------
bundler_inters_feature::bundler_inters_feature()
 :  track(nullptr),
    index_in_track(-1),
    image(nullptr),
    index_in_image(-1),
    visited(false) { }

bundler_inters_feature::bundler_inters_feature(
    double row,
    double col,
    const vnl_vector<double> &d,
    const bundler_inters_image_sptr &img,
    int ind_in_img)
 :  point(row, col),
    descriptor(d),
    track(nullptr),
    index_in_track(-1),
    image(img),
    index_in_image(ind_in_img),
    visited(false) { }

void bundler_inters_feature::mark_as_contributing()
{
    assert(track);
    track->contributing_points[index_in_track] = true;
}

bool bundler_inters_feature::is_contributing() const
{
    return track && track->contributing_points[index_in_track];
}

//----------Bundler Inters Image Impls---------------------
void bundler_inters_image::remove_if_present(
    bundler_inters_feature_sptr const& f)
{
    features.erase(
        remove(features.begin(), features.end(), f),
        features.end());
}

//----------Bundler Inters Match Set Impls---------------------
bundler_inters_match_set::bundler_inters_match_set()
{
    inlier_percentage = INLIER_PERCENT_NOT_SET;
}

int bundler_inters_match_set::num_features() const
{
    return matches.size();
}

void bundler_inters_match_set::add_match(
    bundler_inters_feature_sptr f1,
    bundler_inters_feature_sptr f2)
{
    bundler_inters_feature_pair p =
        std::make_pair(f1, f2);

    matches.push_back(p);

    reset_inlier_percentage();
}


void bundler_inters_match_set::remove_feature(int index)
{
    matches.erase(matches.begin() + index);
    reset_inlier_percentage();
}


void bundler_inters_match_set::clear()
{
    matches.clear();
    reset_inlier_percentage();
}


double bundler_inters_match_set::get_homography_inlier_percentage(
    int num_rounds, double thresh_sq) const
{
    if (inlier_percentage == INLIER_PERCENT_NOT_SET)
    {
        inlier_percentage = bundler_utils_get_homography_inlier_percentage(
                *this, thresh_sq, num_rounds);
    }

    return inlier_percentage;
}

class match_remover
{
  public:
    match_remover(bundler_inters_feature_sptr const& f): f(f) { }

    bool operator()(bundler_inters_feature_pair const& p){
        return p.first == f || p.second == f;
    }

  private:
    bundler_inters_feature_sptr const& f;
};

void bundler_inters_match_set::remove_if_present(
    bundler_inters_feature_sptr const& f)
{
    match_remover remover(f);

    matches.erase(
        remove_if(matches.begin(), matches.end(), remover),
        matches.end());

    reset_inlier_percentage();
}

void bundler_inters_match_set::reset_inlier_percentage()
{
    inlier_percentage = INLIER_PERCENT_NOT_SET;
}


//----------------Tracks Implementations--------------------------
void bundler_inters_track::remove_if_present(
    bundler_inters_feature_sptr const& f)
{
    points.erase(
        remove(points.begin(), points.end(), f),
        points.end());

    contributing_points.assign(points.size(), false);
}
