#include <testlib/testlib_test.h>
#include <bundler/bundler_tracks_impl.h>

#include <vil/vil_load.h>

static const char* IMG1_PATH =
    "contrib/cul/bundler/tests/test_data/kermit000.jpg";

static const char* IMG2_PATH =
    "contrib/cul/bundler/tests/test_data/kermit001.jpg";

static const double TOL = .6;


static double dist_sq(
    vnl_vector<double> const& side1,
    vnl_vector<double> const& side2)
{
    double d = 0;

    for (unsigned int i = 0; i < side1.size(); ++i) {
        const double diff = side1[i] - side2[i];
        d += diff*diff;
    }

    return d;
}

static void test_match_ann(int argc, char** argv)
{
    vil_image_resource_sptr img1;
    vil_image_resource_sptr img2;

    if (argc < 3) {
        std::cerr<<"Supply a filename for the first two args!\n";
        TEST("test_match_ann", true, false);

        img1 = vil_load_image_resource(IMG1_PATH, false);
        img2 = vil_load_image_resource(IMG2_PATH, false);
    }
    else {
        // Load the image
        img1 = vil_load_image_resource(argv[1], false);
        img2 = vil_load_image_resource(argv[2], false);
    }

    // Set up the intermediate types for the matching.
    bundler_inters_image_pair to_match;

    bundler_tracks_impl_detect_sift detect;
    to_match.f1 = detect(img1, -1);
    to_match.f2 = detect(img2, -1);

    // Actually do the matching.
    bundler_inters_match_set matches;

    bundler_tracks_impl_match_ann match;
    match(to_match, matches);

    // Consistency checks
    const bool correct_images =
        (matches.image1 == to_match.f1 &&
         matches.image2 == to_match.f2) ||
        (matches.image1 == to_match.f2 &&
         matches.image2 == to_match.f1);
    Assert("Images match up.", correct_images);

    for (int i = 0; i < matches.num_features(); ++i) {
        Assert("First stored image matches feature image.",
               matches.image1 == matches.matches[i].first->image);

        Assert("Second stored image matches feature image.",
               matches.image2 == matches.matches[i].second->image);

        const double desc_dist = dist_sq(
            matches.matches[i].first->descriptor,
            matches.matches[i].second->descriptor);

        Assert("Matched distances are small.", desc_dist < TOL*TOL);
    }
}

TESTMAIN_ARGS(test_match_ann);
