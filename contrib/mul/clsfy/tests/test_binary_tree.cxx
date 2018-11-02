// This is mul/clsfy/tests/test_binary_tree.cxx
#include <iostream>
#include <string>
#include <numeric>
#include <algorithm>
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests the clsfy_binary_tree class
// \author dac
// Test construction, IO etc

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <clsfy/clsfy_binary_tree.h>
#include <clsfy/clsfy_binary_tree_builder.h>
#include <clsfy/clsfy_binary_threshold_1d_builder.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_sampler.h>
#include <mbl/mbl_data_array_wrapper.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//: Tests the clsfy_binary_tree class
void test_binary_tree()
{
    std::cout << "*******************************************\n"
             << " Testing clsfy_binary_tree_builder\n"
             << "*******************************************\n";

    constexpr double varx = 1.0;
    constexpr double vary = 1.0;
    constexpr double varz = 1.0;

    constexpr double mux = 0.5;
    constexpr double muy = 1.0;
    constexpr double muz = 2.0;

    pdf1d_gaussian pdfx(mux,varx);
    pdf1d_gaussian pdfy(muy,vary);
    pdf1d_sampler* pdfx_sampler = nullptr;
    pdf1d_sampler* pdfy_sampler = nullptr;

    pdf1d_gaussian pdfz(muz,varz);
    pdf1d_sampler* pdfz_sampler = nullptr;

    pdf1d_gaussian pdferror(0.0,0.02*0.02);
    pdf1d_sampler* pdferror_sampler = nullptr;

    pdfx_sampler = pdfx.new_sampler();

    unsigned long ixseed = 12345671;
    pdfx_sampler->reseed(ixseed);

    pdfy_sampler = pdfy.new_sampler();
    unsigned long iyseed = 7653215;
    pdfy_sampler->reseed(iyseed);

    pdfz_sampler = pdfz.new_sampler();
    unsigned long izseed = 56789123;
    pdfz_sampler->reseed(izseed);

    pdferror_sampler = pdferror.new_sampler();

    unsigned NPOINTS=500;
    std::vector<vnl_vector<double > > data(NPOINTS);

    vnl_vector<double> xerr(1);
    vnl_vector<double> yerr(1);
    std::vector<unsigned > training_outputs(NPOINTS,0);
    unsigned label=0;
    vnl_vector<double > data1d(NPOINTS);
    for (unsigned i=0; i<NPOINTS;++i)
    {
        data[i].set_size(2);
        pdfx_sampler->get_samples(xerr);
        pdfy_sampler->get_samples(yerr);
        double x=xerr[0];
        double y=yerr[0];
        data[i][0]=x;
        data[i][1]=y;
        if (x<=0.5)
        {
            if (y<0.75)
//            if (y<1000)
                label=0;
            else
                label=1;
        }
        else
        {
            if (y>1.5)
//            if (y>1000)
                label=0;
            else
                label=1;
        }
//        if (x<mux)
//            label=0;
//        else
//            label=1;
        training_outputs[i]=label;
        data1d[i]=x;
    }

    mbl_data_array_wrapper<vnl_vector<double> > training_set_inputs(data);

    clsfy_binary_tree_builder builder;
    clsfy_classifier_base* pBaseClassifier=builder.new_classifier();
    TEST("Type is binary tree",
         pBaseClassifier->is_a()==std::string("clsfy_binary_tree"), true);
    auto* pClassifier=dynamic_cast<clsfy_binary_tree*>(pBaseClassifier);
    TEST("Can cast to binary tree",pClassifier != nullptr,true);

    builder.build(*pClassifier,
                  training_set_inputs,
                  1,
                  training_outputs);

    std::vector<vnl_vector<double > > testData(NPOINTS);

    std::vector<unsigned > test_outputs(NPOINTS,0);
    constexpr double epsilon = 0.01;
    vnl_vector<double > error(1);
    unsigned tp=0;
    for (unsigned i=0; i<NPOINTS;++i)
    {
        testData[i].set_size(2);
        pdfx_sampler->get_samples(xerr);
        pdfy_sampler->get_samples(yerr);
        pdferror_sampler->get_samples(error);

        double x=xerr[0];
        double y=yerr[0];
        if (x<=0.5+error[0])
        {
            if (y<0.75-error[0])
                label=0;
            else
                label=1;
        }
        else
        {
            if (y>1.5+error[0])
                label=0;
            else
                label=1;
        }

        test_outputs[i]=label;
        testData[i][0]=x+epsilon;
        testData[i][1]=y-epsilon;
        if (label==1)
            ++tp;
    }

    unsigned tpr=0;
    unsigned tnr=0;
    unsigned fpr=0;
    unsigned fnr=0;
    for (unsigned i=0; i<NPOINTS;++i)
    {
        unsigned label=pClassifier->classify(testData[i]);
        if (test_outputs[i]==0)
        {
            if (label==0)
                ++tnr;
            else
                ++fpr;
        }
        else
        {
            if (label==1)
                ++tpr;
            else
                ++fnr;
        }
    }
    auto dtp=double (tp);
    auto dtn=double (NPOINTS-tp);
    double testTPR=double (tpr)/dtp;
    double testFPR=double (fpr)/dtn;
    double testTNR=double (tnr)/dtn;
    double testFNR=double (fnr)/dtp;

    std::cout<<"True Positive Rate " <<double(tpr)/dtp<<'\n'
            <<"False Positive Rate "<<double(fpr)/dtn<<'\n'
            <<"True Negative Rate " <<double(tnr)/dtn<<'\n'
            <<"False Negative Rate "<<double(fnr)/dtp<<std::endl;

    // simple test for binary threshold
    TEST("tpr>0.9", double (tpr)/dtp>0.9, true);
    TEST("fpr<0.1", double (fpr)/dtn<0.1, true);

    std::cout<<"======== TESTING I/O ===========\n";

    // add binary loaders
    vsl_add_to_binary_loader(clsfy_binary_threshold_1d());
    std::string test_path = "test_clsfy_binary_tree.bvl.tmp";

    vsl_b_ofstream bfs_out(test_path);
    TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
    vsl_b_write(bfs_out, *pClassifier);
    TEST("Stream OK after writing ", (!bfs_out ), false);
    bfs_out.close();

    clsfy_classifier_base* pBaseClassifierIn=builder.new_classifier();

    vsl_b_ifstream bfs_in(test_path);
    TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in ), false);
    vsl_b_read(bfs_in, *pBaseClassifierIn);
    TEST("Stream OK after reading ", (!bfs_in ), false);
    bfs_in.close();

    TEST("Type is binary tree",
         pBaseClassifierIn->is_a()==std::string("clsfy_binary_tree"), true);
    auto* pClassifierIn=dynamic_cast<clsfy_binary_tree*>(pBaseClassifierIn);
    TEST("Can cast to binary tree",pClassifierIn != nullptr,true);

    {
        unsigned tp=std::count(test_outputs.begin(),test_outputs.end(),1U);
        unsigned tpr=0;
        unsigned tnr=0;
        unsigned fpr=0;
        unsigned fnr=0;
        for (unsigned i=0; i<NPOINTS;++i)
        {
            unsigned label=pClassifierIn->classify(testData[i]);
            if (test_outputs[i]==0)
            {
                if (label==0)
                    ++tnr;
                else
                    ++fpr;
            }
            else
            {
                if (label==1)
                    ++tpr;
                else
                    ++fnr;
            }
        }
        auto dtp=double (tp);
        auto dtn=double (NPOINTS-tp);
        std::cout<<"True Positive Rate " <<double(tpr)/dtp<<'\n'
                <<"False Positive Rate "<<double(fpr)/dtn<<'\n'
                <<"True Negative Rate " <<double(tnr)/dtn<<'\n'
                <<"False Negative Rate "<<double(fnr)/dtp<<std::endl;

        // simple test for binary threshold
        double tpr_=double (tpr)/dtp;
        double fpr_=double (fpr)/dtn;
        TEST("tpr>0.7", tpr_>0.9, true);
        TEST("fpr<0.3", fpr_<0.1, true);
        TEST("Same TPR as pre-IO ",std::fabs(testTPR- tpr_)<1.0E-6,true);
        TEST("Same FPR as pre-IO ",std::fabs(testFPR- fpr_)<1.0E-6,true);
    }
    std::cout<<"======== TESTING Assignment ===========\n";

    {
        clsfy_binary_tree treeCopy= *pClassifierIn;

        unsigned tp=std::count(test_outputs.begin(),test_outputs.end(),1U);
        unsigned tpr=0;
        unsigned tnr=0;
        unsigned fpr=0;
        unsigned fnr=0;
        for (unsigned i=0; i<NPOINTS;++i)
        {
            unsigned label=treeCopy.classify(testData[i]);
            if (test_outputs[i]==0)
            {
                if (label==0)
                    ++tnr;
                else
                    ++fpr;
            }
            else
            {
                if (label==1)
                    ++tpr;
                else
                    ++fnr;
            }
        }
        auto dtp=double (tp);
        auto dtn=double (NPOINTS-tp);
        std::cout<<"True Positive Rate " <<double(tpr)/dtp<<'\n'
                <<"False Positive Rate "<<double(fpr)/dtn<<'\n'
                <<"True Negative Rate " <<double(tnr)/dtn<<'\n'
                <<"False Negative Rate "<<double(fnr)/dtp<<std::endl;

        // simple test for binary threshold
        double tpr_=double (tpr)/dtp;
        double fpr_=double (fpr)/dtn;
        TEST("tpr>0.7", tpr_>0.9, true);
        TEST("fpr<0.3", fpr_<0.1, true);
        TEST("Same TPR as pre-IO ",std::fabs(testTPR- tpr_)<1.0E-6,true);
        TEST("Same FPR as pre-IO ",std::fabs(testFPR- fpr_)<1.0E-6,true);
    }

    std::cout<<"=========swap pos and neg samples round===========\n";

    for (unsigned i=0;i<NPOINTS;++i)
    {
        unsigned label=training_outputs[i];
        if (label==0)
            training_outputs[i]=1;
        else
            training_outputs[i]=0;
        label=test_outputs[i];
        if (label==0)
            test_outputs[i]=1;
        else
            test_outputs[i]=0;
    }
    // Train again with +ve and -ve data swapped round + see if get same error
    clsfy_classifier_base* pBaseClassifier2=builder.new_classifier();
    auto* pClassifier2=dynamic_cast<clsfy_binary_tree*>(pBaseClassifier2);

    builder.build(*pClassifier2,
                  training_set_inputs,
                  1,
                  training_outputs);

    {
        constexpr int NPOINTS = 500;
        unsigned tp=std::count(test_outputs.begin(),test_outputs.end(),1U);
        unsigned tpr=0;
        unsigned tnr=0;
        unsigned fpr=0;
        unsigned fnr=0;
        for (int i=0; i<NPOINTS; ++i)
        {
            unsigned label=pClassifier2->classify(testData[i]);
            if (test_outputs[i]==0)
            {
                if (label==0)
                    ++tnr;
                else
                    ++fpr;
            }
            else
            {
                if (label==1)
                    ++tpr;
                else
                    ++fnr;
            }
        }
        auto dtp=double (tp);
        auto dtn=double (NPOINTS-tp);
        std::cout<<"True Positive Rate " <<double(tpr)/dtp<<'\n'
                <<"False Positive Rate "<<double(fpr)/dtn<<'\n'
                <<"True Negative Rate " <<double(tnr)/dtn<<'\n'
                <<"False Negative Rate "<<double(fnr)/dtp<<std::endl;

        TEST("tpr>0.9", double (tpr)/dtp>0.9, true);
        TEST("fpr<0.1", double (fpr)/dtn<0.1, true);

        double tpr_=double (tpr)/dtp;
        double fpr_=double (fpr)/dtn;
        double tnr_=double (tnr)/dtn;
        double fnr_=double (fnr)/dtp;

        TEST("Same TPR as initial TNR",std::fabs(testTNR- tpr_)<1.0E-6,true);
        TEST("Same FPR as initial FNR",std::fabs(testFNR- fpr_)<1.0E-6,true);
        TEST("Same TNR as initial TPR",std::fabs(testTPR- tnr_)<1.0E-6,true);
        TEST("Same FNR as initial FPR",std::fabs(testFPR- fnr_)<1.0E-6,true);
    }

    {
        std::cout<<"TESTING Circle Data..."<<std::endl;
        constexpr int NPOINTS = 2000;
        std::vector<vnl_vector<double > > data(NPOINTS);

        pdf1d_gaussian pdferror2(0.0,0.05*0.05);
        pdf1d_sampler* pdferror_sampler2 = pdferror2.new_sampler();;

        vnl_vector<double> xerr(1);
        vnl_vector<double> yerr(1);
        vnl_vector<double> zerr(1);
        std::vector<unsigned > training_outputs(NPOINTS,0);
        unsigned label=0;
        vnl_vector<double > data1d(NPOINTS);
        for (int i=0; i<NPOINTS;++i)
        {
            data[i].set_size(3);
            pdfx_sampler->get_samples(xerr);
            pdfy_sampler->get_samples(yerr);
            pdfz_sampler->get_samples(zerr);

            pdferror_sampler->get_samples(error);

            double x=xerr[0];
            double y=yerr[0];
            double z=zerr[0];

            data[i][0]=x;
            data[i][1]=y;
            data[i][2]=z;

            double rx=x-mux;
            double ry=y-muy;
            double rz=z-muz;

            double r=std::sqrt(rx*rx+ry*ry+rz*rz);

            double err= pdferror_sampler2->sample();
            if (r+err<std::sqrt(3.0))
                label=0;
            else
                label=1;

            training_outputs[i]=label;
            data1d[i]=x;
        }

        mbl_data_array_wrapper<vnl_vector<double> > training_set_inputs(data);

        clsfy_binary_tree_builder builder;
        builder.set_min_node_size(-1);
        clsfy_classifier_base* pBaseClassifier=builder.new_classifier();
        TEST("Type is binary tree",
             pBaseClassifier->is_a()==std::string("clsfy_binary_tree"), true);
        auto* pClassifier=dynamic_cast<clsfy_binary_tree*>(pBaseClassifier);
        TEST("Can cast to binary tree",pClassifier != nullptr,true);

        builder.build(*pClassifier,
                      training_set_inputs,
                      1,
                      training_outputs);

        std::vector<vnl_vector<double > > testData(NPOINTS);

        std::vector<unsigned > test_outputs(NPOINTS,0);
        vnl_vector<double > error(1);
        unsigned tp=0;
        for (int i=0; i<NPOINTS;++i)
        {
            testData[i].set_size(3);
            pdfx_sampler->get_samples(xerr);
            pdfy_sampler->get_samples(yerr);
            pdfz_sampler->get_samples(zerr);

            pdferror_sampler->get_samples(error);

            double x=xerr[0];
            double y=yerr[0];
            double z=zerr[0];

            testData[i][0]=x;
            testData[i][1]=y;
            testData[i][2]=z;

            double rx=x-mux;
            double ry=y-muy;
            double rz=z-muz;
            double r=std::sqrt(rx*rx+ry*ry+rz*rz);

            double err= pdferror_sampler2->sample();
            if (r+err<std::sqrt(3.0))
                label=0;
            else
                label=1;

            test_outputs[i]=label;

            if (label==1)
                ++tp;
        }

        std::cout<<"True positives in test = "<<tp<<std::endl;
        unsigned tpr=0;
        unsigned tnr=0;
        unsigned fpr=0;
        unsigned fnr=0;
        for (int i=0; i<NPOINTS; ++i)
        {
            unsigned label=pClassifier->classify(testData[i]);
            if (test_outputs[i]==0)
            {
                if (label==0)
                    ++tnr;
                else
                    ++fpr;
            }
            else
            {
                if (label==1)
                    ++tpr;
                else
                    ++fnr;
            }
        }
        auto dtp=double (tp);
        auto dtn=double (NPOINTS-tp);
        double testTPR=double (tpr)/dtp;
        double testFPR=double (fpr)/dtn;
        double testTNR=double (tnr)/dtn;
        double testFNR=double (fnr)/dtp;

        std::cout<<"True Positive Rate " <<testTPR<<'\n'
                <<"False Positive Rate "<<testFPR<<'\n'
                <<"True Negative Rate " <<testTNR<<'\n'
                <<"False Negative Rate "<<testFNR<<std::endl;

        // simple test for binary threshold
        TEST("tpr>0.9", testTPR>0.9, true);
        TEST("fpr<0.15",testFPR<0.15,true);
    }

