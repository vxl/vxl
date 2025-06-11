#include <iomanip>
#include <iostream>
#include <vector>

#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vnl/vnl_math.h"
#include "vpgl/vpgl_RSM_camera.h"

// ----- GROUND DOMAIN -----

// create ground domain
// - "G" geodetic ground domain with longitude on range (-pi, pi)
// - "H" geodetic ground domain with longitude on range (0, 2pi)
// - "R-identity" rectangular ground domain with identity transform
// - "R" rectangular ground domain
vpgl_ground_domain<double>
create_ground_domain(std::string ground_domain_id)
{
  if (ground_domain_id == "R-transform")
  {
    // rectangular transformation
    vnl_vector_fixed<double, 3> translation;
    translation[0] = -2.42965895449297E+06;  // RSMIDA.XUOR
    translation[1] = -4.76049894293300E+06;  // RSMIDA.YUOR
    translation[2] = +3.46898407315533E+06;  // RSMIDA.ZUOR

    vnl_matrix_fixed<double, 3, 3> rotation;
    rotation[0][0] = +8.90233155120443E-01;  // RSMIDA.XUXR
    rotation[1][0] = +2.50327118321895E-01;  // RSMIDA.XUYR
    rotation[2][0] = -3.80553890213932E-01;  // RSMIDA.XUZR

    rotation[0][1] = -4.55502457571841E-01;  // RSMIDA.YUXR
    rotation[1][1] = +4.86367706250322E-01;  // RSMIDA.YUYR
    rotation[2][1] = -7.45629911861651E-01;  // RSMIDA.YUZR

    rotation[0][2] = -1.56226448294838E-03;  // RSMIDA.ZUXR
    rotation[1][2] = +8.37127701219746E-01;  // RSMIDA.ZUYR
    rotation[2][2] = +5.47005275276417E-01;  // RSMIDA.ZUZR

    // create ground domain
    auto gd = vpgl_ground_domain<double>("R");
    gd.translation_ = translation;
    gd.rotation_ = rotation;
    return gd;
  }
  else
  {
    return vpgl_ground_domain<double>(ground_domain_id.substr(0, 1));
  }
}

// ground domain coordinates
// - [lon (deg), lat (deg), elev (meters)]
// - [lon (rad), lat (rad), elev (meters)] with lon on range (-pi, pi)
// - [lon (rad), lat (rad), elev (meters)] with lon on range (0, 2pi)
// - Earth centered Earth fixed (ECEF) coordinate
// - Rectangular coordinate
static std::vector<std::vector<std::vector<double> > >
create_ground_domain_coords()
{
  std::vector<std::vector<std::vector<double> > > data = {
    {
      {-117.0386236785, 33.1616263666, -500},
      {-2.04270933519924, 0.578779565419217, -500},
      {4.240475971980347, 0.578779565419217, -500},
      {-2429458.222842, -4760144.758575, 3468704.785650},
      {17.802446, -11.286898, -493.251402},
    },
    {
      {-117.0388169203, 33.1617298457, 0},
      {-2.04271270789735, 0.578781371463418, 0},
      {4.240472599282237, 0.578781371463418, 0},
      {-2429661.700896, -4760503.788871, 3468987.894357},
      {-0.243572, 0.154427, 6.748648},
    },
    {
      {-117.0390101313, 33.1618333074, 500},
      {-2.04271608007467, 0.578783177217907, 500},
      {4.240469227104916, 0.578783177217907, 500},
      {-2429865.178922, -4760862.819116, 3469271.003024},
      {-18.289589, 11.595750, 506.748627},
    },
  };
  return data;
}

