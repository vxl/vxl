#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

//******************************************************************************
//:
// \file
//
//  This file is a subset of routines that will be used
//  to create and list the datum coordinate shift formulas. Inculded are
//  the following  examples of datum shifts, demonstrating the functionality
//  of these routines.
//
//  These datum shifts have the following interesting property:
//
//      TO GET:
//         WGS84 from NAD27 ->  compute d_lat, d_lon, d_H using NAD27 lat/lon,
//      THEN
//         lat(WGS84) = lat(NAD27)+d_lat
//         lon(WGS84) = lon(NAD27)+d_lon
//         elev(WGS84) = elev(NAD27)+d_H
//
//      TO GET:
//         NAD27 from WGS84 -> compute d_lat, d_lon, d_H using WGS84 lat/lon,
//      THEN
//         lat(NAD27) = lat(WGS84)-d_lat
//         lon(NAD27) = lon(WGS84)-d_lon <- note "-" signs
//         elev(NAD27) = elev(WGS84)-d_H
//

#define degree_to_rad   (vnl_math::pi/180.0)      // Degree to rad conv.
#define dcos(x)         vcl_cos((x)*vnl_math::pi/180.0)
#define dsin(x)         vcl_sin((x)*vnl_math::pi/180.0)
#define EPSILON         1.0e-12


double ipow(double x, int i)
{
  double y = 1.0;
  if (i<0) { i *= -1; x = y/x; }
  while (i) { if (i%2) y*=x; i/=2; x*=x; }
  return y;
}


//*************************************************************************
//      FUNCTION: wgs72_to_wgs84_deltas()
//      Computes deltas that are either added or
//      subtracted instead of added to the input lat, lon, elev.
//*************************************************************************

void wgs72_to_wgs84_deltas
  (double phi,                 //!< input lat (degrees)
   double lamda,               //!< input lon (degrees)
   double /* height */,        //!< input elv (meters)
   double *delta_phi,          //!< lat shift (degrees)
   double *delta_lamda,        //!< lon shift (degrees)
   double *delta_hgt)          //!< elev shift (meters)
{
  // Information found from "Supplement to Department of Defense
  // world geodeti systems 1984 Technical Report.  (pg 25-8)

  double delta_f, a, delta_a, delta_r;          // constants

  //****************************** Instructions: ******************************
  //
  // To obtain WGS 84 Coordinates:
  // Add the delta_phi, delta_lamda, and delta_height changes calculated
  // using WGS 72 Coordinates to the WGS 72 Coordinates (phi, lamda, height)
  // given.
  //
  // NOTE:Latitude is positive North, and Longitude is positive East (0 to 360)
  //***************************************************************************

 // Constant definition
  delta_f = .3121057e-7;
  a       = 6378135;          // meters
  delta_a = 2.000;            // meters
  delta_r = 1.400;            // meters

  // Convert the longitude from -180 -> +180  to 0 -> 360
  if (lamda < 0)
    lamda += 360.0;

  // First compure delta_phi and delta_lamda are in arc seconds

  *delta_phi = (4.5 * dcos(phi))/(a * dsin(1/3600.0)) +
    (delta_f * dsin(2.0 * phi)) / (dsin(1/3600.0));

  *delta_lamda = .554;

  *delta_hgt = 4.5 * dsin(phi) +
    a * delta_f * (dsin(phi) * dsin(phi)) - delta_a + delta_r;

  // Convert to decimal degrees
  *delta_phi   /= 3600.0;
  *delta_lamda /= 3600.0;
}

//*************************************************************************
//      FUNCTION: wgs72_to_wgs84()
//*************************************************************************

void wgs72_to_wgs84
  (
   double phi,                 //!< input lat, lon, elev coord (degrees)
   double lamda,
   double height,
   double *wgs84_phi,          //!< lat shift (degrees)
   double *wgs84_lamda,        //!< lon shift (degrees)
   double *wgs84_hgt)          //!< elev shift (meters)
{
  double delta_phi, delta_lamda, delta_hgt;

  wgs72_to_wgs84_deltas(phi, lamda, height,
                        &delta_phi, &delta_lamda, &delta_hgt);

  *wgs84_phi   = phi + delta_phi;
  *wgs84_lamda = lamda + delta_lamda;
  *wgs84_hgt   = height + delta_hgt;
}

//*************************************************************************
//      FUNCTION: wgs84_to_wgs72()
//*************************************************************************

void wgs84_to_wgs72
  (
   double phi,                 //!< input lat, lon, elev coord (degrees)
   double lamda,
   double height,
   double *wgs72_phi,          //!< lat shift (degrees)
   double *wgs72_lamda,        //!< lon shift (degrees)
   double *wgs72_hgt)          //!< elev shift (meters)
{
  double delta_phi, delta_lamda, delta_hgt;

  wgs72_to_wgs84_deltas(phi, lamda, height,
                        &delta_phi, &delta_lamda, &delta_hgt);

  *wgs72_phi   = phi - delta_phi;
  *wgs72_lamda = lamda - delta_lamda;
  *wgs72_hgt   = height - delta_hgt;
}

//*************************************************************************
//      FUNCTION: nad27m_to_wgs84_deltas()
//      Area:     Mexico/Central America
//*************************************************************************

