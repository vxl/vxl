#include <vcl_vector.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_vector_io.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_interpolator_cubic.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <testlib/testlib_test.h>

static void test_vdgl_io()
{
  // Create an edgel chain...
  vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain();

  // ...with some edgels (x,y,gamma,theta)
  vdgl_edgel e1(1,2,3,4);
  vdgl_edgel e2(2,6,7,8);
  vdgl_edgel e3(6,9,3,2);
  vdgl_edgel e4(4,5,3,4);

  ec->add_edgel(e1);
  ec->add_edgel(e2);
  ec->add_edgel(e3);
  ec->add_edgel(e4);

  //Test edgel_chain binary I/O
  vcl_cout << "Test simple edgel_chain io\n";
  vsl_b_ofstream ec_out("test_edgel_chain_io.tmp");
  TEST("Created test_edgel_chain_io.tmp for writing",(!ec_out), false);
  vsl_b_write(ec_out, ec);
  ec_out.close();

  vsl_b_ifstream ec_in("test_edgel_chain_io.tmp");
  TEST("Created test_edgel_chain_io.tmp for reading",(!ec_in), false);
  vdgl_edgel_chain_sptr edgel_chain_in;
  vsl_b_read(ec_in, edgel_chain_in);
  if (edgel_chain_in)
    vcl_cout << "Read edgel_chain " << *edgel_chain_in << '\n';
  if (edgel_chain_in)
    TEST("Testing single edgel_chain io",ec && (*ec) == (*edgel_chain_in), true);

  // remove the temporary file
  vpl_unlink ("test_edgel_chain_io.tmp");

  vcl_cout << "Test simple digital_curve io\n";
  vdgl_interpolator_sptr intp = new vdgl_interpolator_linear(ec);
  vdgl_digital_curve_sptr dc = new vdgl_digital_curve(intp);
  vsl_b_ofstream dc_out("test_digital_curve_io.tmp");
  TEST("Created test_digital_curve_io.tmp for writing",(!dc_out), false);
  vsl_b_write(dc_out, dc);
  dc_out.close();

  vsl_b_ifstream dc_in("test_digital_curve_io.tmp");
  TEST("Created test_digital_curve_io.tmp for reading",(!dc_in), false);
  vdgl_digital_curve_sptr digital_curve_in;
  vsl_b_read(dc_in, digital_curve_in);
  if (digital_curve_in)
    vcl_cout << "Read digital_curve " << *digital_curve_in << '\n';
  if (digital_curve_in)
    TEST("Testing single digital_curve io",dc && (*dc) == (*digital_curve_in), true);
  dc_in.close();

  //Test vector I/O on vdgl_digital_curve(s)
  vdgl_interpolator_sptr intpc = new vdgl_interpolator_cubic(ec);
  vdgl_digital_curve_sptr dca = new vdgl_digital_curve(intpc);

  vcl_vector<vdgl_digital_curve_sptr> dcrvs, dcrvs_in;
  dcrvs.push_back(dc);   dcrvs.push_back(dca);
  vsl_b_ofstream dcv_out("test_digital_curve_io.tmp");
  vsl_b_write(dcv_out, dcrvs);
  dcv_out.close();

  vsl_b_ifstream dcv_in("test_digital_curve_io.tmp");
  vsl_b_read(dcv_in, dcrvs_in);
  dcv_in.close();

  bool good = true;
  int k = 0;
  for (vcl_vector<vdgl_digital_curve_sptr>::iterator dcit = dcrvs_in.begin();
       dcit != dcrvs_in.end(); dcit++, k++)
  {
    if (!(*dcit))
    {
      good = false;
      continue;
    }
    vcl_cout << "Saved digital_curve" << *dcrvs[k] << ' '
             << "Read digital_curve" << *(*dcit) << '\n';
    good = good && *(*dcit) == *dcrvs[k];
  }
  TEST("Testing vdgl_digital_curve vector io", good, true);

  // remove the temporary file
  vpl_unlink ("test_digital_curve_io.tmp");
}

TESTMAIN(test_vdgl_io);
