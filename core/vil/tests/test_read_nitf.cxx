// This is core/vil/tests/test_read_nitf.cxx

//:
// \file
// \date: 2003/12/26
// \author: mlaymon

#include <vcl_cassert.h>

#include <vil/vil_open.h>
#include <vil/vil_stream.h>
#include <vil/file_formats/vil_nitf.h>

#include <vpgl/vpgl_rational_camera.h>

/**
 * Test to read NITF file using class vil_nitf.
 * NOTE: Only reads header data.
 */
static void test_read_nitf (char const* filename)
{
    static vcl_string method_name = "test_read_nitf: ";

    vcl_cout << "\n##### enter " << method_name
             << "file name = <" << filename << ">\n";

    vil_stream * input_stream = vil_open(filename, "r");

    input_stream->ref();
    input_stream->seek(0);  // RESET STREAM TO BEGINNING

    vil_nitf_file_format * file_format = new vil_nitf_file_format();

    vil_image_resource_sptr image_resource = file_format->make_input_image(input_stream);

// FIGURE OUT OUT TO TEST FOR BOOLEAN VALUE USING TESTING FRAMEWORK LATER.  MAL 20oct2003
//    Assert(image_resource != static_cast<vil_nitf_image_subheader_sptr>(0));

    if (image_resource != static_cast<vil_image_resource_sptr>(0)) {
      vcl_cout << method_name
               << "ni = " << image_resource->ni()
               << "  nj = " << image_resource->nj()
               << "  nplanes = " << image_resource->nplanes()
               << "  pixel_format = " << image_resource->pixel_format()
               << vcl_endl;
    }

    // NEED A vil_nitf_image POINTER SO WE CAN INVOKE vil_nitf_image
    // SPECIFIC METHODS, LIKE get_rational_camera_data.

    vil_nitf_image * nitf_image =
        dynamic_cast<vil_nitf_image *>(image_resource.as_pointer());

    if (nitf_image == 0) {
        vcl_cerr << method_name << "dynamic cast to vil_nitf_image failed.\n";
    }
    else {
      //  FOR DEBUGGING TO SEE IF RATIONAL CAMERA CODE WORKS.
      // CLASSES IN VIL CANNOT DEPEND ON VNL, SO MUST PASS vcl_vector,
      // THEN CONVERT TO vnl_vector TO PASS TO RATIONAL CAMERA CLASS.

        vcl_vector<double> samp_num(20);   // coefficients for sample numerator cubic
        vcl_vector<double> samp_denom(20); // coefficients for sample denominator cubic
        vcl_vector<double> line_num(20);   // coefficients for line numerator cubic
        vcl_vector<double> line_denom(20); // coefficients for line denominator cubic

        vcl_vector<double> scalex_vcl(2);  // longitude scale/offset
        vcl_vector<double> scaley_vcl(2);  // latitude scale/offset
        vcl_vector<double> scalez_vcl(2);  // height scale/offset
        vcl_vector<double> scales_vcl(2);  // line scale/offset
        vcl_vector<double> scalel_vcl(2);  // sample scale/offset

        vcl_vector<double> init_pt_vcl(3);  // centroid of the four world corner points
        vcl_vector<double> rescales_vcl(2); // line scale/offset for rescale
        vcl_vector<double> rescalel_vcl(2); // sample scale/offset for rescale

        nitf_image->get_rational_camera_data(
            samp_num, samp_denom, line_num, line_denom,
            scalex_vcl, scaley_vcl, scalez_vcl,
            scales_vcl, scalel_vcl, init_pt_vcl, rescales_vcl, rescalel_vcl,
            vpgl_rational_camera::O_SCALE(),
            vpgl_rational_camera::O_OFFSET());

        // VNL VERSION OF PARAMETERS

        vnl_matrix<double> matx(4, 20);
        for (int index = 0; index < 20; index++)
        {
            matx(0, index) = samp_num[index];
            matx(1, index) = samp_denom[index];
            matx(2, index) = line_num[index];
            matx(3, index) = line_denom[index];
        }
        vcl_cout << "Test equality of matrix with assigned values from passed vectors.\n";
        for (int index = 0; index < 20; index++)
        {
            assert(matx(0, index) == samp_num[index]);
            assert(matx(1, index) == samp_denom[index]);
            assert(matx(2, index) == line_num[index]);
            assert(matx(3, index) == line_denom[index]);
        }

        vnl_vector<double> scalex(2, scalex_vcl[0], scalex_vcl[1]);
        assert(scalex[0] == scalex_vcl[0]);
        assert(scalex[1] == scalex_vcl[1]);

        vnl_vector<double> scaley(2, scaley_vcl[0], scaley_vcl[1]);
        assert(scaley[0] == scaley_vcl[0]);
        assert(scaley[1] == scaley_vcl[1]);

        vnl_vector<double> scalez(2, scalez_vcl[0], scalez_vcl[1]);
        assert(scalez[0] == scalez_vcl[0]);
        assert(scalez[1] == scalez_vcl[1]);

        vnl_vector<double> scales(2, scales_vcl[0], scales_vcl[1]);
        assert(scales[0] == scales_vcl[0]);
        assert(scales[1] == scales_vcl[1]);

        vnl_vector<double> scalel(2, scalel_vcl[0], scalel_vcl[1]);
        assert(scalel[0] == scalel_vcl[0]);
        assert(scalel[1] == scalel_vcl[1]);

        vnl_vector<double> rescales(2, rescales_vcl[0], rescales_vcl[1]);
        assert(rescales[0] == rescales_vcl[0]);
        assert(rescales[1] == rescales_vcl[1]);

        vnl_vector<double> rescalel(2, rescalel_vcl[0], rescalel_vcl[1]);
        assert(rescalel[0] == rescalel_vcl[0]);
        assert(rescalel[1] == rescalel_vcl[1]);

        vnl_vector<double> init_pt(3, init_pt_vcl[0], init_pt_vcl[1], init_pt_vcl[2]);
        assert(init_pt[0] == init_pt_vcl[0]);
        assert(init_pt[1] == init_pt_vcl[1]);
        assert(init_pt[2] == init_pt_vcl[2]);

        vpgl_rational_camera_sptr cam =
            new vpgl_rational_camera(matx, scalex, scaley, scalez, scales, scalel);

        // THIS SHOULD WORK.  SHOULD BE CALLING METHOD IN BASE CLASS
        // vpgl_basic_camera.  USE ALTERNATIVE METHOD FOR NOW.  MAL 6nov2003
        //  cam->set_init_pt(init_pt);
        cam->set_init_pt(init_pt[0], init_pt[1], init_pt[2]);

        cam->rescale_image(rescales, rescalel);

        vcl_vector<double> UL_vcl(3);
        vcl_vector<double> UR_vcl(3);
        vcl_vector<double> LR_vcl(3);
        vcl_vector<double> LL_vcl(3);

        nitf_image->get_image_corners(UL_vcl, UR_vcl, LR_vcl, LL_vcl);

        vnl_vector<double> UL(3, UL_vcl[0], UL_vcl[1], UL_vcl[2]);
        assert(UL[0] == UL_vcl[0]);
        assert(UL[1] == UL_vcl[1]);
        assert(UL[2] == UL_vcl[2]);

        vnl_vector<double> UR(3, UR_vcl[0], UR_vcl[1], UR_vcl[2]);
        assert(UR[0] == UR_vcl[0]);
        assert(UR[1] == UR_vcl[1]);
        assert(UR[2] == UR_vcl[2]);

        vnl_vector<double> LR(3, LR_vcl[0], LR_vcl[1], LR_vcl[2]);
        assert(LR[0] == LR_vcl[0]);
        assert(LR[1] == LR_vcl[1]);
        assert(LR[2] == LR_vcl[2]);

        vnl_vector<double> LL(3, LL_vcl[0], LL_vcl[1], LL_vcl[2]);
        assert(LL[0] == LL_vcl[0]);
        assert(LL[1] == LL_vcl[1]);
        assert(LL[2] == LL_vcl[2]);

        cam->verify_camera(UL, UR, LR, LL);
    }

    input_stream->unref();  // NO NEED TO EXPLICITLY CLOSE vil_stream.  Just call unref.

    vcl_cout << "\n##### exit " << method_name
             << "file name = <" << filename << ">\n";
}  // end method test_read_nitf

//*****************************************************************************
int main (int argc, char **argv)
{
    static vcl_string method_name = "main: ";

    vcl_cout << "argc = " << argc << vcl_endl ;

    if (argc > 1) {
      for (int i = 1 ; i < argc ; ++i) { 
	vcl_cout << "argv[" << i << "] = <" << argv[i] << ">" << vcl_endl ;
	char * test_input_file = argv[i] ;
	test_read_nitf (test_input_file) ;
      }
    }
    else {
      vcl_cout << "Usage: " << argv[0] << " <input_file_name_1> [<input_file_name_2> ...]\n";
    }
}  // end main