void nad27m_to_wgs84_deltas
  (double phi,                 //!< input lat (degrees)
   double lamda,               //!< input lon (degrees)
   double /* height */,        //!< input elv (meters)
   double *delta_phi,          //!< lat shift (arc sec)
   double *delta_lamda,        //!< lon shift (arc sec)
   double *delta_hgt)          //!< elev shift (meters)
{
  // Information found from "Supplement to Department of Defense
  // world geodetic systems 1984 Technical Report.  (pg 20-36)


  double U,V, K;

  K = .05235988;

  // Convert the longitude from -180 -> +180  to 0 -> 360
  if (lamda < 0)
    lamda += 360.0;

  // deta_phi and delta_lamda are in arc seconds

  U = K * (phi - 20);
  V = K * (lamda -290);

  *delta_phi = .67775 + 10.02259*U - 18.72391*V + 12.66341*ipow(U,2) - 32.47686*ipow(V,2) - 52.70768*ipow(U,3) - 13.86527*U*ipow(V,2)
    - 16.82734*ipow(V,3) - 59.54646*V*ipow(U,3) + 120.64887*ipow(U,5) + 7.23362*ipow(U,2)*ipow(V,3) + 95.89131*ipow(U,5)*V -2.89651*U*ipow(V,5)
    - 1.23778*ipow(V,6) + 70.19213*ipow(U,8) + .84596*ipow(U,2)*ipow(V,6) + .14848*ipow(V,8) - 3.83786*ipow(U,3)*ipow(V,6) + .10405*U*ipow(V,9)
      + 30.09795*ipow(U,8)*ipow(V,3) + 12.93585*ipow(U,6)*ipow(V,6) - .58747*ipow(U,3)*ipow(V,9) + .7485*ipow(U,4)*ipow(V,9) +
        6.32462*ipow(U,8)*ipow(V,7) - .12736*ipow(U,6)*ipow(V,9) - .59993*ipow(U,9)*ipow(V,9);

  *delta_lamda = -.33643 - 3.11777*V -5.16881*ipow(U,2) - 3.17318*ipow(V,2) - 34.59331*ipow(U,3) + .97215*U*ipow(V,2) - .5818*ipow(V,3) +
    223.78114*ipow(U,5) + 358.07266*ipow(U,6) + 270.68064*ipow(U,7) - 87.99549*ipow(U,6)*V - .09789*U*ipow(V,6) + 636.41982*ipow(U,7)*V -
      729.91514*ipow(U,6)*ipow(V,2) - .60122*ipow(U,3)*ipow(V,5) - 375.38863*ipow(U,6)*ipow(V,3) - 6.97538*ipow(U,4)*ipow(V,5) -
        .00138*ipow(V,9) - 363.45977*ipow(U,9)*V + 50.19955*ipow(U,8)*ipow(V,2) - 12.01575*ipow(U,5)*ipow(V,6) - .4314*ipow(U,6)*ipow(V,9) +
          .43907*ipow(U,9)*ipow(V,9);

  *delta_hgt = -20.013 + 12.815*U - 8.084*V +74.122*ipow(U,2)*V +39.523*ipow(U,2)*ipow(V,2) - 23.158*ipow(U,4)*V + 194.444*ipow(U,6) +
    36.797*ipow(U,5)*V + .181*U*ipow(V,5) - 292.155*ipow(U,8) + 3.749*ipow(U,5)*ipow(V,5) +2.537*ipow(U,8)*ipow(V,6);

  // Convert to decimal degrees
  *delta_phi   /= 3600.0;
  *delta_lamda /= 3600.0;
}


//*************************************************************************
//      FUNCTION: nad27m_to_wgs84()
//*************************************************************************

void nad27m_to_wgs84
  (double phi,                 //!< input lat, lon, elev coord (degrees)
   double lamda,
   double height,
   double *wgs84_phi,          //!< lat shift (degrees)
   double *wgs84_lamda,        //!< lon shift (degrees)
   double *wgs84_hgt)          //!< elev shift (meters)
{
  double delta_phi, delta_lamda, delta_hgt;

  nad27m_to_wgs84_deltas(phi, lamda, height,
                         &delta_phi, &delta_lamda, &delta_hgt);

  *wgs84_phi   = phi + delta_phi;
  *wgs84_lamda = lamda + delta_lamda;
  *wgs84_hgt   = height + delta_hgt;
}


//*************************************************************************
//      FUNCTION: wgs84_to_nad27m()
//*************************************************************************

void wgs84_to_nad27m
  (double phi,                 //!< input lat, lon, elev coord (degrees)
   double lamda,
   double height,
   double *nad27m_phi,         //!< lat shift (degrees)
   double *nad27m_lamda,       //!< lon shift (degrees)
   double *nad27m_hgt)         //!< elev shift (meters)
{
  double delta_phi, delta_lamda, delta_hgt;

  nad27m_to_wgs84_deltas(phi, lamda, height,
                         &delta_phi, &delta_lamda, &delta_hgt);

  *nad27m_phi   = phi - delta_phi;
  *nad27m_lamda = lamda - delta_lamda;
  *nad27m_hgt   = height - delta_hgt;
}


//*************************************************************************
//      FUNCTION: nad27n_to_wgs84_deltas()
//*************************************************************************

