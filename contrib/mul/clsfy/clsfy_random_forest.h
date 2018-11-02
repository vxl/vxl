// This is mul/clsfy/clsfy_random_forest.h
#ifndef clsfy_random_forest_h_
#define clsfy_random_forest_h_
//:
// \file
// \brief Binary tree classifier
// \author Martin Roberts
#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_classifier_base.h>
#include <clsfy/clsfy_binary_tree.h>
#include <mbl/mbl_cloneable_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Forest of clsfy_binary_tree trees
// Note as it uses clsfy_binary_tree this is a 2-state (binary) classifier also
class clsfy_random_forest : public clsfy_classifier_base
{
  public:

  //: Constructor

    clsfy_random_forest();

    ~clsfy_random_forest() override;


  //: Return the classification of the given probe vector.
    unsigned classify(const vnl_vector<double> &input) const override;

    //: Provides a probability-like value that the input being in each class.
    // output(i) i<nClasses, contains the probability that the input is in class i
    void class_probabilities(std::vector<double> &outputs, const vnl_vector<double> &input) const override;

    //: This value has properties of a Log likelihood of being in class (binary classifiers only)
    // class probability = exp(logL) / (1+exp(logL))
    double log_l(const vnl_vector<double> &input) const override;

    //: The number of possible output classes.
    unsigned n_classes() const override {return 1;}

    //: The dimensionality of input vectors.
    unsigned n_dims() const override;

    //: Storage version number
    virtual short version_no() const;

    //: Name of the class
    std::string is_a() const override;

    //: Name of the class
    bool is_class(std::string const& s) const override;

    //: Create a copy on the heap and return base class pointer
    clsfy_classifier_base* clone() const override;

    //: Print class to os
    void print_summary(std::ostream& os) const override;

    //: Save class to binary file stream
    void b_write(vsl_b_ostream& bfs) const override;

    //: Load class from binary file stream
    void b_read(vsl_b_istream& bfs) override;

    unsigned ntrees() const {return trees_.size();}

    void prune();

    //Append the tres of forest2 onto this
    clsfy_random_forest& operator+=(const clsfy_random_forest& forest2);

    //: Merge the sub-forests in the input filenames into a single larger one
    friend void merge_sub_forests(const std::vector<std::string>& filenames,
                           clsfy_random_forest& large_forest);
    //: Merge the sub-forests pointed to the input vector a single larger one
    friend void merge_sub_forests(const std::vector< clsfy_random_forest*>& sub_forests,
                           clsfy_random_forest& large_forest);

    //: Merge the two input forests
    friend clsfy_random_forest operator+(const clsfy_random_forest& forest1,
                                         const clsfy_random_forest& forest2);


    //: Provides a probability-like value that the input being in each class.
    // output(i) i<nClasses, contains the probability that the input is in class i
    // Do Out of bag estimates, i.e. only use trees whose vector of indices does not include this_indexs
    virtual void class_probabilities_oob(std::vector<double> &outputs,
                                         const vnl_vector<double> &input,
                                         const std::vector<std::vector<unsigned > >& oobIndices,
                                         unsigned this_index) const;


    //: Return the classification of the given probe vector using out of bag trees only.
    //See also class_probabilities_oob
    virtual unsigned classify_oob(const vnl_vector<double> &input,
                                  const std::vector<std::vector<unsigned > >& oobIndices,
                                  unsigned this_index) const;

  private:

    //: The trees in this forest
    std::vector<mbl_cloneable_ptr<clsfy_classifier_base> > trees_;

    friend class clsfy_random_forest_builder;
};

#endif // clsfy_random_forest_h_
