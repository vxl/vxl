
#include <testlib/testlib_test.h>
#include <vpgl/algo/vpgl_bundler_utils.h>

#include <vnl/vnl_inverse.h>

static const double TOL = .0001;

//Silly little test to make sure the get distinct indices works
static const int N = 10;
static const int N_ENTRIES = 2*N;

static void test_get_distinct_indices(){
    for(int i = 0; i < 5; i++){
        int idxs[N];
        vpgl_bundler_utils_get_distinct_indices(N, idxs, N_ENTRIES);

        for(int j = 0; j < N; j++){
            Assert("distinct indices: good range", 
                idxs[j] >= 0 && idxs[j] < N_ENTRIES);

            for(int k = j+1; k < N; k++){
                Assert("distinct indices: unique", idxs[j]!=idxs[k]); 
            }
        }
    }
}


//Test get homography
static void test_get_homography(){
    vgl_point_2d<double> rhs_arr[] = {
        vgl_point_2d<double>(132,132),
        vgl_point_2d<double>(134,175),
        vgl_point_2d<double>(68,124),
        vgl_point_2d<double>(159,174)};

    vgl_point_2d<double> lhs_arr[] = {
        vgl_point_2d<double>(130,133),
        vgl_point_2d<double>(114,173),
        vgl_point_2d<double>(75,100),
        vgl_point_2d<double>(136,182)};

    vcl_vector<vgl_point_2d<double> > 
        rhs(rhs_arr, rhs_arr+sizeof(rhs_arr)/sizeof(vgl_point_2d<double>)),
        lhs(lhs_arr, lhs_arr+sizeof(lhs_arr)/sizeof(vgl_point_2d<double>));

   
    //----------
    vnl_double_3x3 homog;
    vpgl_bundler_utils_get_homography(rhs,lhs,homog); 


    for(int i = 0; i < rhs.size(); i++){
        vnl_vector_fixed<double,3> rhs_vec;
        rhs_vec[0] = rhs[i].x(); rhs_vec[1] = rhs[i].y(); rhs_vec[2] = 1.0;

        vnl_vector_fixed<double,3> lhs_vec = vnl_inverse(homog)*rhs_vec;

        TEST_NEAR("get_homography: testing that rhs*h = lhs, x",
            lhs_vec[0]/lhs_vec[2], lhs[i].x(), TOL);

        TEST_NEAR("get_homography: testing that rhs*h = lhs, y",
            lhs_vec[1]/lhs_vec[2], lhs[i].y(), TOL);
    }
}

//Test triangulation
static void test_triangulation(){

}


void test_bundler_utils(){
    test_get_distinct_indices();
    test_triangulation();
    test_get_homography();
}

TESTMAIN(test_bundler_utils);