void nad27n_to_wgs84_deltas
  (double phi,                 //!< input lat (degrees)
   double lamda,               //!< input lon (degrees)
   double /* height */,        //!< input elv (meters) 
   double *delta_phi,          //!< lat shift (arc sec)
   double *delta_lamda,        //!< lon shift (arc sec)
   double *delta_hgt)          //!< elev shift (meters)
{
  // Information found from "Supplement to Department of Defense
  // world geodetic systems 1984 Technical Report.  (pg 19-2)

  double U,V, K;

  K = .05235988;

  // Convert the longitude from -180 -> +180  to 0 -> 360
  if (lamda < 0)
    lamda += 360.0;

  U = K * (phi - 37.0);
  V = K * (lamda -265.0);

  // deta_phi and delta_lamda are in arc seconds

  *delta_phi = .16984 - .76173*U + .09585*V +1.09919*ipow(U,2) - 4.57801*ipow(U,3) - 1.13239*ipow(U,2)*V + .49831*ipow(V,3)
    - .98399*ipow(U,3)*V + .12415*U*ipow(V,3) + .1145*ipow(V,4) +27.05396*ipow(U,5) + 2.03449*ipow(U,4)*V +.73357*ipow(U,2)*ipow(V,3)
      - .37548*ipow(V,5) - .14197*ipow(V,6) -59.96555*ipow(U,7) + .07439*ipow(V,7) -4.76082*ipow(U,8) + .03385*ipow(V,8) +
        49.0432*ipow(U,9) - 1.30575*ipow(U,6)*ipow(V,3) - .07653*ipow(U,3)*ipow(V,9) + .08646*ipow(U,4)*ipow(V,9);

  *delta_lamda = -.88437 + 2.05061*V + .26361*ipow(U,2) - .76804*U*V + .13374*ipow(V,2) - 1.31974*ipow(U,3) - .52162*ipow(U,2)*V -
    1.05853*U*ipow(V,2) -.49211*ipow(U,2)*ipow(V,2) + 2.17204*U*ipow(V,3) -.06004*ipow(V,4) +.30139*ipow(U,4)*V + 1.88585*U*ipow(V,4)
      - .81162*U*ipow(V,5) -.05183*ipow(V,6) -.96723*U*ipow(V,6) -.12948*ipow(U,3)*ipow(V,5) + 3.41827*ipow(U,9) -.44507*ipow(U,8)*V
        +.18882*U*ipow(V,8) -.01444*ipow(V,9) +.04794*U*ipow(V,9) -.59013*ipow(U,9)*ipow(V,3);

  *delta_hgt = -36.526 +3.9*U -4.723*V -21.553*ipow(U,2) +7.294*U*V +8.886*ipow(V,2) -8.44*ipow(U,2)*V -2.93*U*ipow(V,2) +
    56.937*ipow(U,4) -58.756*ipow(U,3)*V -4.061*ipow(V,4) +4.447*ipow(U,4)*V +4.903*ipow(U,2)*ipow(V,3) -55.873*ipow(U,6)
      +212.005*ipow(U,5)*V + 3.081*ipow(V,6) -254.511*ipow(U,7)*V -.756*ipow(V,8) +30.654*ipow(U,8)*V -.122*U*ipow(V,9);

  // Convert to decimal degrees
  *delta_phi   /= 3600.0;
  *delta_lamda /= 3600.0;
}


//*************************************************************************
//      FUNCTION: nad27n_to_wgs84()
//*************************************************************************

void nad27n_to_wgs84
  (double phi,                 //!< input lat, lon, elev coord (degrees)
   double lamda,
   double height,
   double *wgs84_phi,          //!< lat shift (degrees)
   double *wgs84_lamda,        //!< lon shift (degrees)
   double *wgs84_hgt)          //!< elev shift (meters)
{
  double delta_phi, delta_lamda, delta_hgt;

  nad27n_to_wgs84_deltas(phi, lamda, height,
                         &delta_phi, &delta_lamda, &delta_hgt);

  *wgs84_phi   = phi + delta_phi;
  *wgs84_lamda = lamda + delta_lamda;
  *wgs84_hgt   = height + delta_hgt;
}

//*************************************************************************
//      FUNCTION: wgs84_to_nad27n()
//*************************************************************************

void wgs84_to_nad27n
  (double phi,           //!< input lat, lon, elev coord (degrees)
   double lamda,
   double height,
   double *nad27n_phi,   //!< lat shift (degrees)
   double *nad27n_lamda, //!< lon shift (degrees)
   double *nad27n_hgt)   //!< elev shift (meters)
{
  double delta_phi, delta_lamda, delta_hgt;

  nad27n_to_wgs84_deltas(phi, lamda, height,
                         &delta_phi, &delta_lamda, &delta_hgt);

  *nad27n_phi   = phi - delta_phi;
  *nad27n_lamda = lamda - delta_lamda;
  *nad27n_hgt   = height - delta_hgt;
}

//*************************************************************************
//            ROUTINE: nad27_wgs84_alternate
//
//            REVISION:  October 8, 1990  -    Original
//            REVISION:  May 15,    1992  -    Rajiv Gupta
//
//            This program is a test program designed to convert a
//            latitude/longitude coordinate in NAD27 to the WGS84
//            coordinate system.
//*************************************************************************


//*************************************************************************
//       FUNCTION: nad27n_to_wgs84_alternate()
//       AUTHOR:   Mark Young
//*************************************************************************

