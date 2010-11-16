#include <boxm2/boxm2_block.h>

boxm2_block::boxm2_block(char* buff) 
{
  this->b_read(buff);
}                 // NOT YET IMPLEMENTED


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
    
    //1. read number of blocks in each dimension as integers


    //2. read number of buffers

    //3. read length of tree buffer

    //4. read tree_ptrs_

    //5. read tree_buffers_

    //6. read trees_in_buffers
  
  return false; 
} 


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
    
    //1. write number of blocks in each dimension as integers

    //2. write number of buffers

    //3. write length of tree buffer

    //4. write tree_ptrs_

    //5. write tree_buffers_

    //6. write trees_in_buffers  
  
    return true; 
} 
