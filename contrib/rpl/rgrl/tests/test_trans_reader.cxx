#include <testlib/testlib_test.h>

#include <rgrl/rgrl_transformation_sptr.h>
#include <rgrl/rgrl_trans_reader.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_cast.h>
#include <vcl_fstream.h>

static void test_trans_reader(int argc, char* argv[])
{
  if ( argc < 2 ) {
    vcl_cerr << "Please supply one transformation file.\n";
    return;
  }

  vcl_ifstream is( argv[1], vcl_ios_in|vcl_ios_binary );
  if ( !is ) {
    vcl_cerr << "Cannot open transformation file: " << argv[2] << vcl_endl;
    return;
  }

  // read transformation
  rgrl_transformation_sptr trans_sptr = rgrl_trans_reader::read( is );

  TEST("Check validity of returned transformation", (!trans_sptr), false );

  if ( trans_sptr )
  {
    TEST("Affine type", trans_sptr->is_type( rgrl_trans_affine::type_id() ), true );

    rgrl_trans_affine* affine = rgrl_cast<rgrl_trans_affine*>(trans_sptr);
    TEST("Convert to affine type", affine!=0, true );

    vnl_vector<double> t = affine->t();
    vnl_vector<double> true_t(2);
    true_t[0] = 1.0;
    true_t[1] = -2.0;
    TEST_NEAR("Check parameters of affine transformation", (true_t-t).two_norm(), 0, 1e-12 );

    // test
    affine->write( vcl_cout );
  }
}

TESTMAIN_ARGS(test_trans_reader);
