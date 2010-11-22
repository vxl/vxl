//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <boxm2/boxm2_block_id.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_sio_mgr.h>
#include <boxm2/boxm2_asio_mgr.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "test_utils.h"

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vnl/vnl_random.h>

void test_asio_data()
{
    //: initalize generic random data buffer
    const unsigned int array_size=1024;
    float * farray      = new float[array_size];
    float * farray_bkup = new float[array_size];
    vnl_random rand;
    boxm2_asio_mgr asio_mgr;
    for (unsigned i =0;i<array_size;++i)
    {
      farray[i]=(float)rand.drand32(0,100);
      farray_bkup[i]=farray[i];
    }

    //: create new block from data
    boxm2_block_id id(0,0,0);
    char * buffer = reinterpret_cast<char *>(farray);
    boxm2_data<BOXM2_ALPHA>* test_data = new boxm2_data<BOXM2_ALPHA>(buffer, array_size*sizeof(float), id);
    
    
    //: asynch write to disk while counting flops
    //: TODO MAKE THIS THROUGH ASIO MANAGER
    baio write_status;
    asio_mgr.save_block_data<BOXM2_ALPHA>("", id, test_data, write_status);
    int nflops=0;
    while (write_status.status()==BAIO_IN_PROGRESS) nflops++;
    write_status.close_file();
    delete test_data; 


    //: write a bunch of data blocks - 8 of em to disk
    
    //: do sam e
    

    //: recreate file name for reading
    vcl_ostringstream ns;  // Declare an output string stream.
    ns << "" << boxm2_data_traits<BOXM2_ALPHA>::prefix() << id << ".bin";
    vcl_string filename = ns.str();

    unsigned long bytecount=vul_file::size(filename);
    TEST("Successful Asynchronous Write ",bytecount,array_size*sizeof(float));

    //: asynch read and count flops TODO: this is sort of a hacky way to test a data block
    baio read_status;
    asio_mgr.load_block_data<BOXM2_ALPHA>("",id,read_status);
    while (read_status.status()==BAIO_IN_PROGRESS) nflops++;
    boxm2_data<BOXM2_ALPHA>* read_data = new boxm2_data<BOXM2_ALPHA>(read_status.buffer(), bytecount, id);
    char * bufferread  = read_data->data_buffer();
    char * buffer_bkup = reinterpret_cast<char *>(farray_bkup);

    bool flag=true;
    for (unsigned i=0;i<bytecount;i++)
    {
        if (bufferread[i]!=buffer_bkup[i])
        {
            flag=false;
            vcl_cout<<"Orig val "<<(int) buffer_bkup[i]<<" Read val "<< (int) bufferread[i]<<vcl_endl;
        }
    }
    TEST("Successful Asynchronous Read", true, flag);
    
    //: clean up data block
    delete read_data;
    
    //: clean up buffers 
    if(farray_bkup) delete [] farray_bkup;
}

static void test_asio_blocks()
{
  //: load one block from disk with blocking (like we'd imagine startup)'
  boxm2_block* loaded = boxm2_sio_mgr::load_block("", boxm2_block_id(0,0,0));
  
  //: send some ASIO requests to ASIO manager
  boxm2_asio_mgr mgr; 
  mgr.load_block("", boxm2_block_id(0,0,1));
  mgr.load_block("", boxm2_block_id(0,1,0)); 
  mgr.load_block("", boxm2_block_id(0,1,1)); 
  mgr.load_block("", boxm2_block_id(1,0,1)); 
  mgr.load_block("", boxm2_block_id(1,1,0)); 
  mgr.load_block("", boxm2_block_id(1,1,1)); 

  //: check to see which block is ready, do some computation 
  vcl_vector<boxm2_block*> block_list; 
  int flopCount = 0;
  while(block_list.size() < 6)  {
    typedef vcl_map<boxm2_block_id, boxm2_block*> maptype;
    maptype lmap = mgr.get_loaded_blocks(); 
    maptype::iterator iter;
    for(iter = lmap.begin(); iter != lmap.end(); ++iter) 
      block_list.push_back(iter->second); 
    flopCount++; 
  }
  
  //: report flop count and block ids
  vcl_cout<<"flop count: "<<flopCount<<vcl_endl;
  for(int i=0; i<block_list.size(); ++i) {
    vcl_cout<<block_list[i]->block_id()<<vcl_endl;
  }
  
  //: test two of the blocks - make sure they're the same
  boxm2_test_utils::test_block_equivalence(*loaded, *block_list[3]); 

  //: delete blocks loaded 
  delete loaded;
  for(int i=0; i<block_list.size(); ++i)
    delete block_list[i];
  
}


void test_io()
{
  
  //: ensure 8 test blocks are saved to disk
  int numBuffers = 50;
  int treeLen = 1200;
  int init_level = 1;
  int max_level  = 4;
  int max_mb     = 400;
  int nums[4] = { 30, 40, 50, 0 };
  double dims[4] = { 0.2, 0.4, 0.6, 0.0 };
  for(int i=0; i<2; i++) {
    for(int j=0; j<2; j++) {
      for(int k=0; k<2; k++) {
        char* stream = boxm2_test_utils::construct_block_test_stream( numBuffers, 
                                                                      treeLen, 
                                                                      nums, 
                                                                      dims, 
                                                                      init_level,
                                                                      max_level,
                                                                      max_mb );
        boxm2_block b(boxm2_block_id(i,j,k), stream);  
        boxm2_sio_mgr::save_block("", &b); 
      }
    }
  }
  
  //: ensure 8 data blocks are saved to disk
  
  //: run some aio tests on blocks
  test_asio_blocks(); 
  
  
}


TESTMAIN(test_io);
