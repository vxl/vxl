#include <boxm2/boxm2_block.h>
//:
// \file

boxm2_block::boxm2_block(boxm2_block_id id, char* buff)
{
  block_id_ = id;
  buffer_ = buff;
  this->b_read(buff);
}


bool boxm2_block::b_read(char* buff)
{
    long bytes_read = 0;

    //0. first 8 bytes denote size
    vcl_memcpy(&byte_count_, buff, sizeof(byte_count_));
    bytes_read += sizeof(byte_count_);

    //1. read init level, max level, max mb
    vcl_memcpy(&init_level_, buff+bytes_read, sizeof(init_level_));
    bytes_read += sizeof(init_level_);
    vcl_memcpy(&max_level_, buff+bytes_read, sizeof(max_level_));
    bytes_read += sizeof(max_level_);
    vcl_memcpy(&max_mb_, buff+bytes_read, sizeof(max_mb_));
    bytes_read += sizeof(max_mb_);

    //2. read in sub block dimension, sub block num
    double dims[4];
    vcl_memcpy(&dims, buff+bytes_read, sizeof(dims));
    bytes_read += sizeof(dims);
    int    nums[4];
    vcl_memcpy(&nums, buff+bytes_read, sizeof(nums));
    bytes_read += sizeof(nums);
    sub_block_dim_ = vgl_vector_3d<double>(dims[0], dims[1], dims[2]);
    sub_block_num_ = vgl_vector_3d<int>(nums[0], nums[1], nums[2]);

    //3.  read number of buffers
    int numBuffers;
    vcl_memcpy(&numBuffers, buff+bytes_read, sizeof(numBuffers));
    bytes_read += sizeof(numBuffers);

    //3.a read length of tree buffers
    int treeLen;
    vcl_memcpy(&treeLen, buff+bytes_read, sizeof(treeLen));
    bytes_read += sizeof(treeLen);

    //4. setup big arrays (3d block of trees)
    uchar16* treesBuff = (uchar16*) (buff+bytes_read);
    trees_     = boxm2_array_3d<uchar16>( sub_block_num_.x(),
                                          sub_block_num_.y(),
                                          sub_block_num_.z(),
                                          treesBuff);
    bytes_read += sizeof(uchar16)*sub_block_num_.x()*sub_block_num_.y()*sub_block_num_.z();

    //5. 2d array of tree pointers
    int* treePtrsBuff = (int*) (buff+bytes_read);
    tree_ptrs_ = boxm2_array_2d<int>( numBuffers, treeLen, treePtrsBuff);
    bytes_read += sizeof(int) * numBuffers * treeLen;

    //6. 1d aray of trees_in_buffers
    ushort* treesCountBuff = (ushort*) (buff+bytes_read);
    trees_in_buffers_ = boxm2_array_1d<ushort>(numBuffers, treesCountBuff);
    bytes_read += sizeof(ushort) * numBuffers;

    //7. 1d array of mem pointers
    ushort2* memPtrsBuff = (ushort2*) (buff+bytes_read);
    mem_ptrs_ = boxm2_array_1d<ushort2>(numBuffers, memPtrsBuff);
    bytes_read += sizeof(ushort2) * numBuffers;

    return true;
}

//:
//  This type of writing is sort of counter intuitive, as the buffer
//  just needs to be returned and written to disk. The first few calls
//  ensure the meta data is lined up correctly.  To use this, just pass in
//  the boxm2_block buffer.
bool boxm2_block::b_write(char* buff)
{
    long bytes_written = 0;

    //0. writing total size
    vcl_memcpy(buff, &byte_count_, sizeof(byte_count_));
    bytes_written += sizeof(byte_count_);

    //1. write init level, max level, max mb
    vcl_memcpy(buff+bytes_written, &init_level_, sizeof(init_level_));
    bytes_written += sizeof(init_level_);
    vcl_memcpy(buff+bytes_written, &max_level_, sizeof(max_level_));
    bytes_written += sizeof(max_level_);
    vcl_memcpy(buff+bytes_written, &max_mb_, sizeof(max_mb_));
    bytes_written += sizeof(max_mb_);

    //2. Write sub block dimension, sub block num
    double dims[4] = {sub_block_dim_.x(), sub_block_dim_.y(), sub_block_dim_.z(), 0.0};
    vcl_memcpy(buff+bytes_written, dims, 4 * sizeof(double));
    bytes_written += 4 * sizeof(double);

    int nums[4] = {sub_block_num_.x(), sub_block_num_.y(), sub_block_num_.z(), 0 };
    vcl_memcpy(buff+bytes_written, nums, 4 * sizeof(int));
    bytes_written += 4 * sizeof(int);

    //3.  write number of buffers
    int numBuffers = tree_ptrs_.rows();
    vcl_memcpy(buff+bytes_written, &numBuffers, sizeof(numBuffers));
    bytes_written += sizeof(numBuffers);

    //3.a write length of tree buffers
    int treeLen = tree_ptrs_.cols();
    vcl_memcpy(buff+bytes_written, &treeLen, sizeof(treeLen));
    bytes_written += sizeof(treeLen);

    //the arrays themselves should be already in the char buffer, so no need to copy
    return true;
}


//------------ I/O -------------------------------------------------------------
#if 0
void vsl_b_write(vsl_b_ostream& os, boxm2_block const& scene) {}
void vsl_b_write(vsl_b_ostream& os, const boxm2_block* &p) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_block_sptr& sptr) {}
#endif
//: Binary write boxm2_block to stream.
void vsl_b_write(vsl_b_ostream& os, boxm2_block_sptr const& sptr) {}

#if 0
void vsl_b_read(vsl_b_istream& is, boxm2_block &scene) {}
void vsl_b_read(vsl_b_istream& is, boxm2_block* p) {}
#endif
//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_block_sptr& sptr) {}
//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_block_sptr const& sptr) {}