void nad27n_to_wgs84_alternate
  (double nad27_lat, double nad27_lon, double nad27_el,
   double *wgs84_lat, double *wgs84_lon, double *wgs84_el)
{
  double u,v,k;

  double a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14;
  double a15,a16,a17,a18,a19,a20,a21,a22,a23;

  double b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14;
  double b15,b16,b17,b18,b19,b20,b21,b22,b23;

  double c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14;
  double c15,c16,c17,c18,c19,c20;

  double delta_lat;
  double delta_lon;
  double delta_H;
#ifdef DEBUG_DATUM
  double N_height;
#endif

  double delta_lat_p;
  double delta_lon_p;

#ifdef DEBUG_DATUM
  double prin_lat_rad;
  double ft_lat;
  double ft_lon;
  double c_error;
#endif

#if 0
  int    lon_int;
  double lon_int_fl;
  double lon_frac;
#endif

#ifdef DEBUG_DATUM
  double new_lat;
  double new_lon;
#endif

  double prin_lat; // Est. lat of principal point
  double prin_lon; // Est. lon of principal point

#ifdef DEBUG_DATUM
  //   Display the program title.

  vcl_cout << "\n  NAD27 to WGS84 Conversion routine\n\n"
           << "       Enter latitude and longitude : ";
  vcl_cin >> nad27_lat >> nad27_lon;

#endif

  prin_lat = (double) nad27_lat;
  prin_lon = (double) nad27_lon;

#ifdef DEBUG_DATUM
  prin_lat_rad = prin_lat * degree_to_rad;
#endif

//lon_int = (int) prin_lon;
//lon_int_fl = (double)lon_int;
//lon_frac = vcl_fabs(prin_lon - lon_int);

  if (prin_lon < 0)
    prin_lon = 360.0 + prin_lon;
//  prin_lon = 360.0 + lon_int_fl + lon_frac;

  k = .05235988;
  u = k*(prin_lat - 37);
  v = k*(prin_lon - 265);

  // Initialize latitude constants

  a1 = 0.16984;
  a2 = -0.76173;
  a3 = 0.09585;
  a4 = 1.09919;
  a5 = -4.57801;
  a6 = -1.13239;
  a7 = 0.49831;
  a8 = -.98399;
  a9 = 0.12415;
  a10 = 0.11450;
  a11 = 27.05396;
  a12 = 2.03449;
  a13 = 0.73357;
  a14 = -0.37548;
  a15 = -0.14197;
  a16 = -59.96555;
  a17 = 0.07439;
  a18 = -4.76082;
  a19 = 0.03385;
  a20 = 49.04320;
  a21 = -1.30575;
  a22 = -0.07653;
  a23 = 0.08646;

  delta_lat = a1 + a2*u + a3*v + a4*u*u + a5*u*u*u + a6*u*u*v
    + a7*v*v*v + a8*u*u*u*v + a9*u*v*v*v + a10*v*v*v*v
      + a11*u*u*u*u*u + a12*u*u*u*u*v + a13*u*u*v*v*v
        + a14*v*v*v*v*v + a15*v*v*v*v*v*v + a16*u*u*u*u*u*u*u
          + a17*v*v*v*v*v*v*v + a18*u*u*u*u*u*u*u*u
            + a19*v*v*v*v*v*v*v*v + a20*u*u*u*u*u*u*u*u*u
              + a21*u*u*u*u*u*u*v*v*v + a22*u*u*u*v*v*v*v*v*v*v*v*v
                + a23*u*u*u*u*v*v*v*v*v*v*v*v*v;

  // Initialize longitude constants

  b1 = -0.88437;
  b2 = 2.05061;
  b3 = 0.26361;
  b4 = -0.76804;
  b5 = 0.13374;
  b6 = -1.31974;
  b7 = -0.52162;
  b8 = -1.05853;
  b9 = -0.49211;
  b10 = 2.17204;
  b11 = -0.06004;
  b12 = 0.30139;
  b13 = 1.88585;
  b14 = -0.81162;
  b15 = -0.05183;
  b16 = -0.96723;
  b17 = -.12948;
  b18 = 3.41827;
  b19 = -0.44507;
  b20 = 0.18882;
  b21 = -0.01444;
  b22 = 0.04794;
  b23 = -0.59013;

  delta_lon = b1 + b2*v + b3*u*u + b4*u*v + b5*v*v + b6*u*u*u
    + b7*u*u*v + b8*u*v*v + b9*u*u*v*v + b10*u*v*v*v
      + b11*v*v*v*v + b12*u*u*u*u*v + b13*u*v*v*v*v
        + b14*u*v*v*v*v*v + b15*v*v*v*v*v*v
          + b16*u*v*v*v*v*v*v + b17*u*u*u*v*v*v*v*v
            + b18*u*u*u*u*u*u*u*u*u + b19*u*u*u*u*u*u*u*u*v
              + b20*u*v*v*v*v*v*v*v*v + b21*v*v*v*v*v*v*v*v*v
                + b22*u*v*v*v*v*v*v*v*v*v + b23*u*u*u*u*u*u*u*u*u*v*v*v;


  c1 = -36.526;
  c2 = 3.900;
  c3 = -4.723;
  c4 = -21.553;
  c5 = 7.294;
  c6 = 8.886;
  c7 = -8.440;
  c8 = -2.930;
  c9 = 56.937;
  c10 = -58.756;
  c11 = -4.061;
  c12 = 4.447;
  c13 = 4.903;
  c14 = -55.873;
  c15 = 212.005;
  c16 = 3.081;
  c17 = -254.511;
  c18 = -0.756;
  c19 = 30.654;
  c20 = -0.122;

  delta_H = c1 + c2*u + c3*v + c4*u*u + c5*u*v + c6*v*v + c7*u*u*v
    + c8*u*v*v + c9*u*u*u*u + c10*u*u*u*v + c11*v*v*v*v
      + c12*u*u*u*u*v + c13*u*u*v*v*v + c14*u*u*u*u*u*u
        + c15*u*u*u*u*u*v + c16*v*v*v*v*v*v + c17*u*u*u*u*u*u*u*v
          + c18*v*v*v*v*v*v*v*v + c19*u*u*u*u*u*u*u*u*v
            + c20*v*v*v*v*v*v*v*v*v;


#ifdef DEBUG_DATUM
  double d1 = 5.068;
  double d2 = -11.570;
  double d3 = -8.574;
  double d4 = 27.839;
  double d5 = -51.911;
  double d6 = 29.496;
  double d7 = 28.343;
  double d8 = 24.481;
  double d9 = 11.424;
  double d10 = 132.550;
  double d11 = -110.232;
  double d12 = 41.018;
  double d13 = -64.953;
  double d14 = -128.293;
  double d15 = 51.241;
  double d16 = -4.326;
  double d17 = 104.097;
  double d18 = -128.031;
  double d19 = 110.694;
  double d20 = 36.330;
  double d21 = 243.149;
  double d22 = -15.790;
  double d23 = -38.043;
  double d24 = -40.277;
  double d25 = 2.746;
  double d26 = -7.321;
  double d27 = -394.404;
  double d28 = -927.540;
  double d29 = 63.390;
  double d30 = 10.626;
  double d31 = -0.520;
  double d32 = -117.207;
  double d33 = 16.352;

  N_height = d1 + d2*u + d3*v + d4*u*u + d5*u*v + d6*v*v + d7*u*u*u
    + d8*u*v*v + d9*v*v*v + d10*u*u*u*v + d11*u*u*v*v
      + d12*u*v*v*v + d13*v*v*v*v + d14*u*u*u*v*v + d15*u*v*v*v*v
        + d16*v*v*v*v*v + d17*u*u*u*u*v*v + d18*u*u*u*v*v*v
          + d19*u*u*v*v*v*v + d20*v*v*v*v*v*v + d21*u*u*u*u*u*u*v
            + d22*u*u*v*v*v*v*v + d23*u*v*v*v*v*v*v + d24*u*u*v*v*v*v*v*v
              + d25*u*v*v*v*v*v*v*v + d26*v*v*v*v*v*v*v*v
                + d27*u*u*u*u*u*u*u*u*u + d28*u*u*u*u*u*u*u*u*v
                  + d29*u*u*u*u*v*v*v*v*v + d30*u*v*v*v*v*v*v*v*v
                    + d31*u*v*v*v*v*v*v*v*v*v + d32*u*u*u*u*u*u*u*u*v*v*v*v
                      + d33*u*u*u*u*u*v*v*v*v*v*v*v*v;
#endif

  delta_lat_p = (double)delta_lat; // NOTE: delta_lat is in arc seconds
  delta_lon_p = (double)delta_lon; // NOTE: delta_lon is in arc seconds

#ifdef DEBUG_DATUM
  ft_lat = 101.0 * delta_lat_p;
  ft_lon = 101.0 * vcl_cos(prin_lat_rad) * delta_lon_p;

  c_error = (double)vcl_sqrt(ft_lat*ft_lat + ft_lon*ft_lon);
#endif

  *wgs84_lat = nad27_lat + (delta_lat_p/3600.0);
  *wgs84_lon = nad27_lon + (delta_lon_p/3600.0);
  *wgs84_el  = nad27_el + delta_H;

#ifdef DEBUG_DATUM
  new_lat = *wgs84_lat;
  new_lon = *wgs84_lon;
  vcl_cout << "\n d_lat = " << delta_lat
           << "\n d_lon = " << delta_lon
           << "\n d_H = " << delta_H
           << "\n N = " << N_height
           << "\n circular error = " << c_error
           << "\n New Lat = " << new_lat << ", New Lon =\n" << new_lon
           << '\n';
#endif
}