// test single ground domain coordinate conversion
static void
_test_ground_domain(vpgl_ground_domain<double> gd,
                    std::vector<double> lon_lat_elev,
                    std::vector<double> xyz,
                    double xtol, double ytol, double ztol)
{
  double lon = lon_lat_elev[0], lat = lon_lat_elev[1], elev = lon_lat_elev[2];
  double x = xyz[0], y = xyz[1], z = xyz[2];

  double _x, _y, _z;
  gd.world_to_ground(lon, lat, elev, _x, _y, _z);

  std::cout << "(" << lon << "," << lat << "," << elev << ") -> "
            << "(" << x << "," << y << "," << z << ")\n"
            << "result = (" << _x << "," << _y << "," << _z << ")\n";

  TEST_NEAR("x", _x, x, xtol);
  TEST_NEAR("y", _y, y, ytol);
  TEST_NEAR("z", _z, z, ztol);
}

// test ground domain
static void
test_ground_domain(std::string ground_domain_id)
{
  // tolerance
  double rtol = 1e-8, mtol = 1e-4;
  double xtol, ytol, ztol;
  if (ground_domain_id.substr(0, 1) == "R")
    xtol = ytol = ztol = mtol;
  else
    xtol = ytol = rtol; ztol = mtol;

  // test coordinates
  auto coords = create_ground_domain_coords();
  size_t result_idx;
  if (ground_domain_id == "R-transform")
    result_idx = 4;
  else if (ground_domain_id == "R-identity")
    result_idx = 3;
  else if (ground_domain_id == "H")
    result_idx = 2;
  else
    result_idx = 1;

  // run test
  std::cout << "\n===== Ground domain " << ground_domain_id << " =====\n";
  auto gd = create_ground_domain(ground_domain_id);
  std::cout << gd << "\n";
  for (auto const& item : coords) {
    _test_ground_domain(gd, item[0], item[result_idx], xtol, ytol, ztol);
  }
}


// ----- REGION SELECTOR -----

// test region selector "select"
static void
_test_region_selector(vpgl_region_selector<double> selector,
                      std::array<double, 3> xyz,
                      std::array<size_t, 2> rc)
{
  double x = xyz[0], y = xyz[1], z = xyz[2];
  size_t r = rc[0], c = rc[1];

  size_t _r, _c;
  selector.select(x, y, z, _r, _c);

  std::cout << "(" << x << "," << y << "," << z << ") -> "
            << "(" << r << "," << c << ")\n"
            << "result = (" << _r << "," << _c << ")\n";

  TEST("r", _r, r);
  TEST("c", _c, c);
}

// create region selector
static vpgl_region_selector<double>
create_region_selector()
{
  // RSMIDA
  size_t minr = 0,
         maxr = 9292,
         minc = 0,
         maxc = 9122;

  // RSMPIA
  size_t rnis = 2,
         cnis = 2,
         tnis = 4,
         rssiz = 4646,
         cssiz = 4561;

  std::vector<double> row_coefs = {
    -1.37241587646741E+10, // R0
    +6.80345003035694E+09, // RX
    -2.53961725272867E+09, // RY
    -2.73740874871126E+04, // RZ
    -8.41052141609109E+08, // RXX
    +5.97072375992576E+08, // RXY
    +6.42450733136118E+03, // RXZ
    +6.77395740580080E+06, // RYY
    +2.27081764262021E+02, // RYZ
    +1.03024536087538E-03, // RZZ
  };

  std::vector<double> col_coefs = {
    -1.99837981040581E+09, // C0
    +4.46286444870403E+08, // CX
    +3.61714766643827E+09, // CY
    -3.62940014004744E+03, // CZ
    +1.95441015791188E+07, // CXX
    -1.05767897568751E+09, // CXY
    -2.69030105715253E+02, // CXZ
    +7.66625782038074E+08, // CYY
    +8.24211130197391E+03, // CYZ
    +9.01748615982117E-04, // CZZ
  };

  // initialize region selector
  return vpgl_region_selector<double>(row_coefs, col_coefs,
                                      minr, maxr, minc, maxc,
                                      rnis, cnis, tnis, rssiz, cssiz);
}

