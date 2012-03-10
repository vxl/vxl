
#include <testlib/testlib_test.h>
#include <bundler/bundler_utils.h>

static const double TOL = .0001;

//Silly little test to make sure the get distinct indices works
static const int N = 10;
static const int N_ENTRIES = 2*N;

static void test_get_distinct_indices(){
    for (int i = 0; i < 5; i++){
        int idxs[N];
        bundler_utils_get_distinct_indices(N, idxs, N_ENTRIES);

        for (int j = 0; j < N; j++){
            Assert("distinct indices: good range",
                   idxs[j] >= 0 && idxs[j] < N_ENTRIES);

            for (int k = j+1; k < N; k++){
                Assert("distinct indices: unique", idxs[j]!=idxs[k]);
            }
        }
    }
}

static void test_bundler_utils(){
    test_get_distinct_indices();
}

TESTMAIN(test_bundler_utils);