//*************************************************************************
//      FUNCTION: wgs84_to_nad27n_alternate()
//      AUTHOR:   Mark Young
//      This is a copy of the above routine in which the deltas are
//      subtracted instead of added to the input lat, lon, elev.
//*************************************************************************

void wgs84_to_nad27n_alternate
  (double wgs84_lat, double wgs84_lon, double wgs84_el,
   double *nad27n_lat, double *nad27n_lon, double *nad27n_el)
{
  double u,v,k;

  double a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14;
  double a15,a16,a17,a18,a19,a20,a21,a22,a23;

  double b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14;
  double b15,b16,b17,b18,b19,b20,b21,b22,b23;

  double c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14;
  double c15,c16,c17,c18,c19,c20;

  double delta_lat;
  double delta_lon;
  double delta_H;
#ifdef DEBUG_DATUM
  double N_height;
#endif

  double delta_lat_p;
  double delta_lon_p;

#ifdef DEBUG_DATUM
  double prin_lat_rad;
  double ft_lat;
  double ft_lon;
  double c_error;
#endif

#if 0
  int    lon_int;
  double lon_int_fl;
  double lon_frac;
#endif

#ifdef DEBUG_DATUM
  double new_lat;
  double new_lon;
#endif

  double prin_lat; // Est. lat of principal point
  double prin_lon; // Est. lon of principal point

#ifdef DEBUG_DATUM
  //   Display the program title.

  vcl_cout << "\n  wgs84 to NAD27N Conversion routine\n\n";
              "       Enter latitude, longitude ";
  vcl_cin >> wgs84_lat >> wgs84_lon;

#endif

  prin_lat = (double) wgs84_lat;
  prin_lon = (double) wgs84_lon;

#ifdef DEBUG_DATUM
  prin_lat_rad = prin_lat * degree_to_rad;
#endif

//lon_int = (int) prin_lon;
//lon_int_fl = (double)lon_int;
//lon_frac = vcl_fabs(prin_lon - lon_int);

  if (prin_lon < 0)
    prin_lon = 360.0 + prin_lon;
//  prin_lon = 360.0 + lon_int_fl + lon_frac;

  k = .05235988;
  u = k*(prin_lat - 37);
  v = k*(prin_lon - 265);

  // Initialize latitude constants

  a1 = 0.16984;
  a2 = -0.76173;
  a3 = 0.09585;
  a4 = 1.09919;
  a5 = -4.57801;
  a6 = -1.13239;
  a7 = 0.49831;
  a8 = -.98399;
  a9 = 0.12415;
  a10 = 0.11450;
  a11 = 27.05396;
  a12 = 2.03449;
  a13 = 0.73357;
  a14 = -0.37548;
  a15 = -0.14197;
  a16 = -59.96555;
  a17 = 0.07439;
  a18 = -4.76082;
  a19 = 0.03385;
  a20 = 49.04320;
  a21 = -1.30575;
  a22 = -0.07653;
  a23 = 0.08646;

  delta_lat = a1 + a2*u + a3*v + a4*u*u + a5*u*u*u + a6*u*u*v
    + a7*v*v*v + a8*u*u*u*v + a9*u*v*v*v + a10*v*v*v*v
      + a11*u*u*u*u*u + a12*u*u*u*u*v + a13*u*u*v*v*v
        + a14*v*v*v*v*v + a15*v*v*v*v*v*v + a16*u*u*u*u*u*u*u
          + a17*v*v*v*v*v*v*v + a18*u*u*u*u*u*u*u*u
            + a19*v*v*v*v*v*v*v*v + a20*u*u*u*u*u*u*u*u*u
              + a21*u*u*u*u*u*u*v*v*v + a22*u*u*u*v*v*v*v*v*v*v*v*v
                + a23*u*u*u*u*v*v*v*v*v*v*v*v*v;

  // Initialize longitude constants

  b1 = -0.88437;
  b2 = 2.05061;
  b3 = 0.26361;
  b4 = -0.76804;
  b5 = 0.13374;
  b6 = -1.31974;
  b7 = -0.52162;
  b8 = -1.05853;
  b9 = -0.49211;
  b10 = 2.17204;
  b11 = -0.06004;
  b12 = 0.30139;
  b13 = 1.88585;
  b14 = -0.81162;
  b15 = -0.05183;
  b16 = -0.96723;
  b17 = -.12948;
  b18 = 3.41827;
  b19 = -0.44507;
  b20 = 0.18882;
  b21 = -0.01444;
  b22 = 0.04794;
  b23 = -0.59013;

  delta_lon = b1 + b2*v + b3*u*u + b4*u*v + b5*v*v + b6*u*u*u
    + b7*u*u*v + b8*u*v*v + b9*u*u*v*v + b10*u*v*v*v
      + b11*v*v*v*v + b12*u*u*u*u*v + b13*u*v*v*v*v
        + b14*u*v*v*v*v*v + b15*v*v*v*v*v*v
          + b16*u*v*v*v*v*v*v + b17*u*u*u*v*v*v*v*v
            + b18*u*u*u*u*u*u*u*u*u + b19*u*u*u*u*u*u*u*u*v
              + b20*u*v*v*v*v*v*v*v*v + b21*v*v*v*v*v*v*v*v*v
                + b22*u*v*v*v*v*v*v*v*v*v + b23*u*u*u*u*u*u*u*u*u*v*v*v;


  c1 = -36.526;
  c2 = 3.900;
  c3 = -4.723;
  c4 = -21.553;
  c5 = 7.294;
  c6 = 8.886;
  c7 = -8.440;
  c8 = -2.930;
  c9 = 56.937;
  c10 = -58.756;
  c11 = -4.061;
  c12 = 4.447;
  c13 = 4.903;
  c14 = -55.873;
  c15 = 212.005;
  c16 = 3.081;
  c17 = -254.511;
  c18 = -0.756;
  c19 = 30.654;
  c20 = -0.122;

  delta_H = c1 + c2*u + c3*v + c4*u*u + c5*u*v + c6*v*v + c7*u*u*v
    + c8*u*v*v + c9*u*u*u*u + c10*u*u*u*v + c11*v*v*v*v
      + c12*u*u*u*u*v + c13*u*u*v*v*v + c14*u*u*u*u*u*u
        + c15*u*u*u*u*u*v + c16*v*v*v*v*v*v + c17*u*u*u*u*u*u*u*v
          + c18*v*v*v*v*v*v*v*v + c19*u*u*u*u*u*u*u*u*v
            + c20*v*v*v*v*v*v*v*v*v;

#ifdef DEBUG_DATUM
  d1 = 5.068;
  d2 = -11.570;
  d3 = -8.574;
  d4 = 27.839;
  d5 = -51.911;
  d6 = 29.496;
  d7 = 28.343;
  d8 = 24.481;
  d9 = 11.424;
  d10 = 132.550;
  d11 = -110.232;
  d12 = 41.018;
  d13 = -64.953;
  d14 = -128.293;
  d15 = 51.241;
  d16 = -4.326;
  d17 = 104.097;
  d18 = -128.031;
  d19 = 110.694;
  d20 = 36.330;
  d21 = 243.149;
  d22 = -15.790;
  d23 = -38.043;
  d24 = -40.277;
  d25 = 2.746;
  d26 = -7.321;
  d27 = -394.404;
  d28 = -927.540;
  d29 = 63.390;
  d30 = 10.626;
  d31 = -0.520;
  d32 = -117.207;
  d33 = 16.352;

  N_height = d1 + d2*u + d3*v + d4*u*u + d5*u*v + d6*v*v + d7*u*u*u
    + d8*u*v*v + d9*v*v*v + d10*u*u*u*v + d11*u*u*v*v
      + d12*u*v*v*v + d13*v*v*v*v + d14*u*u*u*v*v + d15*u*v*v*v*v
        + d16*v*v*v*v*v + d17*u*u*u*u*v*v + d18*u*u*u*v*v*v
          + d19*u*u*v*v*v*v + d20*v*v*v*v*v*v + d21*u*u*u*u*u*u*v
            + d22*u*u*v*v*v*v*v + d23*u*v*v*v*v*v*v + d24*u*u*v*v*v*v*v*v
              + d25*u*v*v*v*v*v*v*v + d26*v*v*v*v*v*v*v*v
                + d27*u*u*u*u*u*u*u*u*u + d28*u*u*u*u*u*u*u*u*v
                  + d29*u*u*u*u*v*v*v*v*v + d30*u*v*v*v*v*v*v*v*v
                    + d31*u*v*v*v*v*v*v*v*v*v + d32*u*u*u*u*u*u*u*u*v*v*v*v
                      + d33*u*u*u*u*u*v*v*v*v*v*v*v*v;
#endif

  delta_lat_p = (double)delta_lat; // NOTE: delta_lat is in arc seconds
  delta_lon_p = (double)delta_lon; // NOTE: delta_lon is in arc seconds

#ifdef DEBUG_DATUM
  ft_lat = 101.0 * delta_lat_p;
  ft_lon = 101.0 * vcl_cos(prin_lat_rad) * delta_lon_p;

  c_error = (double)vcl_sqrt(ft_lat*ft_lat + ft_lon*ft_lon);
#endif

  // SUBTRACT the deltas
  *nad27n_lat = wgs84_lat - (delta_lat_p/3600.0);
  *nad27n_lon = wgs84_lon - (delta_lon_p/3600.0);
  *nad27n_el  = wgs84_el - delta_H;

#ifdef DEBUG_DATUM
  new_lat = *nad27n_lat;
  new_lon = *nad27n_lon;
  vcl_cout << "\n d_lat = " << delta_lat
           << "\n d_lon = " << delta_lon
           << "\n d_H = " << delta_H
           << "\n N = " << N_height
           << "\n circular error = " << c_error
           << "\n New Lat = " << new_lat << ", New Lon =\n" << new_lon
           << '\n';
#endif
}


