#ifndef rrel_muse_table_h_
#define rrel_muse_table_h_
//:
//  \file
//  \author Chuck Stewart
//  \date   Summer 2001
//  \brief  Look-up table for the normalization terms used in the MUSE objective function.
//

#include <vbl/vbl_array_2d.h>

//: Look-up table for the MUSET objective function.
//  Look-up table for the MUSET objective function, derived in James
//  V. Miller's 1997 PhD dissertation at Rensselaer.  An earlier
//  version of appeared in CVPR 1996.  The class computes and stores
//  statistics on the order statistics of Gaussian random variates.
//  Actually, these are for the order statistics of the absolute
//  values of Gaussian random variates.  See rrel_muset_obj for more
//  details.

class rrel_muse_table {
public:
  //: Constructor.
  //  \a table_size is the size of table (= max number of residuals
  //  pre-computed).
  rrel_muse_table( unsigned int max_n_stored );

  //: Destructor
  ~rrel_muse_table() {}

  //: Expected value of the kth ordered residual from n samples.
  //  The value is retrieved from the lookup table when possible.
  double expected_kth( unsigned int k, unsigned int n ) const;

  //: Standard deviation of the kth ordered residual from n samples.
  //  The value is retrieved from the lookup table when possible.
  double standard_dev_kth( unsigned int k, unsigned int n ) const;

  //: The divisor for trimmed statistics.
  //  The value is retrieved from the lookup table when possible.
  double muset_divisor( unsigned int k, unsigned int n ) const;


  //: The divisor for trimmed square statistics.
  //  The value is retrieved from the lookup table when possible.
  double muset_sq_divisor( unsigned int k, unsigned int n ) const;

private:
  //: Expected value of the kth ordered residual from n samples.
  //  The value is computed "from scratch".
  double calculate_expected( unsigned int k, unsigned int n ) const;

  //: Standard deviation of the kth ordered residual from n samples.
  //  The value is computed "from scratch".
  double calculate_standard_dev( unsigned int k, unsigned int n, double expected_kth ) const;

  //: The divisor for trimmed statistics.
  //  The value is computed "from scratch".
  double calculate_divisor( unsigned int k, unsigned int n, double expected_kth ) const;

  //: The divisor for trimmed squared statistics.
  //  The value is computed "from scratch".
  double calculate_sq_divisor( unsigned int k, unsigned int n, double expected_kth ) const;

private:
  //: Size of the tables.
  unsigned int max_n_stored_;

  //: Table of expected values.
  vbl_array_2d<double> expected_;

  //: Table of standard deviations.
  vbl_array_2d<double> standard_dev_;

  //: Table of divisors.
  vbl_array_2d<double> muse_t_divisor_;

  //: Table of trimmed square statistics divisors.
  vbl_array_2d<double> muse_t_sq_divisor_;
};

#endif
