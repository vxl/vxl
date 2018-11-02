// This is oul/ouel/kalman_filter.h
// Copyright (c) 1999 Brendan McCane
// University of Otago, Dunedin, New Zealand
// Reproduction rights limited as described in the COPYRIGHT file.
//----------------------------------------------------------------------
#ifndef OTAGO_kalman_filter__h_INCLUDED
#define OTAGO_kalman_filter__h_INCLUDED
//:
// \file
// \brief A linear Kalman filter class
//
// Implementation of a linear Kalman filter. We want to estimate the
// value of a time-varying signal from a measurement and predict the
// next value of the signal.
//
// The relationship between signal and measurement is:
// \verbatim
// z(k)        =  H(k)    *    x(k)     +    v(k)
// measurement    relation     signal        noise
//                matrix
// \endverbatim
//
// The relationship between signals at different times is:
// \verbatim
// x(k+1)      =  A(k)    *    x(k)   +   B(k)   *   u(k)   +   w(k)
// signal         relation     signal     control    control    noise
//                matrix                  matrix     signal
// \endverbatim
//
// Status: Completed
// \author Brendan McCane
//----------------------------------------------------------------------

#include <iostream>
#include <iosfwd>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class KalmanFilter
{
  // The number of dimensions in signal (ns)
  unsigned int num_signal_dimensions;

  // The number of dimensions in measurement (nm)
  unsigned int num_measurement_dimensions;

  // The number of dimensions in control input (nc)
  unsigned int num_control_dimensions;

  // The relation between one signal and the next (ns*ns)
  vnl_matrix<double> A;

  // The relation between measurement and signal (nm*ns)
  vnl_matrix<double> H;

  // The control input relation  (nc*ns)
  vnl_matrix<double> B;

  // The signal vector estimate - actually a column matrix (ns*1)
  vnl_matrix<double> x;

  // The signal vector prediction - actually a column matrix (ns*1)
  vnl_matrix<double> x_pred;

  // The measurement vector - again a column matrix (nm*1)
  vnl_matrix<double> z;

  // The error covariance matrix  (ns*ns)
  vnl_matrix<double> P;

  // The Kalman gain matrix (ns*nm)
  vnl_matrix<double> K;

 public:
  KalmanFilter(unsigned int ns, unsigned int nm, unsigned int nc,
               const vnl_matrix<double> &Ai,
               const vnl_matrix<double> &Hi,
               const vnl_matrix<double> &Bi,
               const vnl_matrix<double> &z_initial,
               const vnl_matrix<double> &x_initial,
               const vnl_matrix<double> &Pi);

  // this version does not have a control input
  KalmanFilter(unsigned int ns, unsigned int nm,
               const vnl_matrix<double> &Ai,
               const vnl_matrix<double> &Hi,
               const vnl_matrix<double> &z_initial,
               const vnl_matrix<double> &x_initial,
               const vnl_matrix<double> &Pi);

  // some utility functions
  void set_initial_input(const vnl_matrix<double> &x_initial)
    {x_pred = x_initial;}

  // zk is the next input measurement
  // Rk is the measurement  error covariance matrix
  //    which is calculated from the noise distribution
  void measurement_update(const vnl_matrix<double> &zk,
                          const vnl_matrix<double> &Rk);

  // Qk is the process error covariance matrix
  //    which is calculated from the noise distribution
  // The predicted value of x(k+1) is returned
  vnl_matrix<double> predict(const vnl_matrix<double> &Qk);

  // Qk is the process error covariance matrix
  //    which is calculated from the noise distribution
  // uk is the control input
  // The predicted value of x(k+1) is returned
  vnl_matrix<double> predict(const vnl_matrix<double> &Qk,
                             const vnl_matrix<double> &uk);

  // Do both measurement update and predict
  vnl_matrix<double> update_predict(const vnl_matrix<double> &zk,
                                    const vnl_matrix<double> &Rk,
                                    const vnl_matrix<double> &Qk);

  // accessor functions
  // return the current signal estimate
  inline vnl_matrix<double> estimate() const {return x;}
  // return the current signal prediction
  inline vnl_matrix<double> prediction() const {return x_pred;}

  friend std::ostream &operator<<(std::ostream &os, const KalmanFilter &kf);
};

#endif // OTAGO_kalman_filter__h_INCLUDED
