#include "boxm2_disk_io_mgr.h"
#include "boxm2_block_id.h"

// creates a BAIO object that loads/saves block data from disk
void boxm2_disk_io_mgr::load_block(vcl_string dir, vgl_point_3d<int> block_id)
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
void boxm2_disk_io_mgr::save_block(vcl_string dir, boxm2_block* block)
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
#if 0
    // unique block id (currently 3D address)
    vgl_point_3d<int>     block_id_;

    // World dimensions of a block .e.g 1 meter x 1 meter x 1 meter
    vgl_vector_3d<double> sub_block_dim_;
    vgl_vector_3d<int> sub_block_num_;

    // high level arrays store sub block information
    vbl_array_3d<ushort2> tree_ptrs_;
    vbl_array_2d<uchar16> tree_buffers_;
    vbl_array_1d<ushort>  trees_in_buffers_;

    // info about block's trees
    int init_level_;   //each sub_blocks's init level (default 1)
    int max_level_;    //each sub_blocks's max_level (default 4)
    int max_mb_;       //each total block mb
#endif
}