//**********************************************************************
// GEO_DETIC2CENTRIC(geodetic_lat, A, B):
//**********************************************************************
double geo_detic2centric
  (double geodetic_lat,  //!< geodetic latitude of input point
   double A,
   double B)             //!< Major and minor axes of earth
{
  return(vcl_atan((B/A)*(B/A) * vcl_tan(geodetic_lat)));
}

//**********************************************************************
// GEO_CENTRIC2DETIC(geocentric_lat, A, B):
//**********************************************************************
double geo_centric2detic
  (double geocentric_lat, //!< geocentric latitude of input point
   double A,
   double B)              //!< Major and minor axes of earth
{
  return(vcl_atan((A/B)*(A/B) * vcl_tan(geocentric_lat)));
}


//**********************************************************************
//  GRS_to_latlong (x, y, z, lat, lon, el, A, B):
//  Computes, from the GRS coordinates of a point, its the lat, long,
//  and elevation in a GEODETIC coordinate system.
//  It used an iterative method based on an alternate way
//  of coputating x, y, z from lat, lon, el based on the equations:
//
//  Nov. 28, 2000 - JLM
//    The input x, y, z, values are assumed to be in meters
//    The input elevation and A, B values are assumed to be in meters
//    The output lat and lon values are in radians
//
//  $N = A/\sqrt{1 - e^2 \sin(\mbox{lat})^2}$
//  $x = (N + \mbox{el}) \cos(\mbox{lat})\cos(\mbox{lon})$
//  $y = (N + \mbox{el}) \cos(\mbox{lat})\sin(\mbox{lon})$
//  $z = (N (1-e^2) + \mbox{el}) \sin(\mbox{lat})$
//
//**********************************************************************
void GRS_to_latlong
  (double x, double y, double z, //!< Input GRS coords
   double *geodetic_lat,
   double *lon,
   double *el,                   //!< output coordinates of point
   double A,
   double B)                     //!< Major and minor axes of earth
{
  double new_lat;                //!< used in iteration
  double N;                      //!< Local distance to earth center
  double xy_dist;                //!< dist in x-y plane
  double ee;                     //!< eccentricity square

  xy_dist = vcl_sqrt(x*x + y*y);
  ee = 1 - (B/A)*(B/A);

  // Compute gecentric = gedetic longitude from the
  // dot-product of (x, y, 0) and (1, 0, 0) and assign
  // proper sign to it based on y.

  *lon = vcl_acos(x/xy_dist);
  if (y < 0)    // Negative y => West of Greenwich meridian
    *lon = - *lon;

  // Compute an initial value for geodetic latitude
  new_lat = vcl_atan2(z, xy_dist);

  do
  {
    *geodetic_lat = new_lat;

    // Compute the sin and cos of the approx geodetic lat

    // Compute the distance N along the height
    N = A / vcl_sqrt(1 - ee * vcl_sin(*geodetic_lat) * vcl_sin(*geodetic_lat));

    // new_lat is already in the range -PI to PI
    new_lat = vcl_atan2( (z + N * ee * vcl_sin(*geodetic_lat)),  xy_dist );
  }
  while (vcl_fabs(new_lat - *geodetic_lat) > EPSILON);

  *geodetic_lat = new_lat;
  *el = xy_dist/vcl_cos(new_lat)  -  N;
}


