#include "boxm2_block_metadata.h"
//:
// \file
#include <boxm2/boxm2_data_traits.h>

//: Calculates the number of data cells allocated given max_mb
long boxm2_block_metadata::num_data_cells()
{
  const int MAX_BYTES    = this->max_mb_*1024*1024;
  const int BUFF_LENGTH  = vcl_pow((float)2,(float)16); //65536
  const float ALPHA_INIT = -vcl_log(1.0f - this->p_init_) / (this->sub_block_dim_.x());
  vcl_cerr << "ALPHA_INIT = " << ALPHA_INIT << '\n';

  //total number of (sub) blocks in the scene
  int total_blocks =  this->sub_block_num_.x()
                    * this->sub_block_num_.y()
                    * this->sub_block_num_.z();
  int blockBytes = total_blocks*(sizeof(int) + 16*sizeof(char)); //16 byte tree, 4 byte int pointer
  int freeBytes = MAX_BYTES - blockBytes;
  int dataSize  = 8*sizeof(char) +    //MOG
                  4*sizeof(short) +   //numObs
                  sizeof(float) +     //alpha
                  4*sizeof(int);      //aux data (cum_seg_len
  int num_cells = (int) (freeBytes/dataSize);                         //number of cells given maxmb
  int num_buffers = (int) vcl_ceil( ((float)num_cells/(float)BUFF_LENGTH) );
  int blocks_per_buffer = (int) vcl_ceil((float)total_blocks/(float)num_buffers);
  if (num_buffers * BUFF_LENGTH <= total_blocks) {
    vcl_cerr<<"**************************************************\n"
            <<"*** boxm2_block::init_empty_block: ERROR!!!!\n"
            <<"*** Max scene size not large enough to accommodate scene dimensions\n"
            <<"*** cells allocated:  "<<num_buffers * BUFF_LENGTH<<'\n'
            <<"*** total subblocks:  "<<total_blocks<<'\n'
            <<"*** blocks per buffer:  "<<blocks_per_buffer<<'\n'
            <<"**************************************************\n";
    return 0;
  }

  vcl_cout<<"Data, num buffers: "<<num_buffers<<vcl_endl;
  return num_buffers * BUFF_LENGTH;
}

