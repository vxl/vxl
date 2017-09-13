/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_octree_dual_h
#define rgtl_octree_dual_h

//:
// \file
// \brief Octree dual cell visitation tools.
// \author Brad King
// \date March 2007

#include "rgtl_config.h"
#include "rgtl_static_assert.h"
#include "rgtl_meta.h"

//: Octree dual cell visitor generation templates.
//
// Visiting an octree dual structure is very tricky especially in high
// dimensions.  These templates automatically generate the calls
// necessary in any number of dimensions.
namespace rgtl
{
  // rgtl implementation details
  namespace detail
  {
    // rgtl::octree_dual implementation details
    namespace octree_dual
    {
      // generate_calls implementation.
      template <unsigned int d>
      struct gen_calls
      {
        // We generate k, c, and i simultaneously for each possible
        // combination.  This instantiation generates the (d-1)th bit
        // of c.
        template <unsigned int k, unsigned int c, unsigned int i>
        struct apply
        {
          template <typename F>
          static void invoke(typename F::arg1_type arg1, F* = 0)
            {
            // Generate the combination with this bit of c set to 0
            // which has no corresponding bit of i.
            meta::call1<typename gen_calls<d-1>::template apply<k, c, i>, F>::invoke(arg1);

            // Generate the combination with this bit of c set to 1
            // and the corresponding bit of i set to 0.
            meta::call1<typename gen_calls<d-1>::template apply<k+1, (1<<(d-1))|c, (0<<k)|i>, F>::invoke(arg1);

            // Generate the combination with this bit of c set to 1
            // and the corresponding bit of i set to 1.
            meta::call1<typename gen_calls<d-1>::template apply<k+1, (1<<(d-1))|c, (1<<k)|i>, F>::invoke(arg1);
            }
        };
      };
      template <>
      struct gen_calls<0>
      {
        // We terminate recursion when all bits of c have been
        // generated for a particular combination.
        template <unsigned int k, unsigned int c, unsigned int i>
        struct apply
        {
          template <typename F>
          static void invoke(typename F::arg1_type arg1, F* = 0)
            {
            // We now have a complete axis combination and call index.
            // Invoke the given function.
            F::template apply<k, c, i>::invoke(arg1);
            }
        private:
          RGTL_STATIC_ASSERT(meta::bits_c<c>::value == k && i < (1<<k));
        };
      };

      // generate_args implementation.  Note that the bits of c and i
      // are consumed from least significant bit to most significant
      // bit even though index generation occurs from most significant
      // bit to least significant bit.  This is done to avoid passing
      // the current bit index for each variable.  Since all
      // combinations of the bits of c and i are enumerated anyway
      // this does not matter.
      template <unsigned int D>
      struct gen_args
      {
        // Implement case for the m bit 1.
        template <unsigned int m, unsigned int c, unsigned int i,
                  unsigned int j, unsigned int inarg, unsigned int child,
                  typename F>
        struct impl_case_m1
        {
          typedef typename F::arg1_type arg1_type;
          typedef typename F::arg2_type arg2_type;
          static void invoke(arg1_type arg1, arg2_type arg2)
            {
            // Generate a bit for j.  It goes into the input argument index
            // and its inverse goes into the child index.
            meta::call2<typename gen_args<D-1>::template apply<m, c, i, (j<<1)|0, (inarg<<1)|0, (child<<1)|1>, F>::invoke(arg1, arg2);
            meta::call2<typename gen_args<D-1>::template apply<m, c, i, (j<<1)|1, (inarg<<1)|1, (child<<1)|0>, F>::invoke(arg1, arg2);
            }
        };

        // Implement case for the m bit 0 and the c bit 1.
        template <unsigned int m, unsigned int c, unsigned int i,
                  unsigned int j, unsigned int inarg, unsigned int child,
                  typename F>
        struct impl_case_m0c1
        {
          typedef typename F::arg1_type arg1_type;
          typedef typename F::arg2_type arg2_type;
          static void invoke(arg1_type arg1, arg2_type arg2)
            {
            // Take a bit from i.  It goes into the child index.
            meta::call2<typename gen_args<D-1>::template apply<m, (c>>1), (i>>1), j, inarg, (child<<1)|(i&1)>, F>::invoke(arg1, arg2);
            }
        };

        // Implement case for the m bit 0 and the c bit 0.
        template <unsigned int m, unsigned int c, unsigned int i,
                  unsigned int j, unsigned int inarg, unsigned int child,
                  typename F>
        struct impl_case_m0c0
        {
          typedef typename F::arg1_type arg1_type;
          typedef typename F::arg2_type arg2_type;
          static void invoke(arg1_type arg1, arg2_type arg2)
            {
            // Generate a bit for j.  It goes into the child index.
            meta::call2<typename gen_args<D-1>::template apply<m, (c>>1), i, (j<<1)|0, inarg, (child<<1)|0>, F>::invoke(arg1, arg2);
            meta::call2<typename gen_args<D-1>::template apply<m, (c>>1), i, (j<<1)|1, inarg, (child<<1)|1>, F>::invoke(arg1, arg2);
            }
        };

