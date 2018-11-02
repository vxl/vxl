/* Copyright 2010 Brad King
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#include <iostream>
#include <rtvl/rtvl_tensor.h>
#include <rtvl/rtvl_vote.h>
#include <rtvl/rtvl_votee.h>
#include <rtvl/rtvl_voter.h>
#include <rtvl/rtvl_weight_smooth.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  std::cout << "voter tensor = " << std::endl << voter_matrix;
  std::cout << "voter stickness = " << voter_tensor.saliency(0) << std::endl;
  std::cout << "voter ballness = " << voter_tensor.saliency(1) << std::endl;

  // Use "rtvl_voter" to encapsulate a token (location + input tensor).
  rtvl_voter<2> voter(voter_location, voter_tensor);

  vnl_vector_fixed<double, 2> votee_location;
  votee_location[0] = 1;
  votee_location[1] = 0;
  vnl_matrix_fixed<double, 2, 2> votee_matrix(0.0);

  // Use "rtvl_votee" to encapsulate a site (location + output tensor).
  rtvl_votee<2> votee(votee_location, votee_matrix);

  std::cout << std::endl;

  // Choose a weight profile, initialized with spatial scale.
  rtvl_weight_smooth<2> tvw(1.0);

  // Compute one vote.
  rtvl_vote(voter, votee, tvw);

  // Decompose the result.
  rtvl_tensor<2> votee_tensor(votee_matrix);
  std::cout << "votee tensor = " << std::endl << votee_matrix;
  std::cout << "votee stickness = " << votee_tensor.saliency(0) << std::endl;
  std::cout << "votee ballness = " << votee_tensor.saliency(1) << std::endl;

  return 0;
}
