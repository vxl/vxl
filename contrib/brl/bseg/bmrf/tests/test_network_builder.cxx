//:
// \file
#include <testlib/testlib_test.h>
#include <vcl_sstream.h>
#include <vil/vil_save.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <brip/brip_vil_float_ops.h>
#include <bmrf/bmrf_network_sptr.h>
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_node.h>
#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_network_builder_params.h>
#include <bmrf/bmrf_network_builder.h>

static float sf(const int b, const int frame, const float s0,
                const float kappa, const float gamma)
{
  return s0/((1-kappa*b)*(1-gamma*frame));
}

// To test network propagation, form a series of vertical greyscale
// bars moving at constant velocity.
static vcl_vector<vtol_edge_2d_sptr>
construct_edges(const int frame,
                const float gamma,
                bmrf_network_builder const& nb,
                const float s_start,
                const float kappa,
                const float bar_alpha_low,
                const float bar_alpha_high,
                const int n_bars)
{
  vcl_vector<vtol_edge_2d_sptr> edges;
  //number of edges is n_bars +1
  for (int b = 0; b<=n_bars; b++)
  {
    float s = sf(b, frame, s_start, kappa, gamma);
    //convert to image coordinates
    double u=0,v=0;
    nb.image_coords(bar_alpha_low,s, u, v);
    if (u<0||u>=100||v<0||v>=100)//for now JLM
      continue;
    vsol_point_2d_sptr ps = new vsol_point_2d(u,v);
    nb.image_coords(bar_alpha_high,s, u, v);
    if (u<0||u>=100||v<0||v>=100)//for now JLM
      continue;
    vsol_point_2d_sptr pe = new vsol_point_2d(u,v);
    vdgl_digital_curve_sptr dc = new vdgl_digital_curve(ps, pe);
    vtol_edge_2d_sptr edge = new vtol_edge_2d();
    edge->set_curve(*(dc->cast_to_curve()));
    edges.push_back(edge);
  }
  return edges;
}

//Fill the image
static vil_image_view<float>
frame_image(const int frame,
            const float gamma,
            bmrf_network_builder const& nb,
            const float s_start,
            const float kappa,
            const float bar_alpha_low,
            const float bar_alpha_high,
            const int n_bars,
            const int ni,//image width
            const int nj,//image height
            const float low_grey_level,
            const float high_grey_level)
{
  vil_image_view<float> image(ni, nj);
  image.fill(0.0);
  float s0 = sf(0, frame, s_start, kappa, gamma);
  bool low_grey = true;
  for (int b = 1; b<=n_bars; b++, low_grey=!low_grey)
  {
    float s = sf(b, frame, s_start, kappa, gamma);
    if (!s||vcl_fabs(s-s0)<1)
    {
      s0 = s;
      continue;
    }
    double u0_low, u_low, u0_high, u_high,
      v_low_s0, v_high_s0, v_low_s, v_high_s;
    nb.image_coords(bar_alpha_low, s0, u0_low, v_low_s0);
    nb.image_coords(bar_alpha_low, s, u_low, v_low_s);
    nb.image_coords(bar_alpha_high, s0, u0_high, v_high_s0);
    nb.image_coords(bar_alpha_high, s, u_high, v_high_s);
    int u0 = (int)((u0_low+u0_high)/2), u = (int)((u_low+u_high)/2);
    int v_low = (int)((v_low_s0+v_low_s)/2), v_high =(int)((v_high_s0+v_high_s)/2);
    for (int ui = u0; ui<=u; ui++)
      for (int vi = v_low; vi<=v_high; vi++)
      {
        if (ui<0||ui>=100||vi<0||vi>=100)//for now JLM
          continue;
        if (low_grey)
          image(ui,vi) = low_grey_level;
        else
          image(ui,vi) = high_grey_level;
      }
    s0 =s;
  }
  return image;
}

//: Test the node class
void test_network_builder()
{
  //The velocity
  float gamma = 0.1f;
  //The bar geometry
  float s_start = 110.0f;
  float kappa = 0.05f;
  float bar_alpha_low = 0.2f, bar_alpha_high = 0.8f;
  int n_bars = 3;
  //The image constants
  int ni = 100, nj = 100;
  float low_grey_level = 50.0f, high_grey_level = 150.0f;
  //Network builder Params
  bmrf_network_builder_params nbp;
  nbp.eu_=-100; // col position of the epipole
  nbp.ev_= 50;   //row position of the epipolep
  nbp.elu_ = 99; //col position of the epipolar parameter space
  nbp.elv_min_=0; // minimum row position of the epipolar space
  nbp.elv_max_=100; // maximum row position of the epipolar space
  nbp.Ns_=20;   // number of intensity samples in s of an unbounded region
  nbp.alpha_range_=0.1f;   //range in alpha for time neighbors [0, 1].
  nbp.s_range_=20.0f;  // range in s for time neighbors (pixels)

  bmrf_network_builder nb(nbp);
  nb.init();
  int n_frames=3;
  vcl_string file = "bmrf_node";
  for (int f = 0; f<n_frames; f++)
  {
    vil_image_view<float> image = frame_image(f,
                                              gamma,
                                              nbp,
                                              s_start,
                                              kappa,
                                              bar_alpha_low, bar_alpha_high,
                                              n_bars,
                                              ni, nj,
                                              low_grey_level,
                                              high_grey_level);

    vcl_vector<vtol_edge_2d_sptr> edges = construct_edges(f,
                                                          gamma,
                                                          nbp,
                                                          s_start,
                                                          kappa,
                                                          bar_alpha_low,
                                                          bar_alpha_high,
                                                          n_bars);
    nb.set_image(image);
    vil_image_view<unsigned char> temp =
      brip_vil_float_ops::convert_to_byte(image, 0.0, 255.0);

    vcl_stringstream str;
    str << f;
    vcl_string out_file = file + str.str() + ".tif";
    vil_save(temp, out_file.c_str());
    nb.set_edges(f, edges);
    nb.build();
  }
  bmrf_network_sptr net = nb.network();
  for (bmrf_network::seg_node_map::const_iterator nit = net->begin(1);
       nit != net->end(1);
       nit++)
    vcl_cout << *((*nit).first);
#if 0
  TEST("Testing build", true, true);
#endif
}


MAIN( test_network_builder )
{
  START( "bmrf_network_builder" );
  test_network_builder();
  SUMMARY();
}
