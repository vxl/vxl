#include <testlib/testlib_test.h>
#include <bundler/bundler_tracks_impl.h>

static const int NUM_FS = 10;

static int factorial(int n){
    int ret = 1;

    for(int i = 2; i <= n; i++){
        ret *= i;
    }
    
    return ret;
}

static int combination(int n, int k){
    return factorial(n) / (factorial(k) * factorial(n-k));
}

static bundler_inters_feature_set_sptr create_dummy_fs(int fl){
    bundler_inters_feature_set_sptr fs(new bundler_inters_feature_set);

    fs->source_image.focal_length = fl;

    return fs;
}

static bool paired(int i, int j, bundler_inters_feature_set_pair const& p){
    const bundler_inters_image &img1 = p.f1->source_image;
    const bundler_inters_image &img2 = p.f2->source_image;

    return 
        (img1.focal_length == i && img2.focal_length == j) || 
        (img1.focal_length == j && img2.focal_length == i);
}

static bool check_for_match(
    const vcl_vector<bundler_inters_feature_set_pair> &matches,
    int fl1, int fl2){

    vcl_vector<bundler_inters_feature_set_pair>::const_iterator i;
    for(i = matches.begin(); i != matches.end(); i++){
        if(paired(fl1, fl2, *i)){
            return true;
        }
    }

    return false;
}

static bool check_no_self_match(
    const vcl_vector<bundler_inters_feature_set_pair> &matches, 
    int fl){

    vcl_vector<bundler_inters_feature_set_pair>::const_iterator i;
    for(i = matches.begin(); i != matches.end(); i++){
        if(paired(fl, fl, *i)){
            return false;
        }
    }

    return true;
}

static void test_propose_matches(){
    //------------------ Create the "feature set" list. Use the focal 
    // length as an identifier.
    vcl_vector<bundler_inters_feature_set_sptr> feature_sets;

    for(int i = 1; i <= NUM_FS; i++){
        feature_sets.push_back(create_dummy_fs(i));
    }

    //------------------ Do the matching.
    vcl_vector<bundler_inters_feature_set_pair> matches;
    
    bundler_tracks_impl_propose_matches_all propose;
    propose(feature_sets, matches);
    

    //------------------ Perform consistency checks.
    TEST_EQUAL("Right number of matches",
        matches.size(),
        combination(NUM_FS, 2));
    

    // Check that i is matched with every other set but itself.
    for(int i = 1; i <= NUM_FS; i++){
        for(int j = 1; j <= NUM_FS; j++){

            if(i != j){
                vcl_stringstream str;
                str<< "Check that " << i << " is matched with " << j << ".";
                Assert(str.str(), check_for_match(matches, i, j));

            } else {
                vcl_stringstream str;
                str<< "Check that " << i << " is not matched with itself.";
                Assert(str.str(), check_no_self_match(matches, i));
            }
        }
    }
}


TESTMAIN(test_propose_matches);
