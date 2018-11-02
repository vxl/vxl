// Testing mipa_multiple_normalisers

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_loader.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_ref.h>
#include <mbl/mbl_read_props.h>
#include <mipa/mipa_l2norm_vector_normaliser.h>
#include <mipa/mipa_identity_normaliser.h>
#include <mipa/mipa_block_normaliser.h>
#include <mipa/mipa_ms_block_normaliser.h>
#include <mipa/mipa_add_all_loaders.h>
#include <mipa/mipa_orientation_histogram.h>
#include <mipa/mipa_sample_histo_boxes.h>

//=======================================================================

// The main control program
static void test_vector_normalisers()
{
    std::cout << '\n'
             << "*********************************\n"
             << " Testing mipa_vector_normalisers\n"
             << "*********************************" << std::endl;

    mipa_add_all_loaders();
    vnl_random rng(123456);
    const int n = 16*16*20;
    vnl_vector<double> data(n);
    for (int i = 3; i < n; i++)
        data(i) = rng.normal();
    data(0) = data(1) = data(2) = 0;
    data *= 100.0;
    const vnl_vector<double> &x=data;

    mipa_l2norm_vector_normaliser l2norm_normaliser;

    std::cout << "===============Testing L2 Norm normalisation============\n"

             << "rms before normalisation" <<data.rms()<<std::endl;
    vnl_vector<double> x1(x);

    l2norm_normaliser.normalise(x1);
    double rms=x1.rms();
    rms *= std::sqrt(double(data.size()));
    std::cout << "rms after normalisation and root n correction" <<rms<<std::endl;
    TEST_NEAR("RMS is near unity", rms, 1.0, 1e-2);

    std::cout << "===============Testing Block Normaliser stream config============" <<std::endl;
    std::string strConfig;
    strConfig += "mipa_block_normaliser {\n";
    strConfig += "normaliser: mipa_l2norm_vector_normaliser\n";
    strConfig +=  "ni: 16\n";
    strConfig +=  "nj: 16\n";
    strConfig +=  "nA: 18\n";
    strConfig +=  "block_size: 4\n";
    strConfig += "}\n";

    std::istringstream ss(strConfig);
    mbl_read_props_type dummy_extra_props;
    std::unique_ptr<mipa_vector_normaliser> norm = mipa_vector_normaliser::new_normaliser_from_stream(ss, dummy_extra_props);
    TEST("Block normaliser created",norm->is_a()=="mipa_block_normaliser",true);

    mipa_vector_normaliser* pNorm = norm->clone();
    auto* pBlockNormaliser=dynamic_cast<mipa_block_normaliser* >(pNorm);
    TEST("Clone is dynamic castable to Block normaliser",pBlockNormaliser != nullptr,true);
    if (pBlockNormaliser)
    {
        TEST("Contains an L2 normaliser",pBlockNormaliser->normaliser().is_a()=="mipa_l2norm_vector_normaliser",true);
        TEST("Configured ni",pBlockNormaliser->ni_region()==16,true);
        TEST("Configured nj",pBlockNormaliser->nj_region()==16,true);
        TEST("Configured nA",pBlockNormaliser->nA()==18,true);
        TEST("Configured nA",pBlockNormaliser->nc_per_block()==4,true);

        std::cout<<"Testing Block Normaliser behaviour"
                <<"Test trivial behaviour if only one block"<<std::endl;
        vnl_vector<double > x2(data);

        pBlockNormaliser->set_nc_per_block(16); //So only one block - the entire region
        pBlockNormaliser->normalise(x2);
        vnl_vector<double > diff=x2-x2;
        TEST_NEAR("Test trivial block normaliser same as non-block",diff.inf_norm(),0.0,1.0E-8);

        unsigned nA_bins=pBlockNormaliser->nA(); //Num angle bins
        unsigned nc=8; //Num pixels per cell
#if 0
        const int npi=16*nc; //Total number of nominal pixels in region
        const int npj=16*nc;
        const int npixels=npi*npj;
#endif // 0
        unsigned ncPerBlock=4; //Num cells per block (in each dimension)
        pBlockNormaliser->set_nc_per_block(ncPerBlock);
        unsigned npixPerBlock=ncPerBlock*nc;
        unsigned sni = 2+npixPerBlock; //add on borders for gradients
        unsigned snj= 2+npixPerBlock;

        const unsigned nData=sni*snj; //Block worth  of pixels +border

        vil_image_view<float> srcBlock(sni,snj);
        vnl_random rng(123456);
        vnl_vector<float> data2(nData);
        for (unsigned int i = 0; i < nData; ++i)
            data2(i) = (float)rng.normal();
        data2 *= 5.0f;
        //Copy in some background noise
        std::copy(data2.begin(),data2.end(),srcBlock.top_left_ptr());

        //Fill in one block
        for (unsigned i=0;i<sni;++i)
        {
            for (unsigned j=0;j<snj;++j)
            {
                if (i==j) //diagonal
                    srcBlock(i,j) = 500.0f+50.0f*float(rng.normal());
                if (i%8==2) //vertical lines
                    srcBlock(i,j) = 200.0f+20.0f*float(rng.normal());
                if (j%4==3) //horiz lines
                    srcBlock(i,j) = 50.0f+10.0f*float(rng.normal());
            }
        }

        bool full360=true;
        vil_image_view<float> histo_im;

        mipa_orientation_histogram(srcBlock,histo_im,nA_bins,nc,full360);
        std::cout<<"Have sampled into hist images of size ni=" <<histo_im.ni()<<'\t'
                <<histo_im.nj()<<'\t'<<histo_im.nplanes()<<std::endl;

        //Now reproduce 4x4 tiling of histo_im (i.e. lots of equivalent normalisation blocks)
        //Fill in next block
        unsigned nblocks=16/ncPerBlock;
        vil_image_view<float> histo_im4x4(16,16,1,histo_im.nplanes());
        for (unsigned iblock=0;iblock<nblocks;++iblock)
        {
            unsigned ioffset=ncPerBlock*iblock;
            for (unsigned jblock=0;jblock<nblocks;++jblock)
            {
                unsigned joffset=ncPerBlock*jblock;
                float k=1.0f;
                if (iblock>0 && jblock>0)
                    k=32.0f*iblock+2.0f*jblock; //scale-up factor
                for (unsigned i=0;i<ncPerBlock;++i)
                {
                    for (unsigned j=0;j<ncPerBlock;++j)
                    {
                        for (unsigned iplane=0; iplane<nA_bins;++iplane)
                        {
                            //Apply same scale up to all elements of block
                            //(so normalisation should remove it)
                            histo_im4x4(ioffset+i,joffset+j,iplane)=k*histo_im(i,j,iplane);
                        }
                    }
                }
            }
        }

        vnl_vector<double> vbn;
        unsigned ni=nblocks*ncPerBlock/2;
        unsigned nj=ni;
        mipa_sample_histo_boxes_3L(histo_im4x4,0,0,vbn,ni,nj);

        unsigned nLevel1=4*nA_bins*ni*nj;

        vnl_vector<double > vL1(nLevel1);
        std::copy(vbn.begin(),vbn.begin()+nLevel1,vL1.begin());

        std::cout<<"Before normalisation..."<<std::endl;
        std::copy(vL1.begin(),vL1.begin()+8,std::ostream_iterator<double>(std::cout,"\t"));


        pBlockNormaliser->normalise(vL1);
        std::cout<<"After normalisation..."<<std::endl;
        std::copy(vL1.begin(),vL1.begin()+8,std::ostream_iterator<double>(std::cout,"\t"));
        double rms=vL1.rms();
        double min=*std::min_element(vL1.begin(),vL1.end());
        double max=*std::max_element(vL1.begin(),vL1.end());
        std::cout<<"Post Norm rms= "<<rms<<" Min= "<<min<<" Max= "<<max<<std::endl;

        // Copy back from renormalised sample to image
        mipa_sample_histo_boxes_3L_inv(histo_im4x4,0,0,vL1,ni,nj);

        // Verify that each block is an identical tiling after normalisation
        for (unsigned iblock=1;iblock<nblocks;++iblock)
        {
            unsigned ioffset=ncPerBlock*iblock;
            for (unsigned jblock=1;jblock<nblocks;++jblock)
            {
                unsigned joffset=ncPerBlock*jblock;
                double maxDiff=-1.0E32;
                for (unsigned i=0;i<ncPerBlock;++i)
                {
                    for (unsigned j=0;j<ncPerBlock;++j)
                    {
                        for (unsigned iplane=0; iplane<nA_bins;++iplane)
                        {
                            //Apply same scale up to all elements of block
                            //(so normalisation should remove it)
                            double diff=histo_im4x4(ioffset+i,joffset+j,iplane)-histo_im4x4(i,j,iplane);
                            diff=std::fabs(diff);
                            if (diff>maxDiff)
                                maxDiff=diff;
                        }
                    }
                }
                TEST_NEAR("Block is normalised as per Top Left Block",maxDiff,0.0,1.0E-4);
            }
        }


        //-------------------------------------------------------------------------
        //
        // Test multi-scale block normaliser
        constexpr unsigned nLevels = 2;
        const bool include_overall_histogram=true;

        mipa_ms_block_normaliser msBlockNormaliser(*pBlockNormaliser,nLevels,include_overall_histogram);
        msBlockNormaliser.normalise(vbn);
        vnl_vector_ref<double > vms1(nLevel1,vbn.data_block());
        vnl_vector<double > vdiff=vms1-vL1;
        double diffNorm=vdiff.inf_norm();
        TEST_NEAR("Testing multi-scale normaliser gives same Level1 result as single scale",diffNorm,0.0,1.0E-6);
        unsigned nLevel2=nLevel1/4;
        vnl_vector_ref<double> vms2(nLevel2,vbn.data_block()+nLevel1);
        double rms2=vms2.rms();

        rms2 *= std::sqrt(double(nLevel2))/double (ncPerBlock);
        std::cout << "Level 2 rms after normalisation and root n correction " <<rms2<<std::endl;
        TEST_NEAR("Level 2 RMS is near unity", rms2, 1.0, 1e-4);
        double mu1=vms1.mean();
        double mu2=vms2.mean()/(std::sqrt(double (ncPerBlock)));
        std::cout<<"Renorm level 1 mean= "<<mu1<<"Renorm n compensated level 2 mean= "<<mu2<<std::endl;

        vnl_vector_ref<double> vms3(nA_bins,vbn.data_block()+nLevel1+nLevel2);

        double sumSq=vms3.squared_magnitude();
        TEST_NEAR("Overall Histogram mean square is near unity", sumSq, 1.0, 1e-4);
    }//Castable to block normaliser

    std::cout << "===============Testing IO============\n"

             << "===============Testing MS Block Normaliser stream config============" <<std::endl;

    mipa_ms_block_normaliser* pMSBlockNormaliser=nullptr;
    {
        std::string strConfig;
        strConfig += "mipa_ms_block_normaliser {\n";
        strConfig += "normaliser: mipa_l2norm_vector_normaliser\n";
        strConfig +=  "ni: 16\n";
        strConfig +=  "nj: 16\n";
        strConfig +=  "nA: 18\n";
        strConfig +=  "block_size: 4\n";
        strConfig +=  "nscales: 2\n";
        strConfig += "include_overall_histogram: true\n";
        strConfig += "}\n";

        std::istringstream ss(strConfig);
        mbl_read_props_type dummy_extra_props;
        std::unique_ptr<mipa_vector_normaliser> msnorm = mipa_vector_normaliser::new_normaliser_from_stream(ss, dummy_extra_props);
        TEST("Block normaliser created",msnorm->is_a()=="mipa_ms_block_normaliser",true);

        mipa_vector_normaliser* pNorm = msnorm->clone();
        pMSBlockNormaliser=dynamic_cast<mipa_ms_block_normaliser* >(pNorm);
        TEST("Clone is dynamic castable to Block normaliser",pMSBlockNormaliser != nullptr,true);
    }
    if (pMSBlockNormaliser)
    {
        TEST("Contains an L2 normaliser",pMSBlockNormaliser->normaliser().is_a()=="mipa_l2norm_vector_normaliser",true);
        TEST("Configured ni",pMSBlockNormaliser->ni_region()==16,true);
        TEST("Configured nj",pMSBlockNormaliser->nj_region()==16,true);
        TEST("Configured nA",pMSBlockNormaliser->nA()==18,true);
        TEST("Configured nA",pMSBlockNormaliser->nc_per_block()==4,true);
        TEST("Configured nscales",pMSBlockNormaliser->nscales()==2,true);
        TEST("Configured include overall histogram",pMSBlockNormaliser->include_overall_histogram(),true);
    }

//---------------------------------------------------------------------------------
    if (pMSBlockNormaliser && pBlockNormaliser)
    {
        const std::string path = "test_vector_normalisers.bvl.tmp";
        vsl_b_ofstream bfs_out(path);
        TEST (("Created " + path +" for writing").c_str(),(!bfs_out), false);
        pBlockNormaliser->set_nc_per_block(4); //Reset to multi blocks
        vsl_b_write(bfs_out, pNorm);
        mipa_vector_normaliser* pNorm2=pMSBlockNormaliser;
        vsl_b_write(bfs_out, pNorm2);
        bfs_out.close();
        delete pNorm;
        delete pNorm2;
        pNorm2=pNorm=nullptr;

        mipa_vector_normaliser *p1=nullptr;
        mipa_vector_normaliser *p2=nullptr;
        vsl_b_ifstream bfs_in(path);
        vsl_b_read(bfs_in, p1);
        vsl_b_read(bfs_in, p2);
        TEST ("Finished reading file successfully", (!bfs_in), false);
        bfs_in.close();

        TEST("Loaded normaliser (binary)",p1->is_a() == "mipa_block_normaliser",true);
        TEST("Loaded normaliser (binary)",p2->is_a() == "mipa_ms_block_normaliser",true);

        pBlockNormaliser=dynamic_cast<mipa_block_normaliser* >(p1);
        TEST("Reloaded normaliser is dynamic castable to Block normaliser",pBlockNormaliser != nullptr,true);
        if (pBlockNormaliser)
        {
            TEST("Reloaded ni",pBlockNormaliser->ni_region()==16,true);
            TEST("Reloaded nj",pBlockNormaliser->nj_region()==16,true);
            TEST("Reloaded nA",pBlockNormaliser->nA()==18,true);
            TEST("Reloaded nA",pBlockNormaliser->nc_per_block()==4,true);
        }

        pMSBlockNormaliser=dynamic_cast<mipa_ms_block_normaliser* >(p2);
        TEST("Reloaded normaliser is dynamic castable to MS Block normaliser",pMSBlockNormaliser != nullptr,true);
        if (pMSBlockNormaliser)
        {
            TEST("Reloaded ni",pMSBlockNormaliser->ni_region()==16,true);
            TEST("Reloaded nj",pMSBlockNormaliser->nj_region()==16,true);
            TEST("Reloaded nA",pMSBlockNormaliser->nA()==18,true);
            TEST("Reloaded nA",pMSBlockNormaliser->nc_per_block()==4,true);
            TEST("Reloaded nA",pMSBlockNormaliser->nscales()==2,true);
            TEST("Reloaded include overall histogram",pMSBlockNormaliser->include_overall_histogram(),true);
        }

        delete p1;delete p2;
    }

    std::cout<<std::endl;
    vsl_delete_all_loaders();
}

TESTMAIN(test_vector_normalisers);
