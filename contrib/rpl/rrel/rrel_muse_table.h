#ifndef rrel_muse_table_h_
#define rrel_muse_table_h_
//:
//  \file
//  \author Chuck Stewart
//  \date   Summer 2001
//  \modified May 2004 to store the values in a map
//  \brief  Look-up table for the normalization terms used in the MUSE objective function.
//

#include <vcl_map.h>

//: Look-up table for the MUSET objective function.
//  Look-up table for the MUSET objective function, derived in James
//  V. Miller's 1997 PhD dissertation at Rensselaer.  An earlier
//  version of appeared in CVPR 1996.  The class computes and stores
//  statistics on the order statistics of Gaussian random variates.
//  Actually, these are for the order statistics of the absolute
//  values of Gaussian random variates.  See rrel_muset_obj for more
//  details.

class rrel_muse_key_type {
 public:
  rrel_muse_key_type( unsigned int k, unsigned int n ) : k_(k), n_(n) {}
  unsigned int k_;
  unsigned int n_;
};

bool operator< ( rrel_muse_key_type const& left, rrel_muse_key_type const& right );


class rrel_muse_table_entry {
 public:
  rrel_muse_table_entry() : initialized_(false) {}
  bool initialized_;
  double expected_;
  double standard_dev_;
  double muse_t_divisor_;
  double muse_t_sq_divisor_;
};
  
typedef vcl_map< rrel_muse_key_type, rrel_muse_table_entry > rrel_muse_map_type;


class rrel_muse_table {
 public:

 public:
  //: Constructor.
  //  \a table_size is the size of table (= max number of residuals
  //  pre-computed).
  rrel_muse_table( unsigned int /* max_n_stored */ ) {}

  rrel_muse_table( ) {}

  //: Destructor
  ~rrel_muse_table() {}

  //: Expected value of the kth ordered residual from n samples.
  //  The value is retrieved from the lookup table when possible.
  double expected_kth( unsigned int k, unsigned int n );

  //: Standard deviation of the kth ordered residual from n samples.
  //  The value is retrieved from the lookup table when possible.
  double standard_dev_kth( unsigned int k, unsigned int n );

  //: The divisor for trimmed statistics.
  //  The value is retrieved from the lookup table when possible.
  double muset_divisor( unsigned int k, unsigned int n );


  //: The divisor for trimmed square statistics.
  //  The value is retrieved from the lookup table when possible.
  double muset_sq_divisor( unsigned int k, unsigned int n );

private:
  void calculate_all( unsigned int k, unsigned int n, rrel_muse_table_entry & entry );

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
  rrel_muse_map_type table_;
};

#endif
