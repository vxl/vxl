// This is brl/bseg/strk/tests/test_tracking_face_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h> // for rand()
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <btol/btol_face_algs.h>
#include <brip/brip_vil1_float_ops.h>
#include <strk/strk_tracking_face_2d.h>

#if 0
static bool near_eq(double x, double y, double tol){return vcl_fabs(x-y)<tol;}
#endif

static void construct_faces(const int w, const int h, const int r,
                            vtol_face_2d_sptr& out_face,
                            vtol_face_2d_sptr& hole_face,
                            vtol_face_2d_sptr& back_face)
{
  int r0 = r;
  //Outside boundary
  vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(w-1,0.0);
  vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(w-1,h-1);
  vtol_vertex_2d_sptr v4 = new vtol_vertex_2d(0.0,h-1);
  if (r0>w/2-2)
    r0 = w/2-2;
  if (r0>h/2-2)
    r0 = h/2 -2;
  int x0 = w/2-r0, y0 = h/2-r0, xm = w/2+r0, ym = h/2 +r0;
  //Interior Hole for background face later and outside of observation
  vtol_vertex_2d_sptr vh1 = new vtol_vertex_2d(x0, y0);
  vtol_vertex_2d_sptr vh2 = new vtol_vertex_2d(xm, y0);
  vtol_vertex_2d_sptr vh3 = new vtol_vertex_2d(xm, ym);
  vtol_vertex_2d_sptr vh4 = new vtol_vertex_2d(x0, ym);

  vcl_vector<vtol_vertex_sptr> verts, hole_verts;

  verts.push_back(v1->cast_to_vertex());
  verts.push_back(v2->cast_to_vertex());
  verts.push_back(v3->cast_to_vertex());
  verts.push_back(v4->cast_to_vertex());

  out_face = new vtol_face_2d(verts);

  hole_verts.push_back(vh1->cast_to_vertex());
  hole_verts.push_back(vh2->cast_to_vertex());
  hole_verts.push_back(vh3->cast_to_vertex());
  hole_verts.push_back(vh4->cast_to_vertex());

  hole_face = new vtol_face_2d(hole_verts);

  vtol_one_chain_sptr out_chain, hole_chain;
  out_chain = btol_face_algs::one_chain(verts);
  hole_chain = btol_face_algs::one_chain(hole_verts);
  vcl_vector<vtol_one_chain_sptr> chains;
  chains.push_back(out_chain);   chains.push_back(hole_chain);
  back_face = new vtol_face_2d(chains);
}

