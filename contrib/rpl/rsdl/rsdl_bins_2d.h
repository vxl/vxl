#ifndef rsdl_bins_2d_h_
#define rsdl_bins_2d_h_

#include <vcl_iosfwd.h>
#include <vnl/vnl_vector_fixed.h>

#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>

template < class COORD_T, class VALUE_T >
class rsdl_bins_2d_entry {
public:
  rsdl_bins_2d_entry() {}
  rsdl_bins_2d_entry( const vnl_vector_fixed< COORD_T, 2 > & pt,
                      const VALUE_T& val )
    : point_(pt), value_(val) {}
  bool operator== ( const rsdl_bins_2d_entry& right ) const // needed for vbl_array_2d
    { return point_ == right.point_ && value_ == right.value_; }
  vnl_vector_fixed<COORD_T, 2> point_;
  VALUE_T value_;
};

template < class COORD_T, class VALUE_T >
vcl_ostream& operator<< ( vcl_ostream& ostr,
                          const vcl_vector< rsdl_bins_2d_entry< COORD_T, VALUE_T > > & entries );


template  < class COORD_T, class VALUE_T >
class rsdl_bins_2d {
public:
  rsdl_bins_2d();
  rsdl_bins_2d( const vnl_vector_fixed< COORD_T, 2 > & min_pt,
                const vnl_vector_fixed< COORD_T, 2 > & max_pt,
                const vnl_vector_fixed< COORD_T, 2 > & bin_sizes );
  ~rsdl_bins_2d();

  void set_distance_tolerance( COORD_T tolerance )
    { dist_tolerance_sqr_ = tolerance * tolerance; }

  void reset( const vnl_vector_fixed< COORD_T, 2 > & min_pt,
              const vnl_vector_fixed< COORD_T, 2 > & max_pt,
              const vnl_vector_fixed< COORD_T, 2 > & bin_sizes );
  void remove_all_points( );

  void add_point( const vnl_vector_fixed< COORD_T,  2 > & pt, const VALUE_T& value );
  bool get_value( const vnl_vector_fixed< COORD_T,  2 > & pt, VALUE_T& value );
  bool change_point( const vnl_vector_fixed< COORD_T, 2 > & pt, const VALUE_T& value );
  bool change_point( const vnl_vector_fixed< COORD_T, 2 > & pt, const VALUE_T& old_val, const VALUE_T& new_val );
  bool remove_point( const vnl_vector_fixed< COORD_T, 2 > & pt );
  bool remove_point( const vnl_vector_fixed< COORD_T, 2 > & pt, const VALUE_T& value );

  void n_nearest( const vnl_vector_fixed< COORD_T, 2 > & query_pt,
                  int n,
                  vcl_vector< VALUE_T >& values ) const;

  void n_nearest( const vnl_vector_fixed< COORD_T, 2 > & query_pt,
                  int n,
                  vcl_vector< vnl_vector_fixed< COORD_T, 2 > > & points,
                  vcl_vector< VALUE_T > & values ) const;

  bool is_any_point_within_radius( const vnl_vector_fixed< COORD_T, 2 > & query_pt,
                                   COORD_T radius ) const;

  void points_within_radius( const vnl_vector_fixed< COORD_T, 2 > & query_pt,
                             COORD_T radius,
                             vcl_vector< VALUE_T >& values ) const;

  void points_within_radius( const vnl_vector_fixed< COORD_T, 2 > & query_pt,
                             COORD_T radius,
                             vcl_vector< vnl_vector_fixed< COORD_T, 2 > > & points,
                             vcl_vector< VALUE_T >& values ) const;

  bool is_any_point_in_bounding_box(  const vnl_vector_fixed< COORD_T, 2 > & min_query_pt,
                                      const vnl_vector_fixed< COORD_T, 2 > & max_query_pt ) const;

  void points_in_bounding_box( const vnl_vector_fixed< COORD_T, 2 > & min_query_pt,
                               const vnl_vector_fixed< COORD_T, 2 > & max_query_pt,
                               vcl_vector< VALUE_T  >& values ) const;

  void points_in_bounding_box( const vnl_vector_fixed< COORD_T, 2 > & min_query_pt,
                               const vnl_vector_fixed< COORD_T, 2 > & max_query_pt,
                               vcl_vector< vnl_vector_fixed< COORD_T, 2 > > & points,
                               vcl_vector< VALUE_T > & values ) const;

  void change_value( const VALUE_T& old_val, const VALUE_T& new_val );

  vnl_vector_fixed<COORD_T, 2> max_pt() {return max_pt_ ;}

private:
  void point_to_bin( COORD_T x, COORD_T y, int& bin_x, int& bin_y ) const;
  COORD_T min_sq_distance_to_bin( COORD_T x, COORD_T y, int bin_x, int bin_y ) const;

public:
  typedef rsdl_bins_2d_entry< COORD_T, VALUE_T > bin_entry_type_;
  typedef vcl_vector< bin_entry_type_ > bin_vector_type_;

private:
  vbl_array_2d< bin_vector_type_ > bins_;

  vnl_vector_fixed< COORD_T, 2 > min_pt_;
  vnl_vector_fixed< COORD_T, 2 > max_pt_;
  vnl_vector_fixed< COORD_T, 2 > bin_sizes_;

  int num_bins_x_, num_bins_y_;

  COORD_T dist_tolerance_sqr_ ;
};

#endif
