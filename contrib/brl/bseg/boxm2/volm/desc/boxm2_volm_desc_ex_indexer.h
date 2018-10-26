#pragma once
// This is brl/bbas/volm/desc/boxm2_volm_desc_ex_indexer.h
#ifndef boxm2_volm_desc_ex_indexer_h_
#define boxm2_volm_desc_ex_indexer_h_
// \file
// \brief  A class to contain the observable scene from each hypothesized location, each location has a volm_ex_descriptor
//
// \author Yi Dong
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
#include <volm/desc/volm_desc_indexer.h>
#include <volm/desc/volm_desc_ex.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_buffered_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>


class volm_desc_ex_indexer : public volm_desc_indexer
{
public:
  static std::string&  get_name();
  //: constructor
  volm_desc_ex_indexer(std::string const& index_folder,
                       std::string const& out_index_folder,
                       std::vector<double> const& radius,
                       std::vector<double> const& depth_interval,
                       unsigned index_layer_size,
                       float ind_buffer,
                       unsigned const& norients = 3,
                       unsigned const& nlands = volm_label_table::compute_number_of_labels(),
                       unsigned char const& initial_mag = 0);

  //: Destructor
  ~volm_desc_ex_indexer() override = default;

  //: load the volm_geo_index for given tile
  bool get_next() override;

  //: extract histogram for a location
  bool extract(double lat, double lon, double elev, std::vector<unsigned char>& values) override;

  //: get the name of the indexer
  std::string get_index_type_str() override { return volm_desc_ex_indexer::get_name(); }

  //: generate parameter files
  bool write_params_file() override;

  //: return the size of the histogram
  unsigned layer_size() override { return layer_size_; }

private:
  //: histogram parameters
  std::vector<double> radius_;
  unsigned         norients_;
  unsigned           nlands_;
  unsigned       layer_size_;
  unsigned char initial_mag_;


  //: directory where the previous created indices is stored
  std::string index_folder_;

  //: indices
  float  ind_buffer_;
  unsigned index_layer_size_;
  boxm2_volm_wr3db_index_sptr ind_dist_;
  boxm2_volm_wr3db_index_sptr ind_comb_;

  //: depth interval table
  std::vector<double> depth_interval_;

};

#endif  // volm_desc_ex_indexer_h_
