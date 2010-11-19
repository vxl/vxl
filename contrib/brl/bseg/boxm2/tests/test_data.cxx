//:
// \file
// \author vishal JAin
// \date 05/17/2010
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
    const int array_size=1024;
    float * farray=new float[array_size];
    float * farray_bkup=new float[array_size];
    vnl_random rand;
    for(unsigned i =0;i<array_size;i++)
    {
        farray[i]=(float)rand.drand32(0,100);
        farray_bkup[i]=farray[i];
    }

    boxm2_block_id id(0,0,0);
    char * buffer=reinterpret_cast<char *>(farray);
    boxm2_data<BOXM2_ALPHA> test_data(buffer, array_size*sizeof(float),id);
    boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>("",id,&test_data);
    test_data.~boxm2_data();

    vcl_ostringstream ns;  // Declare an output string stream.
    ns << "" << boxm2_data_traits<BOXM2_ALPHA>::prefix() << id << ".bin";
    vcl_string filename = ns.str();

    unsigned long bytecount=vul_file::size(filename);
    TEST("Succesful Synchronous Write ",bytecount,array_size*sizeof(float));

    boxm2_data_base * read_data=boxm2_sio_mgr::load_block_data<BOXM2_ALPHA>("",id);
    char * bufferread=read_data->data_buffer();
    char * buffer_bkup=reinterpret_cast<char *>(farray_bkup);

    bool flag=true;
    for(unsigned i=0;i<bytecount;i++)
        if(bufferread[i]!=buffer_bkup[i])
        {
            flag=false;
            vcl_cout<<"Orig val "<<(int) buffer_bkup[i]<<" Read val "<< (int) bufferread[i]<<vcl_endl;
        }

    TEST("Succesful Synchronous Read", true, flag);

}
void test_data_asio(){
    const int array_size=1024;
    float * farray=new float[array_size];
    float * farray_bkup=new float[array_size];
    vnl_random rand;
    boxm2_asio_mgr asio_mgr;
    for(unsigned i =0;i<array_size;i++)
    {
        farray[i]=(float)rand.drand32(0,100);
        farray_bkup[i]=farray[i];
    }

    boxm2_block_id id(0,0,0);
    char * buffer=reinterpret_cast<char *>(farray);
    boxm2_data<BOXM2_ALPHA> test_data(buffer, array_size*sizeof(float),id);
    baio write_status;
    asio_mgr.save_block_data<BOXM2_ALPHA>("",id,&test_data,write_status);
    int nflops=0;
    while(write_status.status()==BAIO_IN_PROGRESS)
        nflops++;
    write_status.close_file();
    test_data.~boxm2_data();

    vcl_ostringstream ns;  // Declare an output string stream.
    ns << "" << boxm2_data_traits<BOXM2_ALPHA>::prefix() << id << ".bin";
    vcl_string filename = ns.str();

    unsigned long bytecount=vul_file::size(filename);
    TEST("Succesful Synchronous Write ",bytecount,array_size*sizeof(float));


    baio read_status;
    asio_mgr.load_block_data<BOXM2_ALPHA>("",id,read_status);
    while(read_status.status()==BAIO_IN_PROGRESS)
        nflops++;


    char * bufferread=read_status.buffer();
    char * buffer_bkup=reinterpret_cast<char *>(farray_bkup);

    bool flag=true;
    for(unsigned i=0;i<bytecount;i++)
    {
        if(bufferread[i]!=buffer_bkup[i])
        {
            flag=false;
            vcl_cout<<"Orig val "<<(int) buffer_bkup[i]<<" Read val "<< (int) bufferread[i]<<vcl_endl;
        }
    }
    TEST("Succesful Synchronous Read", true, flag);
    delete [] buffer_bkup;
}
static void test_data()
{

    test_data_sio();
    test_data_asio();
 }



TESTMAIN(test_data);