#if 0 // 85 lines commented out
    clsfy_classifier_1d* b_thresh_clsfr2=b_thresh_builder.new_classifier();
    double error2= b_thresh_builder.build(*b_thresh_clsfr2,
                                          pos_samples, pos_wts,
                                          neg_samples, neg_wts
                                         );

    b_thresh_clsfr2->print_summary(std::cout);

    std::cout<<"error2= "<<error2<<std::endl;

    TEST_NEAR("error1 ~= error2", error1, error2, 0.001);

    tp=0;
    fp=0;
    for (int i=0; i<n_neg; ++i)
        if ( b_thresh_clsfr2->classify( neg_samples_test[i] ) == 1 ) tp++;

    for (int i=0; i<n_pos; ++i)
        if ( b_thresh_clsfr2->classify( pos_samples_test[i] ) == 1 ) fp++;

    delete b_thresh_clsfr2;

    std::cout<<"Applied to test set (with +ve and -ve other way round:\n";
    tpr=(tp*1.0)/n_neg, fpr= (fp*1.0)/n_pos;
    std::cout<<"True positives= "<<tpr<<'\n'
            <<"False positives= "<<fpr<<std::endl;

    te= ((n_neg-tp+fp)*1.0)/(n_pos+n_neg);
    std::cout<<"te= "<<te<<std::endl;

    // simple test for binary threshold
    TEST( "tpr>0.7", tpr>0.7, true );
    TEST( "fpr<0.3", fpr<0.3, true );

    std::cout << "***********************************\n"
             << " Testing clsfy_binary_threshold_1d\n"
             << "***********************************\n";

    // Test various parameter settings
    vnl_vector<double> p(2);
    // i.e. x=>5
    p[0]=1;
    p[1]=5;
    b_thresh_clsfr->set_params(p);
    TEST( "10 > 5", b_thresh_clsfr->classify(10), 1 );
    TEST( "2 < 5", b_thresh_clsfr->classify(2), 0 );

    // i.e. x=<5
    p[0]=-1;
    p[1]=-5;
    b_thresh_clsfr->set_params(p);
    TEST( "10 > 5", b_thresh_clsfr->classify(10), 0 );
    TEST( "2 < 5", b_thresh_clsfr->classify(2), 1 );

    // Test loading clsfy_binary_threshold_1d by base class pointer

    std::cout<<"======== TESTING I/O ===========\n";

    // add binary loaders
    vsl_add_to_binary_loader(clsfy_binary_threshold_1d());
    std::string test_path = "test_clsfy_simple_adaboost.bvl.tmp";

    vsl_b_ofstream bfs_out(test_path);
    TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
    vsl_b_write(bfs_out, *b_thresh_clsfr);
    bfs_out.close();

    clsfy_classifier_1d* classifier_in = b_thresh_builder.new_classifier();;

    vsl_b_ifstream bfs_in(test_path);
    TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in ), false);
    vsl_b_read(bfs_in, *classifier_in);

    bfs_in.close();
#if !LEAVE_FILES_BEHIND
    vpl_unlink(test_path.c_str());
#endif

    std::cout<<"Saved :\n"
            << *b_thresh_clsfr << '\n'
            <<"Loaded:\n"
            << classifier_in << std::endl;

    TEST("saved classifier = loaded classifier",
         b_thresh_clsfr ->params(), classifier_in->params());
#endif // 85 lines commented out

    std::cout<<"Deleting classifiers"<<std::endl;
    delete pClassifier;
    std::cout<<"Deleting the second classifier"<<std::endl;
    delete pClassifier2;
    std::cout<<"have deleted classifiers"<<std::endl;

    delete pClassifierIn;
    vsl_delete_all_loaders();
}

TESTMAIN(test_binary_tree);