//**********************************************************************
//  latlong_to_GRS (lat, lon, el, x, y, z, A, B):
//  Computes GRS coordinates of a point from its the lat, long,
//  and elevation in a GEODETIC coordinate system.
//
//    Nov. 28, 2000 - JLM
//    The input lat and lon values are assumed to be in radians
//    The input elevation and A, B values are assumed to be in meters
//    The output x, y, z, values are in meters
//
//
//  An alternate way of doing this computation would be:
//
//  N = A/vcl_sqrt(1 - e*e*vcl_sin(lat)*vcl_sin(lat))
//  x = (N + el)vcl_cos(lat)vcl_cos(lon)
//  y = (N + el)vcl_cos(lat)vcl_sin(lon)
//  z = (N(1-e*e) + el) vcl_sin(lat)
//**********************************************************************

void latlong_to_GRS
  (double geodetic_lat,
   double lon,
   double el,         //!< Input coordinates of point
   double *x,
   double *y,
   double *z,         //!< Output GRS coords
   double A,
   double B)          //!< Major and minor axes of earth
{
  double geocentric_lat;
  double local_radius; //!< Local distance to earth center
  double c, s;

  geocentric_lat = geo_detic2centric(geodetic_lat, A, B);

  // Compute the sin and cos of the latitude
  s = vcl_sin(geocentric_lat);
  c = vcl_cos(geocentric_lat);


  // Compute the distance to the centre of the earth
  local_radius = (A*B) / vcl_sqrt(B*B*c*c + A*A*s*s);

  *x =  (local_radius*c + el*vcl_cos(geodetic_lat))*vcl_cos(lon);
  *y =  (local_radius*c + el*vcl_cos(geodetic_lat))*vcl_sin(lon);
  *z =  (local_radius*s + el*vcl_sin(geodetic_lat));

#ifdef DEBUG_DATUM
  double tlat, tlon, tel;
  GRS_to_latlong(*x, *y, *z, &tlat, &tlon, &tel, A, B);
  vcl_cout << "Error in computation: (" << (tlat-geodetic_lat) << ", " << (tlon-lon) << ", " << (tel-el) << ")\n";
#endif
}

