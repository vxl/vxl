#ifndef vgl_distance_h_
#define vgl_distance_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// squared distance
double vgl_distance2_to_linesegment(double x1, double y1,
				    double x2, double y2,
				    double x, double y);

double vgl_distance_to_linesegment(double x1, double y1,
				   double x2, double y2,
				   double x, double y);

double vgl_distance_to_non_closed_polygon(float const px[], float const py[], unsigned n,
					  double x, double y);

double vgl_distance_to_closed_polygon(float const px[], float const py[], unsigned n,
				      double x, double y);

#endif
