// This is oul/ouel/kalman_filter.cxx
#include <iostream>
#include <cstdlib>
#include "kalman_filter.h"
//:
// \file
// \brief kalman_filter.cc: A linear Kalman filter class
//
// Implementation of a linear Kalman filter. We want to estimate the
// value of a time-varying signal from a measurement and predict the
// next value of the signal.
//
// The relationship between signal and measurement is:
// z(k)        =  H(k)    *    x(k)     +    v(k)
// measurement    relation     signal        noise
//                matrix
//
// The relationship between signals at different times is:
// x(k+1)      =  A(k)    *    x(k)   +   B(k)   *   u(k)   +   w(k)
// signal         relation     signal     control    control    noise
//                matrix                  matrix     signal
//
// Copyright (c) 1999 Brendan McCane
// University of Otago, Dunedin, New Zealand
// Reproduction rights limited as described in the COPYRIGHT file.
//----------------------------------------------------------------------

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/algo/vnl_svd.h>

//----------------------------------------------------------------------
//: Constructor
//
// Initialises the necessary parameters and matrices
//
// \param ns  number of dimensions in signal
// \param nm  number of dimensions in measurement
// \param nc  number of dimensions in control input
//
// \param A   the ns*ns matrix relating signal at time k to signal at time k+1.
//
// \param H   the nm*ns matrix relating the measurement to the signal.
//
// \param B   the nc*ns matrix relating the signal to the control input.
//   I'm not sure what this is used for, but I've included it for completeness.
//
// \param x_initial  the initial ns*1 estimate of the signal.
//
// \param p_initial  the initial error variance vector.
//        P = p_initial.transpose()*p_initial.
//
// \todo   under development
// \author Brendan McCane
//----------------------------------------------------------------------

KalmanFilter::KalmanFilter(
  unsigned int ns, unsigned int nm, unsigned int nc,
  const vnl_matrix<double> &Ai,
  const vnl_matrix<double> &Hi,
  const vnl_matrix<double> &Bi,
  const vnl_matrix<double> &z_initial,
  const vnl_matrix<double> &x_initial,
  const vnl_matrix<double> &Pi
  ):
  // initialise all the matrices etc
  num_signal_dimensions(ns), num_measurement_dimensions(nm),
  num_control_dimensions(nc), A(Ai), H(Hi), B(Bi),
  x(x_initial), x_pred(x_initial), z(z_initial),
  P(Pi), K(ns, nm)
{
  // do some size checking
  if ((A.rows()!=num_signal_dimensions)||
    (A.cols()!=num_signal_dimensions))
  {
    std::cerr << "Error in Kalman constructor:\n"
             << "\tMatrix A must be of size ns*ns\n";
    std::exit(-1);
  }

  if ((H.rows()!=num_measurement_dimensions)||
    (H.cols()!=num_signal_dimensions))
  {
    std::cerr << "Error in Kalman constructor:\n"
             << "\tMatrix H must be of size nm*ns\n";
    std::exit(-1);
  }

  if ((B.cols()!=num_control_dimensions)||
    (B.rows()!=num_signal_dimensions))
  {
    std::cerr << "Error in Kalman constructor:\n"
             << "\tMatrix B must be of size ns*nc\n";
    std::exit(-1);
  }

  if ((x_pred.rows()!=num_signal_dimensions)||
    (x_pred.cols()!=1))
  {
    std::cerr << "Error in Kalman constructor:\n"
             << "\tMatrix x must be of size ns*1\n";
    std::exit(-1);
  }

  if ((Pi.rows()!=num_signal_dimensions)||
    (Pi.cols()!=num_signal_dimensions))
  {
    std::cerr << "Error in Kalman constructor:\n"
             << "\tMatrix p_initial must be of size ns*1\n";
    std::exit(-1);
  }
}


//----------------------------------------------------------------------
//: Constructor
//
// Initialises the necessary parameters and matrices
//
// \param ns  number of dimensions in signal
// \param nm  number of dimensions in measurement
//
// \param A   the ns*ns matrix relating signal at time k to signal at time k+1.
//
// \param H   the nm*ns matrix relating the measurement to the signal.
//
// \param x_initial  the initial ns*1 estimate of the signal.
//
// \param p_initial  the initial error variance vector.
//        P = p_initial.transpose()*p_initial.
//
// \todo   under development
// \author Brendan McCane
//----------------------------------------------------------------------
KalmanFilter::KalmanFilter(
  unsigned int ns, unsigned int nm,
  const vnl_matrix<double> &Ai,
  const vnl_matrix<double> &Hi,
  const vnl_matrix<double> &z_initial,
  const vnl_matrix<double> &x_initial,
  const vnl_matrix<double> &Pi
  ):
  // initialise all the matrices etc
  num_signal_dimensions(ns), num_measurement_dimensions(nm),
  num_control_dimensions(0), A(Ai), H(Hi),
  x(x_initial), x_pred(x_initial), z(z_initial),
  P(Pi), K(ns, nm)
{
  // do some size checking
  if ((A.rows()!=num_signal_dimensions)||
    (A.cols()!=num_signal_dimensions))
  {
    std::cerr << "Error in Kalman constructor:\n"
             << "\tMatrix A must be of size ns*ns\n";
    std::exit(-1);
  }

  if ((H.rows()!=num_measurement_dimensions)||
    (H.cols()!=num_signal_dimensions))
  {
    std::cerr << "Error in Kalman constructor:\n"
             << "\tMatrix H must be of size nm*ns\n";
    std::exit(-1);
  }

  if ((x_pred.rows()!=num_signal_dimensions)||
    (x_pred.cols()!=1))
  {
    std::cerr << "Error in Kalman constructor:\n"
             << "\tMatrix x must be of size ns*1\n";
    std::exit(-1);
  }

  if ((Pi.rows()!=num_signal_dimensions)||
    (Pi.cols()!=num_signal_dimensions))
  {
    std::cerr << "Error in Kalman constructor:\n"
             << "\tMatrix p_initial must be of size ns*1\n";
    std::exit(-1);
  }
}