// test region selector
static void
test_region_selector()
{
  // test data: (lon_rad, lat_rad, elev_m) -> (row, col)
  // ground domain "H" using longitude on range (0, 2*pi)
  std::vector<std::pair<std::array<double, 3>, std::array<size_t, 2> > > data = {
    {{4.2404725993, 0.5787813715, 0}, {1, 1}},  // 0, 0
    {{4.2405416897, 0.5787811963, 0}, {1, 1}},  // 1024, 0
    {{4.2406112558, 0.5788392388, 0}, {1, 1}},  // 2048, 1024
    {{4.2406811481, 0.5788975452, 0}, {1, 1}},  // 3072, 2048
    {{4.2407513729, 0.5789561208, 0}, {1, 1}},  // 4096, 3072
    {{4.2408219371, 0.5790149710, 0}, {2, 1}},  // 5120, 4096
    {{4.2408928473, 0.5790741013, 0}, {2, 2}},  // 6144, 5120
    {{4.2409641106, 0.5791335174, 0}, {2, 2}},  // 7168, 6144
    {{4.2410357339, 0.5791932251, 0}, {2, 2}},  // 8192, 7168
    {{4.2411077246, 0.5792532304, 0}, {2, 2}},  // 9216, 8192
    {{4.2411800193, 0.5793134802, 0}, {2, 2}},  // 10239*, 9215*
  };

  // run tests
  std::cout << "\n===== Region selector =====\n";
  auto selector = create_region_selector();
  for (auto const& item : data) {
    _test_region_selector(selector, item.first, item.second);
  }
}


// ----- POLYCAM -----

// polycam in rectangular ground domain
static vpgl_polycam<double>
create_polycam_rect()
{
  // RSMPCA
  std::vector<vpgl_scale_offset<double>> scale_offsets = {
    {+3.11793659470231E+03, +2.65571142640788E+03}, // XNRMSF, XNRMO
    {+2.96084811268529E+03, +2.40732324869712E+03}, // YNRMSF, YNRMO
    {+1.00233148808220E+03, -2.33161661728923E+00}, // ZNRMSF, ZNRMO
    {+5.49720000000000E+03, +4.56100000000000E+03}, // CNRMSF, CNRMO
    {+5.59920000000000E+03, +4.64600000000000E+03}, // RNRMSF, RNRMO
  };

  std::vector<std::vector<int>> powers = {
    {1, 1, 1}, // CNPWRX, CNPWRY, CNPWRZ
    {1, 1, 1}, // CDPWRX, CDPWRY, CDPWRZ
    {1, 1, 1}, // RNPWRX, RNPWRY, RNPWRZ
    {1, 1, 1}, // RDPWRX, RDPWRY, RDPWRZ
  };

  auto CNPCF = std::vector<double>(8, 0.0);
  CNPCF[0] = +3.66773732052461E-01;
  CNPCF[1] = -3.99759610583925E-02;
  CNPCF[2] = +1.49479106149881E+00;
  CNPCF[4] = +3.60340008196375E-01;

  auto CDPCF = std::vector<double>(8, 0.0);
  CDPCF[0]= +1.00000000000000E+00;
  CDPCF[1]= +5.02862720498340E-02;
  CDPCF[2]= -3.02226608402894E-02;
  CDPCF[4]= -4.48001662401429E-01;

  auto RNPCF = std::vector<double>(8, 0.0);
  RNPCF[0] = +4.63481151803541E-01;
  RNPCF[1] = +1.47720153582094E+00;
  RNPCF[2] = +2.45032267644299E-02;
  RNPCF[4] = +3.89362443002779E-01;

  auto RDPCF = std::vector<double>(8, 0.0);
  RDPCF[0] = +1.00000000000000E+00;
  RDPCF[1] = +5.02862720499332E-02;
  RDPCF[2] = -3.02226608403966E-02;
  RDPCF[4] = -4.48001662401358E-01;

  std::vector<std::vector<double>> coeffs = {CNPCF, CDPCF, RNPCF, RDPCF};

  // initialize polycam
  return vpgl_polycam<double>(1, 1, powers, coeffs, scale_offsets);
}

