//:
// \file
#include <iostream>
#include <cmath>
#include <sstream>
#include <string>
#include <testlib/testlib_test.h>
#include <brad/brad_image_metadata.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_random.h>


//: Test the image_metadata class
static void test_image_metadata()
{
   vnl_random rnd(1231);
   // test i/o functions
   brad_image_metadata md;
   md.sun_azimuth_ = 205;
   md.sun_elevation_ = 45;
   md.view_azimuth_ = 30;
   md.view_elevation_ = 82;
   md.sun_irradiance_ = 1350.5;
   md.number_of_bits_ = 11;
   md.satellite_name_ = "WorldView3";
   md.band_ = "MULTI";
   md.n_bands_ = 8;
   std::vector<double> abscal;
   std::vector<double> effect_band;
   for (unsigned i = 0; i < md.n_bands_; i++) {
     abscal.push_back(rnd.drand32());
     effect_band.push_back(rnd.drand32());
   }
   md.abscal_ = abscal;
   md.effect_band_width_ = effect_band;

   std::stringstream ss;
   ss << md;

   std::cout << md;

   brad_image_metadata md2;
   ss >> md2;

   double tol = 1e-5;
   TEST_NEAR("i/o test: sun_azimuth", md.sun_azimuth_,md2.sun_azimuth_,tol);
   TEST_NEAR("i/o test: sun_elevation", md.sun_elevation_, md2.sun_elevation_, tol);
   TEST_NEAR("i/o test: view_azimuth",md.view_azimuth_, md2.view_azimuth_, tol);
   TEST_NEAR("i/o test: view_elevation",md.view_elevation_, md2.view_elevation_, tol);
   TEST_NEAR("i/o test: sun_irradiance",md.sun_irradiance_, md2.sun_irradiance_, tol);
   TEST_EQUAL("i/o test: number_of_bits",md.number_of_bits_, md2.number_of_bits_);
   TEST("i/o test: satellite name", md.satellite_name_, md2.satellite_name_);
   bool succeed = true;
   for (unsigned i = 0; i < md.abscal_.size(); i++) {
     succeed = (std::fabs(md.abscal_[i] - md2.abscal_[i]) <= tol);
   }
   TEST("i/o test: absCalFactor", succeed, true);
   succeed = true;
   for (unsigned i = 0; i < md.effect_band_width_.size(); i++) {
     succeed = (std::fabs(md.effect_band_width_[i] - md2.effect_band_width_[i]) <= tol);
   }
   TEST("i/o test: effectBandWidth", succeed, true);
   md.read_band_dependent_gain_offset();
   md.read_band_dependent_solar_irradiance();

   md.print_out_radiometric_parameter();
   md.print_out_solar_irradiance();

   // test binary IO
   vsl_b_ofstream os("./metadata.bin");
   md.b_write(os);
   os.close();
   vsl_b_ifstream is("./metadata.bin");
   brad_image_metadata md_in;
   md_in.b_read(is);
   is.close();

   std::cout << "Test Binary i/o:\n";
   std::cout << "Binary i/o version:" << md.version() << std::endl;
   TEST("binary i/o version", md.version(), md_in.version());
   TEST_NEAR("binary i/o test: sun_azimuth", md.sun_azimuth_,md_in.sun_azimuth_,tol);
   TEST_NEAR("binary i/o test: sun_elevation", md.sun_elevation_, md_in.sun_elevation_, tol);
   TEST_NEAR("binary i/o test: view_azimuth",md.view_azimuth_, md_in.view_azimuth_, tol);
   TEST_NEAR("binary i/o test: view_elevation",md.view_elevation_, md_in.view_elevation_, tol);
   //TEST_NEAR("binary i/o test: gain",md.abscal_, md_in.abscal_, tol);
   //TEST_NEAR("binary i/o test: offset",md.effect_band_width_, md_in.effect_band_width_, tol);
   TEST_NEAR("binary i/o test: sun_irradiance",md.sun_irradiance_, md_in.sun_irradiance_, tol);
   TEST_EQUAL("binary i/o test: number_of_bits",md.number_of_bits_, md_in.number_of_bits_);
   TEST("binary i/o test: satellite name", md.satellite_name_, md_in.satellite_name_);

   succeed = true;
   for (unsigned i = 0; i < md.gains_.size(); i++) {
     succeed = (std::fabs(md.abscal_[i] - md2.abscal_[i]) <= tol);
   }
   TEST("binary i/o test: absCalFactor", succeed, true);

   succeed = true;
   for (unsigned i = 0; i < md.effect_band_width_.size(); i++) {
     succeed = (std::fabs(md.effect_band_width_[i] - md2.effect_band_width_[i]) <= tol);
   }
   TEST("binary i/o test: effectBandWidth", succeed, true);

   succeed = true;
   for (unsigned i = 0; i < md.gains_.size(); i++) {
     succeed = (std::fabs(md.gains_[i] - md_in.gains_[i]) <= tol);
   }
   TEST("binary i/o test: band dependent gain value", succeed, true);

   for (unsigned i = 0; i < md.offsets_.size(); i++)
     succeed = (std::fabs(md.offsets_[i] - md_in.offsets_[i]) <= tol);
   TEST("binary i/o test: band dependent offset value", succeed, true);
   for (unsigned i = 0; i < md.normal_sun_irradiance_values_.size(); i++)
     succeed = (std::fabs(md.normal_sun_irradiance_values_[i] - md_in.normal_sun_irradiance_values_[i]) <= tol);
   TEST("binary i/o test: band dependent sun irradiance value", succeed, true);

}

TESTMAIN( test_image_metadata );
