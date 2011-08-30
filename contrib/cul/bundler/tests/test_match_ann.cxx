#include <testlib/testlib_test.h>
#include <bundler/bundler_tracks_impl.h>

#include <vil/vil_load.h>

#include <vcl_cmath.h>

static const char* IMG1_PATH = 
    "/home/anjruu/vxl/contrib/cul/bundler/tests/test_data/kermit000.jpg";

static const char* IMG2_PATH = 
    "/home/anjruu/vxl/contrib/cul/bundler/tests/test_data/kermit001.jpg";

static const double TOL = .6;


static double dist(
    vnl_vector<double> const& side1, 
    vnl_vector<double> const& side2){

    double d = 0;

    for(int i = 0; i < side1.size(); i++){
        const double diff = side1[i] - side2[i];

        d += diff*diff;
    }

    return sqrt(d);
}

static void test_match_ann(int argc, char** argv){
    vil_image_resource_sptr img1; 
    vil_image_resource_sptr img2; 

    if(argc < 3){
        vcl_cerr<<"Supply a filename for the first two args!" << vcl_endl;
        TEST("test_tracks_detect", true, false);

        // TODO Get an argument into this test!
        img1 = vil_load_image_resource(IMG1_PATH, false); 
        img2 = vil_load_image_resource(IMG2_PATH, false);
        
    } else {
        // Load the image
        img1 = vil_load_image_resource(argv[1], false);
        img2 = vil_load_image_resource(argv[2], false);
    }

    // Set up the intermediate types for the matching.
    bundler_inters_feature_set_pair to_match;

    bundler_tracks_impl_detect_sift detect;
    to_match.f1 = detect(img1, -1);
    to_match.f2 = detect(img2, -1);

    // Actually do the matching.
    bundler_inters_match_set matches;

    bundler_tracks_impl_match_ann match;
    match(to_match, matches);


    // Consistency checks
    const bool correct_images = 
        (matches.image1 == to_match.f1->source_image && 
        matches.image2 == to_match.f2->source_image) ||
        (matches.image1 == to_match.f2->source_image && 
        matches.image2 == to_match.f1->source_image);
    Assert("Images match up.", correct_images);

    Assert("Sizes are the same.", 
        matches.side1.size() == matches.side2.size());

    for(int i = 0; i < matches.num_features(); i++){
        Assert("First stored image matches feature image.",
            matches.image1 == matches.side1[i]->source_image);

        Assert("Second stored image matches feature image.",
            matches.image2 == matches.side2[i]->source_image);

        const double desc_dist = dist(
            matches.side1[i]->descriptor, 
            matches.side2[i]->descriptor); 

        Assert("Matched distances are small.", desc_dist < TOL);
    }
}

TESTMAIN_ARGS(test_match_ann);