// test coordinates in rectangular ground domain
// {lon_deg, lat_deg, elev_m}, {x_rect, y_rect, z_rect}, {u, v}
static std::vector<std::vector<std::vector<double> > >
create_coords_rect()
{
  std::vector<std::vector<std::vector<double> > > coords{
    {{-117.02719568, 33.17020003, 0}, {1081.953217, 941.641878, 6.588443}, {2610, 2994}},
    {{-117.03119771, 33.18429170, 0}, {705.641227, 2503.781141, 6.219689}, {7091, 1995}},
    {{-117.00590098, 33.17798445, 0}, {3066.295151, 1809.120255, 5.757182}, {4901, 8287}},
    {{-117.00460320, 33.17908728, 0}, {3187.063711, 1931.696404, 5.662121}, {5229, 8607}},
    {{-117.03541506, 33.18205237, 0}, {312.810150, 2254.668096, 6.343963}, {6410, 887}},
    {{-117.02750809, 33.17830643, 0}, {1051.038729, 1840.643992, 6.397982}, {5149, 2936}},
    {{-117.03719018, 33.16317745, 0}, {151.200618, 160.988480, 6.745030}, {449, 422}},
    {{-117.01476467, 33.16645820, 350}, {2242.425984, 529.035918, 356.333541}, {1676, 7218}},
    {{-117.02436681, 33.18640237, -150}, {1342.178538, 2739.062627, -143.979161}, {7208, 3508}},
    {{-117.02801139, 33.17715898, 200}, {1004.381186, 1713.344492, 206.440932}, {5259, 3109}},
    {{-117.03878450, 33.16924395, 100}, {1.225252, 833.541210, 106.695078}, {2453, 14}},
    {{-117.02261539, 33.16302301, -50}, {1510.768136, 146.511297, -43.431599}, {398, 4024}},
    {{-117.02975285, 33.17217737, -250}, {842.977143, 1160.429414, -243.411437}, {2918, 2084}},
    {{-117.00941214, 33.17111665, -450}, {2740.269250, 1046.648427, -443.924287}, {2385, 6127}},
    {{-117.00408316, 33.17647492, -275}, {3236.062515, 1642.002735, -269.281494}, {3966, 7742}},
    {{-117.02974527, 33.17974551, 25}, {842.081550, 1999.846652, 31.381064}, {5686, 2394}},
    {{-117.03860157, 33.16831576, 475}, {18.481759, 730.671321, 481.707579}, {2590, 128}},
    {{-117.03262886, 33.16858343, 0}, {575.528881, 761.360585, 6.678099}, {2123, 1603}},
  };
  return coords;
}

