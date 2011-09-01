#include <bundler/bundler_inters.h>
#include <bundler/bundler_utils.h>
#include <vcl_cassert.h>

const double INLIER_PERCENT_NOT_SET = -1.0;


//----------Bundler Inters Match Set Impls---------------------
bundler_inters_match_set::bundler_inters_match_set()
{
    inlier_percentage = INLIER_PERCENT_NOT_SET;
}

int bundler_inters_match_set::num_features() const
{
    assert(side1.size() == side2.size());
    return side1.size();
}

void bundler_inters_match_set::add_match(
    bundler_inters_feature_sptr f1,
    bundler_inters_feature_sptr f2)
{
    side1.push_back(f1);
    side2.push_back(f2);

    reset_inlier_percentage();
}


void bundler_inters_match_set::remove_feature(int index)
{
    side1.erase(side1.begin() + index);
    side2.erase(side2.begin() + index);

    reset_inlier_percentage();
}

void bundler_inters_match_set::clear()
{
    side1.clear();
    side2.clear();

    reset_inlier_percentage();
}

double bundler_inters_match_set::get_homography_inlier_percentage(
    unsigned num_rounds, double thresh_sq) const
{
    if (inlier_percentage == INLIER_PERCENT_NOT_SET)
    {
        inlier_percentage = bundler_utils_get_homography_inlier_percentage(
                *this, thresh_sq, num_rounds);
    }

    return inlier_percentage;
}

void bundler_inters_match_set::reset_inlier_percentage()
{
    inlier_percentage = INLIER_PERCENT_NOT_SET;
}
