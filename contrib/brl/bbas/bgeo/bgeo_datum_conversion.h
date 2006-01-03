#ifndef bgo_datum_conversion_h
#define bgo_datum_conversion_h

extern void nad27m_to_wgs84
  (double phi,                 /* input lat, lon, elev coord (degrees) */
   double lamda, 
   double height, 
   double *wgs84_phi,          /* lat new (degrees) */
   double *wgs84_lamda,        /* lon new (degrees) */
   double *wgs84_hgt);         /* elev new (meters) */

extern void wgs84_to_nad27m
  (double phi,                 /* input lat, lon, elev coord (degrees) */
   double lamda, 
   double height, 
   double *nad27m_phi,          /* lat new (degrees) */
   double *nad27m_lamda,        /* lon new (degrees) */
   double *nad27m_hgt);         /* elev new (meters) */

extern void nad27n_to_wgs84
  (double phi,                 /* input lat, lon, elev coord (degrees) */
   double lamda, 
   double height, 
   double *wgs84_phi,          /* lat new (degrees) */
   double *wgs84_lamda,        /* lon new (degrees) */
   double *wgs84_hgt);         /* elev new (meters) */

extern void wgs84_to_nad27n
  (double phi,                 /* input lat, lon, elev coord (degrees) */
   double lamda, 
   double height, 
   double *nad27n_phi,          /* lat new (degrees) */
   double *nad27n_lamda,        /* lon new (degrees) */
   double *nad27n_hgt);         /* elev new (meters) */

extern void nad27n_to_wgs84_alternate
  (double nad27_lat, double nad27_lon, double nad27_el,
   double *wgs84_lat, double *wgs84_lon, double *wgs84_el);

extern void wgs84_to_nad27n_alternate
  (double wgs84_lat, double wgs84_lon, double wgs84_el,
   double *nad27n_lat, double *nad27n_lon, double *nad27n_el);

extern void wgs72_to_wgs84
  (
   double phi,                 /* input lat, lon, elev coord (degrees) */
   double lamda, 
   double height, 
   double *wgs84_phi,          /* lat new (degrees) */
   double *wgs84_lamda,        /* lon new (degrees) */
   double *wgs84_hgt);          /* elev new (meters) */

extern void wgs84_to_wgs72
  (
   double phi,                 /* input lat, lon, elev coord (degrees) */
   double lamda, 
   double height, 
   double *wgs72_phi,          /* lat new(degrees) */
   double *wgs72_lamda,        /* lon new (degrees) */
   double *wgs72_hgt);          /* elev new (meters) */

extern void latlong_to_GRS 
  (double geodetic_lat, 
   double lon, 
   double el,               /* Input coordinates of point */
   double *x, 
   double *y, 
   double *z,		    /* Output GRS coords */
   double A, 
   double B);		    /* Major and minor axes of earth */

extern void GRS_to_latlong 
  (double x, double y, double z,  /* Input GRS coords */
   double *geodetic_lat, 
   double *lon, 
   double *el,                    /* output coordinates of point */
   double A, 
   double B);		          /* Major and minor axes of earth */

extern double geo_detic2centric
  (double geodetic_lat,  	/* gedetic  latitude of input point */
   double A, 
   double B);	                /* Major and minor axes of earth */

extern void nad27n_to_wgs72
  (double phi,                 /* input lat, lon, elev coord (degrees) */
   double lamda, 
   double height, 
   double *wgs72_phi,          /* lat in wgs72 (degrees) */
   double *wgs72_lamda,        /* lon in wgs72 (degrees) */
   double *wgs72_hgt);         /* elev in wgs72 (meters) */

extern void wgs72_to_nad27n
  (double phi,                 /* input lat, lon, elev coord (degrees) */
   double lamda, 
   double height, 
   double *nad27n_phi,          /* lat in nad27n (degrees) */
   double *nad27n_lamda,        /* lon in nad27n (degrees) */
   double *nad27n_hgt);         /* elev in nad27n (meters) */

#endif //bgo_datum_conversion_h