//----------------------------------------------------------------------
//: measurement_update
//
// Calculate the Kalman gain, update the signal estimate from the last
// time point using the previous estimate and the new measurement, and
// update the error covariance matrix.
//
// \param zk the new measurement
// \param Rk the measurement error covariance matrix
//
// \todo   under development
// \author Brendan McCane
//----------------------------------------------------------------------

void KalmanFilter::measurement_update(const vnl_matrix<double> &zk,
                                      const vnl_matrix<double> &Rk)
{
  // do some checks
  if ((zk.rows()!=num_measurement_dimensions)||
    (zk.cols()!=1))
  {
    std::cerr << "Error in Kalman measurement_update\n"
             << "\tzk must have dimensions nm*1\n";
    std::exit(-1);
  }

  if ((Rk.rows()!=num_measurement_dimensions)||
    (Rk.cols()!=num_measurement_dimensions))
  {
    std::cerr << "Error in Kalman measurement_update\n"
             << "\tRk must have dimensions nm*nm\n";
    std::exit(-1);
  }

  this->z = zk;

  // first calculate the Kalman gain
  vnl_matrix<double> Tmp(Rk);

  Tmp += H*P*H.transpose();

  vnl_svd<double> Tmp2(Tmp);    // Singular value decomp inverse

  K = P*H.transpose()*Tmp2.inverse();

  // update the signal estimate using the previous estimate plus the
  // measurement.
  // since zk = H*x + v, zk-H*x is an error estimate
  x = x_pred + K*(zk-H*x_pred);

  // update the error covariance
  vnl_matrix<double> ident(P);
  ident.set_identity();

  P = (ident - K*H)*P;
}

//----------------------------------------------------------------------
//: predict
//
// Calculate the next state given the current estimate and project the
// error covariance matrix ahead in time as well. In this case, I am
// assuming there is no control input.
//
// \param Qk  the process error covariance matrix
//
// \todo   under development
// \author Brendan McCane
//----------------------------------------------------------------------

vnl_matrix<double>
KalmanFilter::predict(const vnl_matrix<double> &Qk)
{
  // do some checks
  if ((Qk.rows()!=num_signal_dimensions)||
    (Qk.cols()!=num_signal_dimensions))
  {
    std::cerr << "Error in Kalman predict\n"
             << "\tQk must have dimensions ns*ns\n";
    std::exit(-1);
  }

  // estimate the new state
  x_pred = A*x;

  // estimate the new error covariance matrix
  P = A*P*A.transpose() + Qk;

  return x_pred;
}

//----------------------------------------------------------------------
//: predict
//
// Calculate the next state given the current estimate and project the
// error covariance matrix ahead in time as well.
//
// \param Qk  the process error covariance matrix
// \param uk  the control input
//
// \todo   under development
// \author Brendan McCane
//----------------------------------------------------------------------

vnl_matrix<double>
KalmanFilter::predict(const vnl_matrix<double> &Qk,
                      const vnl_matrix<double> &uk)
{
  // do some checks
  if ((Qk.rows()!=num_signal_dimensions)||
    (Qk.cols()!=num_signal_dimensions))
  {
    std::cerr << "Error in Kalman predict\n"
             << "\tQk must have dimensions ns*ns\n";
    std::exit(-1);
  }

  if ((uk.rows()!=num_control_dimensions)||
    (uk.cols()!=1))
  {
    std::cerr << "Error in Kalman predict\n"
             << "\tuk must have dimensions nc*1\n";
    std::exit(-1);
  }

  // estimate the new state
  x_pred = A*x + B*uk;

  // estimate the new error covariance matrix
  P = A*P*A.transpose() + Qk;

  return x_pred;
}

//----------------------------------------------------------------------
//: update_predict
//
// A wrapper function to both update the measurement and predict the
// new state.
//
// \param zk  the new measurement
// \param Rk  the measurement error covariance matrix
// \param Qk  the process error covariance matrix
//
// \todo   under development
// \author Brendan McCane
//----------------------------------------------------------------------

vnl_matrix<double> KalmanFilter::update_predict
(
  const vnl_matrix<double> &zk,
  const vnl_matrix<double> &Rk,
  const vnl_matrix<double> &Qk
)
{
  measurement_update(zk, Rk);
  return predict(Qk);
}

//----------------------------------------------------------------------
//: output operator
//
// \param kf   the Kalman filter to output
//
// \todo   under development
// \author Brendan McCane
//----------------------------------------------------------------------

std::ostream &operator<<(std::ostream &os, const KalmanFilter &kf)
{
  os << "Current state of Kalman Filter is:\n"
     << "estimate = " << kf.x.transpose()
     << "prediction = " << kf.x_pred.transpose()
     << "measurement = " << kf.z.transpose()
     << "error covariance =\n" << kf.P
     << "Kalman gain =\n" << kf.K
     << "A =\n" << kf.A
     << "H =\n" << kf.H;
  if (kf.num_control_dimensions>0)
    os << "B =\n" << kf.B;
  return os;
}