//*************************************************************************
//      FUNCTION: nad27n_to_wgs72()
//*************************************************************************

void nad27n_to_wgs72
  (
   double phi,                 //!< input lat, lon, elev coord (degrees)
   double lamda,
   double height,
   double *wgs72_phi,          //!< lat in wgs72 (degrees)
   double *wgs72_lamda,        //!< lon in wgs72 (degrees)
   double *wgs72_hgt)          //!< elev in wgs72 (meters)
{
  double wgs84_phi, wgs84_lamda, wgs84_hgt;

  nad27n_to_wgs84(phi, lamda, height,
                  &wgs84_phi, &wgs84_lamda, &wgs84_hgt);

  wgs84_to_wgs72(wgs84_phi, wgs84_lamda, wgs84_hgt,
                 wgs72_phi, wgs72_lamda, wgs72_hgt);
}


//*************************************************************************
//      FUNCTION: wgs72_to_nad27n()
//*************************************************************************

void wgs72_to_nad27n
  (
   double phi,           //!< input lat, lon, elev coord (degrees)
   double lamda,
   double height,
   double *nad27n_phi,   //!< lat in nad27n (degrees)
   double *nad27n_lamda, //!< lon in nad27n (degrees)
   double *nad27n_hgt)   //!< elev in nad27n (meters)
{
  double wgs84_phi, wgs84_lamda, wgs84_hgt;

  wgs72_to_wgs84(phi, lamda, height,
                 &wgs84_phi, &wgs84_lamda, &wgs84_hgt);

  wgs84_to_nad27n(wgs84_phi, wgs84_lamda, wgs84_hgt,
                  nad27n_phi, nad27n_lamda, nad27n_hgt);
}

