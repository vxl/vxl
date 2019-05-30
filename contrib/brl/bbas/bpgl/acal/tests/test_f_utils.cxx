#include <iostream>
#include <testlib/testlib_test.h>
#include <limits>
#include <string>
#include <map>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <vnl/vnl_matrix_fixed.h>
#include <bpgl/acal/acal_f_utils.h>


static bool abs_less(const double& a, const double& b) {
	return fabs(a) < fabs(b);
}


static void test_f_utils()
{
  vnl_vector_fixed<double, 3> c1(1.0), c2(1.0);
  std::vector<vnl_vector_fixed<double, 3> > corrs1, corrs2;

  // actual correspondences
  c1[0] = 179.1840; c1[1] =  29.4661; c2[0] = 205.4420; c2[1] =  34.4196; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  91.1275; c1[1] =  78.3622; c2[0] = 448.5680; c2[1] = 519.9030; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 154.5370; c1[1] =  73.3591; c2[0] = 107.7510; c2[1] =  74.3437; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 243.4730; c1[1] =  91.1386; c2[0] = 423.3930; c2[1] = 245.0190; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =   7.0481; c1[1] = 128.9270; c2[0] =  20.1435; c2[1] = 117.5360; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =   7.0481; c1[1] = 128.9270; c2[0] =  20.1435; c2[1] = 117.5360; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 209.7440; c1[1] = 163.1460; c2[0] = 214.3220; c2[1] = 169.2870; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 203.1510; c1[1] = 206.6270; c2[0] =  61.4054; c2[1] =  44.4826; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 319.8440; c1[1] = 203.7800; c2[0] = 321.9810; c2[1] = 247.7750; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 304.4210; c1[1] = 219.0760; c2[0] = 323.5170; c2[1] = 244.7250; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 304.4210; c1[1] = 219.0760; c2[0] = 323.5170; c2[1] = 244.7250; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 313.8120; c1[1] = 217.8060; c2[0] = 332.8540; c2[1] = 243.9420; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  85.4921; c1[1] = 238.0250; c2[0] = 180.9010; c2[1] = 142.4460; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 227.8510; c1[1] = 228.0710; c2[0] = 248.7200; c2[1] = 248.2260; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 225.4930; c1[1] = 230.7090; c2[0] = 247.4090; c2[1] = 251.0550; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 423.5700; c1[1] = 225.8510; c2[0] = 427.8230; c2[1] = 253.7440; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  58.1851; c1[1] = 254.5760; c2[0] =  52.7895; c2[1] = 258.1860; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 164.3420; c1[1] = 253.1990; c2[0] = 184.2550; c2[1] = 269.2800; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 242.6760; c1[1] = 251.1900; c2[0] = 261.4260; c2[1] = 272.6380; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 335.5270; c1[1] = 243.4450; c2[0] = 342.0580; c2[1] = 268.7130; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 245.5120; c1[1] = 270.9880; c2[0] = 262.9860; c2[1] = 294.6480; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 409.7230; c1[1] = 262.2620; c2[0] = 414.0440; c2[1] = 291.3500; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  89.8847; c1[1] = 282.5520; c2[0] =  66.3610; c2[1] = 261.0740; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 141.0530; c1[1] = 287.0030; c2[0] = 159.3580; c2[1] = 287.6220; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 145.3120; c1[1] = 284.6490; c2[0] = 159.0870; c2[1] = 297.5240; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 228.4750; c1[1] = 279.2360; c2[0] = 238.8650; c2[1] = 304.4310; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 228.4750; c1[1] = 279.2360; c2[0] = 238.8650; c2[1] = 304.4310; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  94.8873; c1[1] = 291.9480; c2[0] = 106.7660; c2[1] = 299.6020; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  93.2562; c1[1] = 294.2350; c2[0] = 104.3390; c2[1] = 302.4500; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  98.6600; c1[1] = 292.4770; c2[0] = 109.7580; c2[1] = 300.2780; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 260.3190; c1[1] = 296.8050; c2[0] =  65.8224; c2[1] = 168.4470; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  46.5029; c1[1] = 314.3990; c2[0] =  58.7938; c2[1] = 320.1040; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  47.4869; c1[1] = 317.7270; c2[0] =  59.5675; c2[1] = 323.4320; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  73.0530; c1[1] = 314.6870; c2[0] =  85.2222; c2[1] = 322.1960; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  58.2309; c1[1] = 330.9910; c2[0] =  68.4026; c2[1] = 337.5140; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 105.0530; c1[1] = 332.0410; c2[0] = 115.9270; c2[1] = 343.4650; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 395.4780; c1[1] = 335.4470; c2[0] = 398.6790; c2[1] = 368.4400; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  90.4540; c1[1] = 346.2590; c2[0] = 128.5130; c2[1] = 329.8510; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 176.5570; c1[1] = 344.6380; c2[0] = 192.1840; c2[1] = 245.3980; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 325.6790; c1[1] = 348.4800; c2[0] = 331.2470; c2[1] = 377.4650; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 360.1200; c1[1] = 345.3530; c2[0] = 365.1300; c2[1] = 376.2670; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 361.6900; c1[1] = 349.5340; c2[0] = 365.9410; c2[1] = 381.0850; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 375.8390; c1[1] = 337.5000; c2[0] = 192.5250; c2[1] =  31.1670; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 163.2310; c1[1] = 370.9590; c2[0] = 175.1580; c2[1] = 391.3000; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 272.9800; c1[1] = 381.1040; c2[0] = 282.6730; c2[1] = 409.9170; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  41.2728; c1[1] = 389.6790; c2[0] =  54.3014; c2[1] = 400.8510; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  51.9041; c1[1] = 394.8450; c2[0] =  65.0425; c2[1] = 407.4350; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  51.9041; c1[1] = 394.8450; c2[0] =  65.0425; c2[1] = 407.4350; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 104.5790; c1[1] = 399.2630; c2[0] = 131.0760; c2[1] = 427.1660; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 139.9130; c1[1] = 386.4730; c2[0] = 151.0600; c2[1] = 398.6200; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 358.2350; c1[1] = 385.3330; c2[0] = 363.4050; c2[1] = 419.2890; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 370.5010; c1[1] = 394.7700; c2[0] = 374.8730; c2[1] = 429.7200; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 416.3910; c1[1] = 396.9140; c2[0] = 416.4440; c2[1] = 434.3010; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 422.3420; c1[1] = 398.5700; c2[0] = 421.3400; c2[1] = 436.2320; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  39.9328; c1[1] = 401.5000; c2[0] =  52.2904; c2[1] = 413.1280; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  40.1720; c1[1] = 404.1670; c2[0] =  52.4588; c2[1] = 415.8920; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  44.8254; c1[1] = 415.6630; c2[0] = 128.2940; c2[1] =  58.7021; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  61.4255; c1[1] = 401.3790; c2[0] = 166.9420; c2[1] = 328.2690; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 379.3910; c1[1] = 400.3520; c2[0] = 469.6010; c2[1] = 330.2400; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  29.9582; c1[1] = 423.3670; c2[0] =  41.9734; c2[1] = 435.6500; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  65.5813; c1[1] = 427.0860; c2[0] =  73.1327; c2[1] = 439.8640; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  90.9760; c1[1] = 432.1910; c2[0] = 100.0080; c2[1] = 448.0120; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 135.9260; c1[1] = 417.1750; c2[0] = 111.1800; c2[1] = 410.5130; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 136.4640; c1[1] = 430.9800; c2[0] = 143.1980; c2[1] = 449.3690; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 234.4060; c1[1] = 416.5370; c2[0] = 206.4000; c2[1] = 395.1470; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 270.8500; c1[1] = 429.7280; c2[0] = 282.0000; c2[1] = 462.9460; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 325.4240; c1[1] = 419.9440; c2[0] = 333.5910; c2[1] = 455.9550; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 198.2470; c1[1] = 452.7840; c2[0] = 421.5560; c2[1] = 556.1180; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 419.4320; c1[1] = 456.9700; c2[0] = 421.3130; c2[1] = 501.1460; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 132.9050; c1[1] = 479.7100; c2[0] = 140.5600; c2[1] = 502.2930; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 194.1360; c1[1] = 467.8820; c2[0] = 198.9550; c2[1] = 493.3030; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  35.4795; c1[1] = 489.0200; c2[0] =  46.0062; c2[1] = 506.0290; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  40.3799; c1[1] = 491.4590; c2[0] =  50.7183; c2[1] = 508.8640; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 431.9740; c1[1] = 482.0530; c2[0] = 434.1440; c2[1] = 527.8360; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  37.8218; c1[1] = 541.4370; c2[0] =  48.2307; c2[1] = 561.7880; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  81.3650; c1[1] = 529.0390; c2[0] =  90.7744; c2[1] = 551.9770; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 281.6730; c1[1] = 543.4200; c2[0] = 286.9960; c2[1] = 582.9390; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 409.7750; c1[1] = 543.1890; c2[0] = 411.2080; c2[1] = 591.5170; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  17.7540; c1[1] = 557.2060; c2[0] =  27.9096; c2[1] = 577.1900; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 96.36830; c1[1] = 545.0200; c2[0] = 104.7540; c2[1] = 570.4340; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 149.5740; c1[1] = 546.4670; c2[0] = 155.6060; c2[1] = 574.8850; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 276.5080; c1[1] = 546.5820; c2[0] = 282.4730; c2[1] = 586.3290; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 273.8230; c1[1] = 552.3270; c2[0] = 278.9240; c2[1] = 591.3360; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =   7.7523; c1[1] = 568.4830; c2[0] =  17.5215; c2[1] = 588.8840; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 334.7200; c1[1] = 569.6990; c2[0] = 337.0860; c2[1] = 614.3390; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  27.5075; c1[1] = 587.0500; c2[0] =  36.5129; c2[1] = 609.5550; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] =  80.1634; c1[1] = 582.3430; c2[0] =  88.5361; c2[1] = 608.3710; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 189.0260; c1[1] = 581.4750; c2[0] = 194.8080; c2[1] = 616.0350; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 178.0750; c1[1] = 587.9780; c2[0] = 184.2180; c2[1] = 622.2190; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 192.3850; c1[1] = 591.0030; c2[0] = 196.8610; c2[1] = 625.5430; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 339.3290; c1[1] = 587.5410; c2[0] = 339.6030; c2[1] = 631.8150; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 364.3340; c1[1] = 577.0260; c2[0] = 364.3510; c2[1] = 625.5000; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 138.3910; c1[1] = 597.1200; c2[0] = 113.2660; c2[1] = 429.3530; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 215.6860; c1[1] = 598.4160; c2[0] = 123.7960; c2[1] = 554.4460; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 192.9880; c1[1] =  71.5402; c2[0] = 299.9940; c2[1] = 567.1310; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 178.1220; c1[1] =  88.2486; c2[0] = 195.7850; c2[1] =  86.3628; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 220.3700; c1[1] = 103.8670; c2[0] = 230.1970; c2[1] = 107.1330; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 120.4190; c1[1] = 151.1500; c2[0] = 145.7620; c2[1] = 148.9790; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 174.4520; c1[1] = 156.4720; c2[0] = 171.4500; c2[1] = 529.5140; corrs1.push_back(c1); corrs2.push_back(c2);
  c1[0] = 186.9160; c1[1] = 153.3510; c2[0] = 196.0120; c2[1] = 156.9820; corrs1.push_back(c1); corrs2.push_back(c2);

  size_t n_matches = corrs1.size();
  std::vector<acal_match_pair> matches;
  for (size_t i = 0; i < n_matches; ++i) {
    vnl_vector_fixed<double, 3> c1 = corrs1[i];
    vnl_vector_fixed<double, 3> c2 = corrs2[i];
	  vgl_point_2d<double> p1(c1[0], c1[1]), p2(c2[0], c2[1]);
	  acal_corr corr1(i, p1), corr2(i, p2);
    acal_match_pair mp(corr1, corr2);
	  matches.push_back(mp);
  }

  // actual F
  vnl_matrix_fixed<double, 3, 3> Fa(0.0);
  Fa[0][2] = -0.138048; Fa[1][2] = 0.224528;
  Fa[2][0]= 0.117889; Fa[2][1] = -0.241421; Fa[2][2] =   6.93955;

  size_t n_filt = acal_f_utils::filter_matches(Fa, matches);
  TEST("filtering with F", n_filt, 74);

  // test ray intersection using cameras and correspondences from challenge site
  // camera 20
  vnl_vector_fixed<double,4> row_20_0, row_20_1;
  row_20_0[0]=1.34828; row_20_0[1]=0.0037962; row_20_0[2]=0.276516; row_20_0[3]=5.85829;
  row_20_1[0]=0.218073; row_20_1[1]=-0.9263; row_20_1[2]=-1.00106; row_20_1[3]=532.937;
  vpgl_affine_camera<double> acam_20(row_20_0, row_20_1);
  acam_20.set_viewing_distance(9488.48);

  // camera 27
  vnl_vector_fixed<double,4> row_27_0, row_27_1;
  row_27_0[0]=1.31473; row_27_0[1]=0.0143843; row_27_0[2]=0.461217; row_27_0[3]=9.89921;
  row_27_1[0]=0.334822; row_27_1[1]=-0.989292; row_27_1[2]=-0.937947; row_27_1[3]=546.072;
  vpgl_affine_camera<double> acam_27(row_27_0, row_27_1);
  acam_27.set_viewing_distance(9488.48);

  // camera 28
  vnl_vector_fixed<double,4> row_28_0, row_28_1;
  row_28_0[0]=1.31477; row_28_0[1]=0.0144363; row_28_0[2]=0.461303; row_28_0[3]=9.87035;
  row_28_1[0]=0.334867; row_28_1[1]=-0.989425; row_28_1[2]=-0.938105; row_28_1[3]=550.072;
  vpgl_affine_camera<double> acam_28(row_28_0, row_28_1);
  acam_28.set_viewing_distance(9488.48);

  // camera 30
  vnl_vector_fixed<double,4> row_30_0, row_30_1;
  row_30_0[0]=1.34466; row_30_0[1]=0.00114891; row_30_0[2]=-0.3428; row_30_0[3]=80.0172;
  row_30_1[0]=-0.250436; row_30_1[1]=-0.956661; row_30_1[2]=-0.983978; row_30_1[3]=615.884;
  vpgl_affine_camera<double> acam_30(row_30_0, row_30_1);
  acam_30.set_viewing_distance(9488.48);

  vgl_point_2d<double> t_0_20(227.851, 228.071);
  vgl_point_2d<double> t_0_27(248.72, 248.226);
  vgl_point_2d<double> t_0_28(248.68, 248.203);
  vgl_point_2d<double> t_0_30(237.199, 237.808);


  /// 3-d point should be
  vgl_point_3d<double> p_0_3d(148.0, 252.0, 101.0);
  // as found by BWM

  std::map<size_t, vpgl_affine_camera<double> > cams;
  cams[20] = acam_20;
  cams[27] = acam_27;
  cams[28] = acam_28;
  cams[30] = acam_30;

  std::map<size_t, vgl_point_3d<double> > inter_pts;
  std::vector< std::map<size_t, vgl_point_2d<double> > > tracks(1);
  std::map<size_t, std::map<size_t, vgl_point_2d<double> > > proj_tracks;
  tracks[0][20]=t_0_20;
  tracks[0][27] = t_0_27;
  tracks[0][28] = t_0_28;
  tracks[0][30] = t_0_30;

  bool good = acal_f_utils::intersect_tracks_with_3d(cams, tracks, inter_pts, proj_tracks);

  double dist_3d = (p_0_3d - inter_pts[0]).length();
  vgl_vector_2d<double> tr_20 = t_0_20 - proj_tracks[0][20];
  vgl_vector_2d<double> tr_27 = t_0_27 - proj_tracks[0][27];
  vgl_vector_2d<double> tr_28 = t_0_28 - proj_tracks[0][27];
  vgl_vector_2d<double> tr_30 = t_0_30 - proj_tracks[0][30];
  double proj_error = (tr_20.length() + tr_27.length() + tr_28.length() + tr_30.length())/4.0;
  TEST("affine camera ray intersection", proj_error<1.1, true);
}

TESTMAIN(test_f_utils);
