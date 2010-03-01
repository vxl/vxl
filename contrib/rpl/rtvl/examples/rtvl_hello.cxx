/* Copyright 2010 Brad King
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#include <rtvl/rtvl_tensor.hxx>
#include <rtvl/rtvl_vote.hxx>
#include <rtvl/rtvl_votee.hxx>
#include <rtvl/rtvl_voter.hxx>
#include <rtvl/rtvl_weight_smooth.hxx>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vcl_iostream.h>

int main()
{
  // Use any SPD voter matrix.
  vnl_matrix_fixed<double, 2, 2> voter_matrix;
  voter_matrix(0,0) = 0.2;
  voter_matrix(1,0) = voter_matrix(0,1) = 0.1;
  voter_matrix(1,1) = 1;
  vnl_vector_fixed<double, 2> voter_location(0.0);

  // Use "rtvl_tensor" to decompose the matrix.
  rtvl_tensor<2> voter_tensor(voter_matrix);
  vcl_cout << "voter tensor = " << vcl_endl << voter_matrix;
  vcl_cout << "voter stickness = " << voter_tensor.saliency(0) << vcl_endl;
  vcl_cout << "voter ballness = " << voter_tensor.saliency(1) << vcl_endl;

  // Use "rtvl_voter" to encapsulate a token (location + input tensor).
  rtvl_voter<2> voter(voter_location, voter_tensor);

  vnl_vector_fixed<double, 2> votee_location;
  votee_location[0] = 1;
  votee_location[1] = 0;
  vnl_matrix_fixed<double, 2, 2> votee_matrix(0.0);

  // Use "rtvl_votee" to encapsulate a site (location + output tensor).
  rtvl_votee<2> votee(votee_location, votee_matrix);

  vcl_cout << vcl_endl;

  // Choose a weight profile, initialized with spatial scale.
  rtvl_weight_smooth<2> tvw(1.0);

  // Compute one vote.
  rtvl_vote(voter, votee, tvw);

  // Decompose the result.
  rtvl_tensor<2> votee_tensor(votee_matrix);
  vcl_cout << "votee tensor = " << vcl_endl << votee_matrix;
  vcl_cout << "votee stickness = " << votee_tensor.saliency(0) << vcl_endl;
  vcl_cout << "votee ballness = " << votee_tensor.saliency(1) << vcl_endl;

  return 0;
}
