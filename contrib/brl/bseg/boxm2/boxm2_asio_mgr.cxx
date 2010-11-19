#include "boxm2_asio_mgr.h"
#include "boxm2_block_id.h"
#include <vul/vul_file.h>

// creates a BAIO object that loads/saves block data from disk
void boxm2_asio_mgr::load_block(vcl_string dir, boxm2_block_id block_id)
{
    //0. open up file for writing
    vcl_ostringstream ns;  // Declare an output string stream.
    ns << dir << "block." << block_id << ".bin";
    vcl_string filename = ns.str();

    //1. read number of blocks in each dimension as integers

    //2. read number of buffers

    //3. read length of tree buffer

    //4. read tree_ptrs_

    //5. read tree_buffers_

    //6. read trees_in_buffers
}

// method of saving block
void boxm2_asio_mgr::save_block(vcl_string dir, boxm2_block* block)
{
    //0. open up file for writing
    //vcl_ostringstream ns;  // Declare an output string stream.
    //ns << dir << "block." << block->block_id() << ".bin";
    //vcl_string filename = ns.str();

    //1. write number of blocks in each dimension as integers

    //2. write number of buffers

    //3. write length of tree buffer

    //4. write tree_ptrs_

    //5. write tree_buffers_

    //6. write trees_in_buffers
}

// creates a BAIO object that loads/saves block data from disk
