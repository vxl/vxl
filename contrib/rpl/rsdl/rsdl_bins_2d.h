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
  typedef vnl_vector_fixed< COORD_T, 2 >  point_type;
  typedef VALUE_T value_type;

public:
  rsdl_bins_2d();
  rsdl_bins_2d( const point_type & min_pt,
                const point_type & max_pt,
                const point_type & bin_sizes );
  ~rsdl_bins_2d();

  void set_distance_tolerance( COORD_T tolerance )
    { dist_tolerance_sqr_ = tolerance * tolerance; }

  void reset( const point_type & min_pt,
              const point_type & max_pt,
              const point_type & bin_sizes );
  void remove_all_points( );

  void add_point( const point_type & pt, const value_type& value );
  bool get_value( const point_type & pt, value_type& value );
  bool change_point( const point_type & pt, const value_type& value );
  bool change_point( const point_type & pt, const value_type& old_val, const value_type& new_val );
  bool remove_point( const point_type & pt );
  bool remove_point( const point_type & pt, const value_type& value );

  void n_nearest( const point_type & query_pt,
                  int n,
                  vcl_vector< value_type >& values ) const;

  void n_nearest( const point_type & query_pt,
                  int n,
                  vcl_vector< point_type > & points,
                  vcl_vector< value_type > & values ) const;

  bool is_any_point_within_radius( const point_type & query_pt,
                                   COORD_T radius ) const;

  void points_within_radius( const point_type & query_pt,
                             COORD_T radius,
                             vcl_vector< value_type >& values ) const;

  void points_within_radius( const point_type & query_pt,
                             COORD_T radius,
                             vcl_vector< point_type > & points,
                             vcl_vector< value_type >& values ) const;

  bool is_any_point_in_bounding_box(  const point_type & min_query_pt,
                                      const point_type & max_query_pt ) const;

  void points_in_bounding_box( const point_type & min_query_pt,
                               const point_type & max_query_pt,
                               vcl_vector< value_type  >& values ) const;

  void points_in_bounding_box( const point_type & min_query_pt,
                               const point_type & max_query_pt,
                               vcl_vector< point_type > & points,
                               vcl_vector< value_type > & values ) const;

  void change_value( const value_type& old_val, const value_type& new_val );

  point_type max_pt() {return max_pt_ ;}
  
  point_type min_pt() {return min_pt_ ;}
  
  point_type bin_sizes() {return bin_sizes_;}



private:
  void point_to_bin( COORD_T x, COORD_T y, int& bin_x, int& bin_y ) const;
  COORD_T min_sq_distance_to_bin( COORD_T x, COORD_T y, int bin_x, int bin_y ) const;

public:
  typedef rsdl_bins_2d_entry< COORD_T, VALUE_T > bin_entry_type_;
  typedef vcl_vector< bin_entry_type_ > bin_vector_type_;

private:
  vbl_array_2d< bin_vector_type_ > bins_;

  point_type min_pt_;
  point_type max_pt_;
  point_type bin_sizes_;

  int num_bins_x_, num_bins_y_;

  COORD_T dist_tolerance_sqr_ ;
};

#endif