// create polycam in geodetic ground domain
static vpgl_polycam<double>
create_polycam_geodetic(std::string ground_domain_id)
{
  // longitude offset for "G" vs "H" ground domain
  double XNRMO = ground_domain_id == "G" ? -2.04221591140939E+00 : 4.24096939577019E+00;

  // RSMPCA
  std::vector<vpgl_scale_offset<double>> scale_offsets = {
    {+5.83421668078010E-04, XNRMO}, // XNRMSF, XNRMO
    {+4.66059641384187E-04, +5.79158321291012E-01}, // YNRMSF, YNRMO
    {+1.00000000000186E+03, -6.74864824209362E+00}, // ZNRMSF, ZNRMO
    {+5.49720000000000E+03, +4.56100000000000E+03}, // CNRMSF, CNRMO
    {+5.59920000000000E+03, +4.64600000000000E+03}, // RNRMSF, RNRMO
  };

  std::vector<std::vector<int>> powers = {
    {3, 3, 3}, // CNPWRX, CNPWRY, CNPWRZ
    {3, 3, 3}, // CDPWRX, CDPWRY, CDPWRZ
    {3, 3, 3}, // RNPWRX, RNPWRY, RNPWRZ
    {3, 3, 3}, // RDPWRX, RDPWRY, RDPWRZ
  };

  auto CNPCF = std::vector<double>(64, 0.0);
  CNPCF[0] = +3.64262524869639E-01;
  CNPCF[1] = -4.03269322436926E-02;
  CNPCF[2] = +2.94189908809905E-04;
  CNPCF[3] = +1.47987110587007E-06;
  CNPCF[4] = +1.44039580063938E+00;
  CNPCF[5] = -7.64240318965315E-03;
  CNPCF[6] = -4.62914407821698E-05;
  CNPCF[8] = -2.27657634100344E-01;
  CNPCF[9] = -6.99927840105152E-05;
  CNPCF[12] = +4.10411248708628E-05;
  CNPCF[16] = +3.98505389032450E-01;
  CNPCF[17] = -7.13633177112544E-03;
  CNPCF[18] = -1.40962539967904E-05;
  CNPCF[20] = +1.03986534900037E-01;
  CNPCF[21] = -3.98879093080698E-05;
  CNPCF[24] = -1.05187629636560E-04;
  CNPCF[32] = +3.81241726833631E-02;
  CNPCF[33] = -5.44862677883373E-06;
  CNPCF[36] = -2.41515190161667E-05;
  CNPCF[48] = -9.24599477974846E-06;

  auto CDPCF = std::vector<double>(64, 0.0);
  CDPCF[0] = +1.00000000000000E+00;
  CDPCF[1] = +4.19453955246271E-02;
  CDPCF[2] = -1.50048653690059E-04;
  CDPCF[3] = -4.71548453751820E-06;
  CDPCF[4] = -1.81890686856092E-01;
  CDPCF[5] = -7.52417593265918E-03;
  CDPCF[6] = -5.23025726600268E-05;
  CDPCF[8] = +4.85002743683534E-03;
  CDPCF[9] = +1.12968148962368E-06;
  CDPCF[12] = -4.69864840875186E-05;
  CDPCF[16] = -3.41282595066945E-01;
  CDPCF[17] = +9.34608164095360E-03;
  CDPCF[18] = +5.00754772029558E-05;
  CDPCF[20] = +6.47988903173317E-02;
  CDPCF[21] = +1.72426529077727E-05;
  CDPCF[24] = +3.32067421698402E-05;
  CDPCF[32] = -4.74114961590690E-02;
  CDPCF[33] = +6.73247093950977E-06;
  CDPCF[36] = +1.35286284390110E-05;
  CDPCF[48] = +1.14945299924978E-05;

  auto RNPCF = std::vector<double>(64, 0.0);
  RNPCF[0] = +4.61453664950781E-01;
  RNPCF[1] = +1.38190696682569E+00;
  RNPCF[2] = -3.06147364200467E-01;
  RNPCF[3] = +3.28792501108032E-05;
  RNPCF[4] = +2.45188701607480E-02;
  RNPCF[5] = +6.11460883514606E-03;
  RNPCF[6] = -6.45331750414860E-05;
  RNPCF[8] = -1.00782705816033E-04;
  RNPCF[9] = +8.46152870619938E-05;
  RNPCF[12] = -1.49259216156496E-06;
  RNPCF[16] = +4.35563233105028E-01;
  RNPCF[17] = +6.90909414451905E-02;
  RNPCF[18] = -2.10065633297316E-04;
  RNPCF[20] = +5.01234694861233E-03;
  RNPCF[21] = -1.29148996553568E-04;
  RNPCF[24] = -1.86040154831026E-05;
  RNPCF[32] = +3.93012571486070E-02;
  RNPCF[33] = -9.31517434748352E-05;
  RNPCF[36] = -1.19840956001379E-05;
  RNPCF[48] = -2.19962926020238E-05;

  auto RDPCF = std::vector<double>(64, 0.0);
  RDPCF[0] = +1.00000000000000E+00;
  RDPCF[1] = -1.56190767496260E-01;
  RDPCF[2] = -1.02038541324516E-02;
  RDPCF[3] = -7.03651699125910E-05;
  RDPCF[4] = -2.24238916336525E-02;
  RDPCF[5] = +6.43745504883784E-03;
  RDPCF[6] = +8.10102566594885E-07;
  RDPCF[8] = +1.10331627345199E-04;
  RDPCF[9] = -5.97118995547424E-05;
  RDPCF[12] = +1.58015332154755E-06;
  RDPCF[16] = -3.46110400197230E-01;
  RDPCF[17] = +9.76454921221523E-02;
  RDPCF[18] = +3.63845507917380E-05;
  RDPCF[20] = -6.37513210682523E-03;
  RDPCF[21] = +4.74097007262724E-05;
  RDPCF[24] = +2.15371334134181E-05;
  RDPCF[32] = -4.52832093222257E-02;
  RDPCF[33] = +3.58743363036983E-05;
  RDPCF[36] = +1.40009993944189E-05;
  RDPCF[48] = +2.53076939483868E-05;

  std::vector<std::vector<double>> coeffs = {CNPCF, CDPCF, RNPCF, RDPCF};

  // initialize polycam
  return vpgl_polycam<double>(1, 1, powers, coeffs, scale_offsets);
}