        // Want to generate the output index j, input index inarg, and
        // child index "child" simultaneously.  We are given m, c, and
        // i.  This instantiation strips one bit from m and strips or
        // generates up to one bit of c, i, j, inarg, and child.
        template <unsigned int m, unsigned int c, unsigned int i, unsigned int j,
                  unsigned int inarg, unsigned int child>
        struct apply
        {
          template <typename F>
          static void invoke(typename F::arg1_type arg1,
                             typename F::arg2_type arg2, F* = 0)
            {
            // Name the implementation of each case.
            typedef impl_case_m1<m, c, i, j, inarg, child, F> case_m1;
            typedef impl_case_m0c1<m, c, i, j, inarg, child, F> case_m0c1;
            typedef impl_case_m0c0<m, c, i, j, inarg, child, F> case_m0c0;

            // Select the case corresponding to the bit values of m and c.
            typedef typename meta::if_<((m>>(D-1))&1), meta::identity<case_m1>,
              meta::if_<(c&1), case_m0c1, case_m0c0> >::type select_type;
            typedef typename select_type::type case_type;

            // Handle the selected case.
            case_type::invoke(arg1, arg2);
            }
        };
      private:
        RGTL_STATIC_ASSERT(D>0);
      };
      template <>
      struct gen_args<0>
      {
        // We terminate recursion when all bits of m have been
        // handled.
        template <unsigned int m, unsigned int c, unsigned int i, unsigned int j,
                  unsigned int inarg, unsigned int child>
        struct apply
        {
          template <typename F>
          static void invoke(typename F::arg1_type arg1,
                             typename F::arg2_type arg2, F* = 0)
            {
            // We now have a complete output argument index "j",
            // input argument index "inarg" and child index "child".
            // Invoke the given function.
            F::template apply<j, inarg, child>::invoke(arg1, arg2);
            }
        private:
          // All bits of c and i should have been consumed.
          RGTL_STATIC_ASSERT(c==0 && i==0);
        };
      };

      template <unsigned int D> struct gen_m;
      template <unsigned int D, unsigned int m, unsigned int c> struct gen_m_case_m1;
      template <unsigned int D, unsigned int m, unsigned int c> struct gen_m_case_m0c1;
      template <unsigned int D, unsigned int m, unsigned int c> struct gen_m_case_m0c0;

      // generate_m implementation.
      template <unsigned int D>
      struct gen_m
      {
        // Want to generate the output axis mask for the current axis
        // combination and input axis mask.  We are given m, and c.
        // The value n starts at D.  This instantiation strips one bit
        // from the input mask m and stores one bit in the output
        // mask.
        template <unsigned int m, unsigned int c>
        struct apply
        {
        private:
          // Name the implementation of each case.
          typedef gen_m_case_m1<D,m,c> case_m1;
          typedef gen_m_case_m0c1<D,m,c> case_m0c1;
          typedef gen_m_case_m0c0<D,m,c> case_m0c0;

          // Select the case corresponding to the bit values of m and c.
          typedef typename meta::if_<((m>>(D-1))&1), meta::identity< case_m1 >,
                                     meta::if_<(c&1), case_m0c1, case_m0c0 > >::type select_type;
          typedef typename select_type::type case_type;
        public:
          // Use the value from the selected case.
          RGTL_STATIC_CONST(unsigned int, value = case_type::value);
        };
      private:
        RGTL_STATIC_ASSERT(D>0);
      };
      template <>
      struct gen_m<0>
      {
        // We terminate recursion when all bits of m have been
        // handled.
        template <unsigned int m, unsigned int c>
        struct apply
        {
        private:
          RGTL_STATIC_ASSERT(c==0);
        public:
          // The output mask starts at zero and has one bit stored by
          // each above recursion level above.
          RGTL_STATIC_CONST(unsigned int, value = 0);
        };
      };

      // Implement gen_m case for the m bit 1.
      template <unsigned int D, unsigned int m, unsigned int c>
      struct gen_m_case_m1
      {
        // Generate a bit for j.
        typedef typename gen_m<D-1>::template apply<m, (c>>0)> apply_type;
        RGTL_STATIC_CONST(unsigned int, value = (1<<(D-1))|apply_type::value);
      };

      // Implement gen_m case for the m bit 0 and the c bit 1.
      template <unsigned int D, unsigned int m, unsigned int c>
      struct gen_m_case_m0c1
      {
        // Take a bit from i.
        typedef typename gen_m<D-1>::template apply<m, (c>>1)> apply_type;
        RGTL_STATIC_CONST(unsigned int, value = (0<<(D-1))|apply_type::value);
      };

      // Implement gen_m case for the m bit 0 and the c bit 0.
      template <unsigned int D, unsigned int m, unsigned int c>
      struct gen_m_case_m0c0
      {
        // Generate a bit for j.
        typedef typename gen_m<D-1>::template apply<m, (c>>1)> apply_type;
        RGTL_STATIC_CONST(unsigned int, value = (1<<(D-1))|apply_type::value);
      };

