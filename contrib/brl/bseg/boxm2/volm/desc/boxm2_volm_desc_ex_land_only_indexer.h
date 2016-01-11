#pragma once
// This is brl/bseg/volm/desc/boxm2_volm_desc_ex_land_only_indexer.h
#ifndef boxm2_volm_desc_ex_land_only_indexer_h_
#define boxm2_volm_desc_ex_land_only_indexer_h_
// \file
// \brief  A class to contain the observable scene from each hypothesized location, each location has a volm_ex_descriptor
//         with only land type considered
//
// \author Yi Dong
// \date June 11, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
#include <volm/desc/volm_desc_indexer.h>
#include <volm/desc/volm_desc_ex_land_only.h>
#include <volm_io.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_buffered_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>

class volm_desc_ex_land_only_indexer : public volm_desc_indexer
{
public:
  static vcl_string get_name();

  //: Constructor
  volm_desc_ex_land_only_indexer(vcl_string const& index_folder,
                                 vcl_string const& out_index_folder,
                                 vcl_vector<double> const& radius,
                                 vcl_vector<double> const& depth_interval,
                                 unsigned index_layer_size,
                                 float ind_buffer,
                                 unsigned const& nlands = volm_label_table::compute_number_of_labels(),
                                 unsigned char const& initial_mag = 0);

  //: Desturctor
  ~volm_desc_ex_land_only_indexer() {}

  //: Load the volm_geo_index for given tile
  virtual bool get_next();

  //: extract histogram for a location
  virtual bool extract(double lat, double lon, double elev, vcl_vector<unsigned char>& values);

  //: get the name of the indexer
  virtual vcl_string get_index_type_str() { return volm_desc_ex_land_only_indexer::get_name(); }

  //: generate parameter files
  virtual bool write_params_file();

  //: return the size of the histogram
  virtual unsigned layer_size() { return layer_size_; }

private:
  //: histogram parameters
  vcl_vector<double> radius_;
  unsigned           nlands_;
  unsigned       layer_size_;
  unsigned char initial_mag_;


  //: directory where the previous created indices is stored
  vcl_string index_folder_;

  //: indices
  float    ind_buffer_;
  unsigned index_layer_size_;
  boxm2_volm_wr3db_index_sptr ind_dist_;
  boxm2_volm_wr3db_index_sptr ind_comb_;

  //: depth interval table
  vcl_vector<double> depth_interval_;

};

#endif // boxm2_volm_desc_ex_land_only_indexer_h_