// test coordinates in geodetic ground domain
// {lon_deg, lat_deg, elev_m}, {lon_rad, lat_rad, elev_m}, {u, v}
static std::vector<std::vector<std::vector<double> > >
create_coords_geodetic(std::string ground_domain_id)
{
  std::vector<std::vector<std::vector<double> > > coords{
    {{-117.02206729, 33.17258376, 0}, {-2.0424203717, 0.5789708080, 0.0}, {3329, 4294}},
    {{-117.01467863, 33.17967653, 0}, {-2.0422914152, 0.5790946002, 0.0}, {5484, 6166}},
    {{-117.00795867, 33.17994699, 0}, {-2.0421741298, 0.5790993206, 0.0}, {5517, 7808}},
    {{-117.02662957, 33.16689753, 0}, {-2.0424999984, 0.5788715645, 0.0}, {1589, 3128}},
    {{-117.01567762, 33.17521761, 0}, {-2.0423088509, 0.5790167774, 0.0}, {4111, 5893}},
    {{-117.03857234, 33.17523722, 0}, {-2.0427084390, 0.5790171197, 0.0}, {4246, 55}},
    {{-117.00829643, 33.18434521, 0}, {-2.0421800247, 0.5791760840, 0.0}, {6880, 7761}},
    {{-117.03713606, 33.16335494, 0}, {-2.0426833713, 0.5788097347, 0.0}, {504, 436}},
    {{-117.01432909, 33.18788889, 0}, {-2.0422853145, 0.5792379329, 0.0}, {8055, 6304}},
    {{-117.03377254, 33.18717004, 0}, {-2.0426246668, 0.5792253867, 0.0}, {8052, 1323}},
    {{-117.03344850, 33.16318601, 0}, {-2.0426190111, 0.5788067864, 0.0}, {452, 1387}},
    {{-117.01733827, 33.16397381, 425}, {-2.0423378346, 0.5788205360, 425.0}, {819, 6734}},
    {{-117.00722322, 33.17719353, 275}, {-2.0421612938, 0.5790512636, 275.0}, {5297, 9100}},
    {{-117.03740164, 33.17801722, 225}, {-2.0426880066, 0.5790656398, 225.0}, {5711, 431}},
    {{-117.03795063, 33.16734085, 100}, {-2.0426975882, 0.5788793019, 100.0}, {1824, 244}},
    {{-117.01591060, 33.17653480, -125}, {-2.0423129171, 0.5790397666, -125.0}, {4286, 5521}},
    {{-117.03543247, 33.16280543, -200}, {-2.0426536381, 0.5788001439, -200.0}, {318, 785}},
    {{-117.03197105, 33.18881664, -300}, {-2.0425932249, 0.5792541252, -300.0}, {7531, 1551}},
    {{-117.01352899, 33.18081351, -325}, {-2.0422713501, 0.5791144443, -325.0}, {5103, 5605}},
    {{-117.00701383, 33.16155285, -450}, {-2.0421576392, 0.5787782823, -450.0}, {0, 6558}},
  };

  // convert lon_rad to "H" coordinates on range (0, 2pi)
  if (ground_domain_id == "H") {
    for (auto & item : coords) {
      double& lon_rad = item[1][0];
      lon_rad = lon_rad < 0 ? lon_rad + vnl_math::twopi : lon_rad;
    }
  }

  return coords;
}