      // Enumerate all calls made by the dual octree visitor for
      // features of dimension d.  Note that this does not depend on
      // the embedding dimension D.  Only the number of cells
      // surrounding the features changes with the embedding
      // dimension.
      template <unsigned int d>
      struct generate_calls
      {
        // Call "F::apply<k, c, i>::invoke(arg1)" for each axis
        // combination c and call index i.  There are d bits in c of
        // which k are non-zero.  There are k bits in i.  F will be
        // called for every bit combination of c and i, which is a total
        // of 3^d times (2^k for each combination c with k bits set).
        template <typename F>
        static void invoke(typename F::arg1_type arg1, F* = 0)
          {
          meta::call1<typename gen_calls<d>::template apply<0,0,0>, F>::invoke(arg1);
          }
      };

      // Enumerate the arguments passed to call i of axis combination c
      // when made for embedding dimension D in a dual octree cell
      // visitor of dimension d with axis mask m, where d bits of m are
      // set to 1.
      template <unsigned int D, unsigned int m, unsigned int c, unsigned int i>
      struct generate_args
      {
        // Call "F::apply<j, inarg, child>::invoke(arg1, arg2)" for each
        // argument of the current call.  Child "child" of input
        // argument "inarg" must be passed to output argument j unless
        // inarg is a leaf in which case it is just passed directly.
        template <typename F>
        static void invoke(typename F::arg1_type arg1,
                           typename F::arg2_type arg2, F* = 0)
          {
          meta::call2<typename gen_args<D>::template apply<m, c, i, 0, 0, 0>, F>::invoke(arg1, arg2);
          }
      };

      // Compute the axis mask for axis combination c in embedding
      // dimension D in a dual octree cell visitor of dimension d with
      // axis mask m, where d bits of m are set to 1.  This is just a
      // mask of D bits set to 1 if the corresponding bit of the child
      // index depends on the output argument index j and 0 if it
      // depends on the call index i.
      template <unsigned int D, unsigned int m, unsigned int c>
      struct generate_m
      {
      private:
        typedef typename gen_m<D>::template apply<m, c> apply_type;
      public:
        RGTL_STATIC_CONST(unsigned int, value = apply_type::value);
      };

      // Function to pass to generate_calls for a given procedure.
      template <typename P>
      struct caller
      {
        // Lookup the procedure interface.
        typedef typename P::argument_type argument_type;
        typedef argument_type const& arg1_type;
        RGTL_STATIC_CONST(unsigned int, D = P::value_D);
        RGTL_STATIC_CONST(unsigned int, d = P::value_d);
        RGTL_STATIC_CONST(unsigned int, m = P::value_m);

        template <unsigned int k, unsigned int c, unsigned int i>
        struct apply
        {
          // Borland wants another copy of these here.
          RGTL_STATIC_CONST(unsigned int, D = P::value_D);
          RGTL_STATIC_CONST(unsigned int, d = P::value_d);
          RGTL_STATIC_CONST(unsigned int, m = P::value_m);

          // Compute the m
          RGTL_STATIC_CONST(unsigned int, n = (generate_m<D, m, c>::value));

          static void invoke(arg1_type in)
            {
            // Lookup the procedure to call.
            typedef typename P::template other<k, n>::type other_type;

            // Generate call arguments.
            typename other_type::argument_type out(in);
            typedef typename P::template argument_get<k, n>::type arg_get_type;
            meta::call2<generate_args<D, m, c, i>, arg_get_type>::invoke(in, out);

            // Make the recursive call.
            other_type::invoke(out);
            }
        };
      private:
        RGTL_STATIC_ASSERT(meta::bits_c<m>::value == (D-d));
      };
    } // namespace rgtl::detail::octree_dual
  } // namespace rgtl::detail

  // Primary interface.
  namespace octree_dual
  {
    // Generate recursive calls for a dual octree visiting procedure.
    // The procedure should define the following interface:
    //
    //  value_D = Dimension of octree embedding space
    //  value_d = Dimension of octree feature visited by the procedure
    //  value_m = Axis mask of the procedure
    //  argument_type = Type encoding procedure runtime arguments
    //  other<k,n>::type = The procedure for D=D, d=k, m=n
    //  argument_get<k>::type = Argument assignment procedure
    //  invoke(argument_get const&) = Runtime invocation function
    //
    // Sample usage inside invoke(argument_get const& in):
    //  octree_dual::recurse<procedure>::invoke(in);
    template <typename P>
    class recurse
    {
      RGTL_STATIC_CONST(unsigned int, d = P::value_d);
      typedef typename P::argument_type argument_type;
    public:
      static void invoke(argument_type const& arg1)
        {
        typedef detail::octree_dual::generate_calls<d> call_generator_type;
        typedef detail::octree_dual::caller<P> caller_type;
        meta::call1<call_generator_type, caller_type>::invoke(arg1);
        }
    };

  } // namespace rgtl::octree_dual
} // namespace rgtl

#endif
