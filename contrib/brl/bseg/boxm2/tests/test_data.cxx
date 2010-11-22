//:
// \file
// \author vishal JAin
// \date May 17, 2010
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block_id.h>
#include <boxm2/boxm2_sio_mgr.h>
#include <boxm2/boxm2_asio_mgr.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vnl/vnl_random.h>
#include <vul/vul_file.h>

void test_data_sio()
{
    const unsigned int array_size=1024;
    float * farray      = new float[array_size];
    float * farray_bkup = new float[array_size];
    vnl_random rand;
    for (unsigned i =0;i<array_size;++i)
    {
        farray[i]=(float)rand.drand32(0,100);
        farray_bkup[i]=farray[i];
    }

    //create new block from farray, save it and delete it
    boxm2_block_id id(0,0,0);
    char * buffer = reinterpret_cast<char *>(farray);
    boxm2_data<BOXM2_ALPHA>* test_data = new boxm2_data<BOXM2_ALPHA>(buffer, array_size*sizeof(float), id);
    boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>("", id, test_data);
    delete test_data;  

    vcl_ostringstream ns;  // Declare an output string stream.
    ns << "" << boxm2_data_traits<BOXM2_ALPHA>::prefix() << id << ".bin";
    vcl_string filename = ns.str();

    unsigned long bytecount=vul_file::size(filename);
    TEST("Successful Synchronous Write ",bytecount,array_size*sizeof(float));

    boxm2_data_base * read_data = boxm2_sio_mgr::load_block_data<BOXM2_ALPHA>("",id);
    char * bufferread  = read_data->data_buffer();
    char * buffer_bkup = reinterpret_cast<char *>(farray_bkup);

    bool flag=true;
    for (unsigned i=0;i<bytecount;i++)
        if (bufferread[i]!=buffer_bkup[i])
        {
            flag=false;
            vcl_cout<<"Orig val "<<(int) buffer_bkup[i]<<" Read val "<< (int) bufferread[i]<<vcl_endl;
        }

    TEST("Successful Synchronous Read", true, flag);
      
    //clean up read_data block
    delete read_data; 

    //clean up farray_bkup and buffer_bkup
    if(farray_bkup) delete[] farray_bkup;
}

void test_data_asio()
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
    baio write_status;
    asio_mgr.save_block_data<BOXM2_ALPHA>("", id, test_data, write_status);
    int nflops=0;
    while (write_status.status()==BAIO_IN_PROGRESS) nflops++;
    write_status.close_file();
    delete test_data; 

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

static void test_data()
{
    //test_data_sio();
    test_data_asio();
}


TESTMAIN(test_data);