// polycam selector
static vpgl_polycam<double>
create_polycam(std::string ground_domain_id)
{
  if (ground_domain_id == "R")
    return create_polycam_rect();
  else
    return create_polycam_geodetic(ground_domain_id);
}

// coords selector
static std::vector<std::vector<std::vector<double> > >
create_coords(std::string ground_domain_id)
{
  if (ground_domain_id == "R")
    return create_coords_rect();
  else
    return create_coords_geodetic(ground_domain_id);
}

// test projection of single coordinate via polycam
static void
_test_polycam(vpgl_polycam<double> cam,
              std::vector<double> xyz,
              std::vector<double> uv,
              double tol=0.002)
{
  double x = xyz[0], y = xyz[1], z = xyz[2];
  size_t u = uv[0], v = uv[1];

  double _u, _v;
  cam.project(x, y, z, _u, _v);

  std::cout << "(" << x << "," << y << "," << z << ") -> "
            << "(" << u << "," << v << ")\n"
            << "result = (" << _u << "," << _v << ")\n";

  TEST_NEAR("u", _u, u, tol);
  TEST_NEAR("v", _v, v, tol);
}

// test polycam
static void
test_polycam(std::string ground_domain_id)
{
  std::cout << "\n===== Polycam " << ground_domain_id << " =====\n";
  auto cam = create_polycam(ground_domain_id);
  auto coords = create_coords(ground_domain_id);
  for (auto const& item : coords) {
    _test_polycam(cam, item[1], item[2]);
  }
}


// ----- RSM CAMERA -----

// create RSM camera with single polycam
static vpgl_RSM_camera<double>
create_camera(std::string ground_domain_id)
{
  auto polycam = create_polycam(ground_domain_id);

  std::string id = (ground_domain_id == "R") ? "R-transform" : ground_domain_id;
  auto gd = create_ground_domain(id);

  auto cam = vpgl_RSM_camera<double>(polycam);
  cam.set_ground_domain(gd);

  return cam;
}

// test single coordinate projection via RSM camera
static void
_test_camera(vpgl_RSM_camera<double> cam,
             std::vector<double> lon_lat_elev,
             std::vector<double> uv,
             double tol=0.002)
{
  double lon = lon_lat_elev[0], lat = lon_lat_elev[1], elev = lon_lat_elev[2];
  size_t u = uv[0], v = uv[1];

  double _u, _v;
  cam.project(lon, lat, elev, _u, _v);

  std::cout << "(" << lon << "," << lat << "," << elev << ") -> "
            << "(" << u << "," << v << ")\n"
            << "result = (" << _u << "," << _v << ")\n";

  TEST_NEAR("u", _u, u, tol);
  TEST_NEAR("v", _v, v, tol);
}

// test RSM camera
static void
test_camera(std::string ground_domain_id)
{
  std::cout << "\n===== RSM Camera " << ground_domain_id << " =====\n";
  auto cam = create_camera(ground_domain_id);
  auto coords = create_coords(ground_domain_id);
  for (auto const& item : coords) {
    _test_camera(cam, item[0], item[2]);
  }
}


// ----- MAIN TEST FUNCTION -----
static void
test_RSM_camera()
{
  // increase std::cout precision
  auto flags = std::cout.flags();
  std::cout << std::setprecision(12);

  // ground domain
  test_ground_domain("G");
  test_ground_domain("H");
  test_ground_domain("R-identity");
  test_ground_domain("R-transform");

  // region selector
  test_region_selector();

  // polycam
  test_polycam("R");
  test_polycam("G");
  test_polycam("H");

  // RSM camera
  test_camera("R");
  test_camera("G");
  test_camera("H");

  // reset std::cout format
  std::cout.flags(flags);
}

TESTMAIN(test_RSM_camera);