static void test_tracking_face_2d(int argc, char* argv[])
{
  double s = 0.0625, ts = -27.5;
  vnl_matrix_fixed<double, 3, 3> L, R;
  L[0][0] = s;   L[0][1] = 0.0;   L[0][2] = ts;
  L[1][0] = 0.0;   L[1][1] = s;   L[1][2] = 0.0;
  L[2][0] = 0.0;   L[2][1] = 0.0;   L[2][2] = 1.0;

  R[0][0] = s;   R[0][1] = 0.0;   R[0][2] = 0.0;
  R[1][0] = 0.0;   R[1][1] = s;   R[1][2] = 0.0;
  R[2][0] = 0.0;   R[2][1] = 0.0;   R[2][2] = 1.0;

  vil1_image left_image = vil1_load(argc>1 ? argv[1] : "image0005_left.bmp");
  vil1_image right_image = vil1_load(argc>2 ? argv[2] : "image0005_right.bmp");
  vil1_memory_image_of<float> left_flt =
    brip_vil1_float_ops::convert_to_float(left_image);

  vil1_memory_image_of<float> right_flt =
    brip_vil1_float_ops::convert_to_float(right_image);

  vil1_memory_image_of<float> left_scaled, right_scaled;

  //bool rhomg = brip_vil1_float_ops::homography(right_flt, R, right_scaled);

  int ws = right_scaled.width(), hs = right_scaled.height();
  left_scaled.resize(ws, hs);

  brip_vil1_float_ops::homography(left_flt, L, left_scaled, true);

  vsol_box_2d_sptr roi = new vsol_box_2d();
  roi->add_point(0, 11);
  roi->add_point(ws-1,hs-1);
  vil1_memory_image_of<float> left_chip, right_chip;
  brip_vil1_float_ops::chip(left_scaled, roi, left_chip);
  brip_vil1_float_ops::chip(right_scaled, roi, right_chip);
  vil1_memory_image_of<unsigned char> left_byte =
    brip_vil1_float_ops::convert_to_byte(left_chip, 0, 255);

  vil1_memory_image_of<unsigned char> right_byte =
    brip_vil1_float_ops::convert_to_byte(right_chip, 0, 255);

  vil1_save(left_byte, argc>3 ? argv[3] : "image0005_left_scaled.bmp");
  vil1_save(right_byte, argc>4 ? argv[4] : "image0005_right_scaled.bmp");

#if 0
  vtol_one_chain_sptr outside_chain = btol_face_algs::one_chain(verts);
  vcl_vector<vtol_vertex_sptr> chain_verts;
  outside_chain->vertices(chain_verts);
  vcl_vector<vtol_one_chain_sptr> chains;
#endif // 0
  vnl_matrix_fixed<double, 3, 3> T;
  T[0][0] = 1.0;   T[0][1] = 0.0;   T[0][2] = 0.0;
  T[1][0] = 0.0;   T[1][1] = 1.0;   T[1][2] = 0.0;
  T[2][0] = 0.0;   T[2][1] = 0.0;   T[2][2] = 1.0;
  //vtol_face_2d_sptr trans_simple_f = btol_tracking_face_2d::transform(simple_f, T);

  int w = 100, h = 100;
  vil1_memory_image_of<float> model_image_uni(w, h), obs_image_uni(w, h), null;
  //fill images with random pixel values
  for (int x = 0; x<w; ++x)
    for (int y = 0; y<w; ++y)
    {
      model_image_uni(x,y) = 255.0f*float(vcl_rand()/(RAND_MAX+1.0));
    }
  for (int x = 0; x<w; ++x)
    for (int y = 0; y<w; ++y)
    {
      obs_image_uni(x,y) = 255.0f*float(vcl_rand()/(RAND_MAX+1.0));
    }
  vil1_memory_image_of<float> model_image_comb(w, h), obs_image_comb(w, h);
  //fill images random even pixel values (comb). This will test Parzen
  for (int x = 0; x<w; ++x)
    for (int y = 0; y<w; ++y)
    {
      model_image_comb(x,y) = 16.0f*int(15*(vcl_rand()/(RAND_MAX+1.0)));
    }
  for (int x = 0; x<w; ++x)
    for (int y = 0; y<w; ++y)
    {
      obs_image_comb(x,y) = 16.0f*int(15*(vcl_rand()/(RAND_MAX+1.0)));
    }

  //Testing ...
#if 0
  strk_tracking_face_2d(vtol_face_2d_sptr const& face,
                        vil1_memory_image_of<float> const& image,
                        vil1_memory_image_of<float> const& Ix,
                        vil1_memory_image_of<float> const& Iy,
                        vil1_memory_image_of<float> const& hue,
                        vil1_memory_image_of<float> const& sat,
                        const float min_gradient,
                        const float parzen_sigma);
#endif // 0
  vtol_face_2d_sptr out_f, hole_f, back_f;

  vcl_cout << "testing face constructor\n";
  for (int r = 1; r<20; ++r)
  {
    vcl_cout << "Uniform Random Distribution =\n";
    construct_faces(w, h, r, out_f, hole_f, back_f);
    strk_tracking_face_2d_sptr tf = new strk_tracking_face_2d(hole_f,
                                                              model_image_uni,
                                                              null, null,
                                                              null, null,
                                                              0, 0);
    vcl_cout << "Parzen Sigma = 0\n";
    tf->compute_mutual_information(obs_image_uni, null, null, null, null);
    double jent0 = tf->intensity_entropy();

    tf = new strk_tracking_face_2d(hole_f,
                                   model_image_uni,
                                   null, null,
                                   null, null,
                                   0, 0.5);
    vcl_cout << "Parzen Sigma = 1\n";
    tf->compute_mutual_information(obs_image_uni, null, null, null, null);
    double jent1 = tf->intensity_entropy();
    int npix = tf->Npix();
    vcl_cout << "Nsamples = "<< npix
             << "  jent0 = " << jent0 << " jent1.0 = " << jent1 << vcl_endl
             << "Random Comb Distribution =\n";

    construct_faces(w, h, r, out_f, hole_f, back_f);
    tf = new strk_tracking_face_2d(hole_f,
                                   model_image_comb,
                                   null, null,
                                   null, null,
                                   0, 0);
    vcl_cout << "Parzen Sigma = 0\n";
    tf->compute_mutual_information(obs_image_comb, null, null, null, null);
    jent0 = tf->intensity_entropy();

    tf = new strk_tracking_face_2d(hole_f,
                                   model_image_comb,
                                   null, null,
                                   null, null,
                                   0, 0.5);
    vcl_cout << "Parzen Sigma = 1\n";
    tf->compute_mutual_information(obs_image_comb, null, null, null, null);
    jent1 = tf->intensity_entropy();

    npix = tf->Npix();
    vcl_cout << "Nsamples = "<< npix
             << "  jent0 = " << jent0 << " jent1.0 = " << jent1 << vcl_endl;
  }

  vcl_cout << "\n\n\nBackground Face Tests\n";
  for (int r = 3; r<30; ++r)
  {
    construct_faces(w, h, r, out_f, hole_f, back_f);

    strk_tracking_face_2d_sptr bf = new strk_tracking_face_2d(back_f,
                                                              model_image_uni,
                                                              null, null,
                                                              null, null,
                                                              0, 0);
    //  bf->print_pixels(obs_image_uni);
    strk_tracking_face_2d_sptr obf = new strk_tracking_face_2d(hole_f,
                                   model_image_uni,
                                 null, null,
                                 null, null,
                                 0, 0);
     int ob_pix = obf->Npix(), b_pix = bf->Npix();
     vcl_cout << "Nobs = "<< ob_pix << "  Nbk = " << b_pix << '\n';
    obf->intensity_mutual_info_diff(bf, obs_image_uni, true);
  }
#if 0
  // Test mutual information
  bool compute_mutual_information(vil1_memory_image_of<float> const& image,
                                    vil1_memory_image_of<float> const& Ix,
                                    vil1_memory_image_of<float> const& Iy,
                                    vil1_memory_image_of<float> const& hue,
                                    vil1_memory_image_of<float> const& sat);
  vcl_cout << "testing info computation\n"

           << "model_intensity_entropy = " << tf->model_intensity_entropy()
           << "\nintensity_entropy = " << tf->intensity_entropy()
           << "\nintensity_joint_entropy = " <<tf->intensity_joint_entropy()
           << "\n\nmodel_gradient_entropy = " << tf->model_gradient_entropy()
           << "\ngradient_entropy = " << tf->gradient_entropy()
           << "\ngradient_joint_entropy = " << tf->gradient_joint_entropy()
           << "\n\nmodel_color_entropy = " << tf->model_color_entropy()
           << "\ncolor_entropy = " << tf->color_entropy()
           << "\ncolor_joint_entropy = " << tf->color_joint_entropy()
           << vcl_endl << vcl_endl;
  TEST("model intensity entropy ",
       near_eq(tf->model_intensity_entropy(), 4.0, 0.05), true);
  TEST("obs intensity entropy ",
       near_eq(tf->intensity_entropy(), 4.0, 0.05), true);
  TEST("obs intensity entropy ",
       near_eq(tf->intensity_joint_entropy(), 8.0, 0.05), true);
#endif // 0
}

TESTMAIN_ARGS(test_tracking_face_2d);
