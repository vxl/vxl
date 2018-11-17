#include <vector>
#include <string>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_2d.h>

#include <vpgl/vpgl_essential_matrix.h>
#include <vpgl/algo/vpgl_em_compute_5_point.h>

#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_trace.h>
#include <vnl/vnl_inverse.h>

static const double TOL = .001;

static void normalize(
    const std::vector<vgl_point_2d<double> > &points,
    const vpgl_calibration_matrix<double> &k,
    std::vector<vgl_point_2d<double> > &normed_points)
{
    vnl_matrix_fixed<double, 3, 3> k_inv = vnl_inverse(k.get_matrix());

    for (auto point : points)
    {
        vnl_matrix_fixed<double, 3, 1> vec;
        vec.put(0, 0, point.x());
        vec.put(1, 0, point.y());
        vec.put(2, 0, 1);

        vnl_matrix_fixed<double,3,1> normed = k_inv*vec;

        normed_points.emplace_back(normed.get(0,0), normed.get(1,0));
    }
}

static void eval_e_mats(
    const std::vector<vpgl_essential_matrix<double> > &ems,
    const std::vector<vgl_point_2d<double> > &corres1,
    const std::vector<vgl_point_2d<double> > &corres2,
    const std::string& test_name)
{
    //Check each essential matrix.
    std::vector<vpgl_essential_matrix<double> >::const_iterator i;
    for (i = ems.begin(); i != ems.end(); ++i)
    {
        //Check that the determinant of the essential matrix is 0.
        double det = vnl_determinant(i->get_matrix());
        TEST_NEAR(
         (test_name+std::string(": Test det(essential_matrix) = 0")).c_str(),
         det, 0, TOL);

        //Now check that 2*e*e^t*e - tr(e * e^t) * e = 0;
        vnl_matrix_fixed<double, 3, 3> e_et =
            i->get_matrix() * i->get_matrix().transpose();

        double v =
            (e_et*i->get_matrix()*2.0 -
                vnl_trace(e_et)*i->get_matrix()).get(0,0);

        TEST_NEAR(
        (test_name+std::string(": Test 2*E*E^t*E - tr(E*E^t)*E = 0")).c_str(),
        v, 0, TOL);


        //Finally, test that q' * E * q = 0, where (q, q') are point
        // correspondences.
        for (unsigned int j = 0; j < corres1.size(); ++j)
        {
            //Get the RHS
            vnl_matrix_fixed<double, 3, 1> vec1;
            vec1.put(0, 0, corres1[j].x());
            vec1.put(1, 0, corres1[j].y());
            vec1.put(2, 0, 1);

            //Get the LHS
            vnl_matrix_fixed<double, 3, 1> vec2;
            vec2.put(0, 0, corres2[j].x());
            vec2.put(1, 0, corres2[j].y());
            vec2.put(2, 0, 1);

            double result =
                (vec2.transpose() * i->get_matrix() * vec1).get(0,0);

            TEST_NEAR(
            (test_name+std::string(": Testing that q' * E * q = 0")).c_str(),
            result, 0, TOL);
        }
    }
}


static void test_unnormed()
{
    //*************Definition of test stuff
    //Define the information in the first camera
    double focal_length1_mm = 5.4; //In mm
    double ccd_width1 = 5.312;
    int width1 = 640, height1 = 480;
    double focal_length1 = width1*(focal_length1_mm / ccd_width1);

    vpgl_calibration_matrix<double> k1
        (focal_length1, vgl_point_2d<double>(width1/2, height1/2));


    //Define the information in the second camera
    double focal_length2_mm = 5.4; //In mm
    double ccd_width2 = 5.312;
    int width2 = 640, height2 = 480;
    double focal_length2 = width2*(focal_length2_mm / ccd_width2);

    vpgl_calibration_matrix<double> k2
        (focal_length2, vgl_point_2d<double>(width2/2, height2/2));

    //Finally, the point correspondences
    vgl_point_2d<double> corres1_arr[] =
        {vgl_point_2d<double>(310,263),
         vgl_point_2d<double>(230,246),
         vgl_point_2d<double>(585,321),
         vgl_point_2d<double>(362,119),
         vgl_point_2d<double>(502,170)};
    std::vector<vgl_point_2d<double> > corres1(corres1_arr,
        corres1_arr + sizeof(corres1_arr) / sizeof(vgl_point_2d<double>));

    vgl_point_2d<double> corres2_arr[] =
        {vgl_point_2d<double>(274,268),
         vgl_point_2d<double>(199,240),
         vgl_point_2d<double>(582,377),
         vgl_point_2d<double>(370,114),
         vgl_point_2d<double>(544,182)};
    std::vector<vgl_point_2d<double> > corres2(corres2_arr,
        corres2_arr + sizeof(corres2_arr) / sizeof(vgl_point_2d<double>));

    Assert("Un-normed: Testing that the correspondences are the same size.",
           corres1.size() == corres2.size());
    //*************End definitions.

    //Now actually do the computation.
    std::vector<vpgl_essential_matrix<double> > ems;
    vpgl_em_compute_5_point<double> em_5_pt;

    Assert(
        "Un-normed: Testing that the 5 point algorithm returns correctly.",
        em_5_pt.compute(corres1, k1, corres2, k2, ems));

    Assert(
        "Un-normed: Testing that we found the right number of solutions.",
        ems.size() <= 10);

    std::vector<vgl_point_2d<double> > normed1, normed2;
    normalize(corres1, k1, normed1);
    normalize(corres2, k2, normed2);

    eval_e_mats(ems, normed1, normed2, std::string("Un-normed"));
}

static void test_normed()
{
    //*****************************
    //Finally, the point correspondences
    vgl_point_2d<double> corres1_arr[] =
        {vgl_point_2d<double>(-.291040, -.049485),
         vgl_point_2d<double>(-0.341221,0.060139),
         vgl_point_2d<double>(0.138513,0.329644),
         vgl_point_2d<double>(-0.028314,0.155250),
         vgl_point_2d<double>(0.057233,0.067887)};
    std::vector<vgl_point_2d<double> > corres1(corres1_arr,
        corres1_arr + sizeof(corres1_arr) / sizeof(vgl_point_2d<double>));

    vgl_point_2d<double> corres2_arr[] =
        {vgl_point_2d<double>(-0.373122,-0.034761),
         vgl_point_2d<double>(-0.425709,-0.102118),
         vgl_point_2d<double>(0.105493,0.307611),
         vgl_point_2d<double>(-0.065042,0.175362),
         vgl_point_2d<double>(0.014028,0.086819)};
    std::vector<vgl_point_2d<double> > corres2(corres2_arr,
        corres2_arr + sizeof(corres2_arr) / sizeof(vgl_point_2d<double>));

    Assert("Normed: Testing that the correspondences are the same size.",
           corres1.size() == corres2.size());
    //*************End definitions.

    //Now actually do the computation.
    std::vector<vpgl_essential_matrix<double> > ems;
    vpgl_em_compute_5_point<double> em_5_pt;

    Assert("Normed: Testing that the 5 point algorithm returns correctly.",
           em_5_pt.compute(corres1, corres2, ems));

    Assert("Normed: Testing that we found the right number of solutions.",
           ems.size() <= 10);

    eval_e_mats(ems, corres1, corres2, std::string("Normed"));
}

static void test_em_compute_5_point()
{
    test_normed();
    test_unnormed();
}

TESTMAIN(test_em_compute_5_point);
