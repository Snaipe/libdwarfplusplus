// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// File adapted from https://gist.github.com/martong/d526cc36c08dce8367f2

#ifndef LIBDWARF_XVECTOR_HH
# define LIBDWARF_XVECTOR_HH

/// martong: Putting Dave's code in a namespace
namespace typelist {

//
//   assert_same -- a low-rent assertion
//__________________________________________________________
    template <class T, class U> struct assert_same;
    template <class T> struct assert_same<T,T> {};

//
//   places -- a wrapper for "N variadic parameters"
//__________________________________________________________

    enum place { _ };
    template <place...>
    struct places {};

// Glue two specializations of places together
    template <class P1, class P2> struct append_places;

    template <place...X1, place...X2>
    struct append_places<places<X1...>, places<X2...> >
    {
        typedef places<X1...,X2...> type;
    };

// Generate places<_,_,_,..._> with N arguments in O(log N)
    template <unsigned N>
    struct make_places
            : append_places<
                    typename make_places<N/2>::type,
                    typename make_places<N-N/2>::type
            >
    {};

    template <> struct make_places<0> { typedef places<> type; };
    template <> struct make_places<1> { typedef places<_> type; };

//
//   types -- a basic type sequence
//__________________________________________________________
    template <class...T> struct types
    {
        typedef types type;
    };

//
//   nth_type -- get the Nth type in a sequence in O(1)
//__________________________________________________________

// Wrapper to prevent type decay
    template <class T>
    struct no_decay
    {
        typedef T type;
    };

// Use as function parameter that eats any POD
    template <place ignored>
    struct eat
    { eat(...); };

// inner beauty
    template <class T> struct nth_type_impl;

    template <place...X>
    struct nth_type_impl<places<X...> >
    {
        template <class U, class...Tail>
        static U result(eat<X>..., U*, Tail*...);
    };

// Select the Nth type in O(1) (once all the necessary places<...> have
// been instantiated, which is O(log N) but happens only once.
    template <unsigned N, class S> struct nth_type;

    template <unsigned N, class...T>
    struct nth_type<N, types<T...> >
            : decltype(
              nth_type_impl<typename make_places<N>::type>::result(
                      (no_decay<T>*)0 ...))
    {};

//
//   drop -- drop N elements from the front of a type sequence in O(1)
//______________________________________________________________________


// inner beauty
    template <class T> struct drop_impl;

    template <place...X>
    struct drop_impl<places<X...> >
    {
        template <class...Tail>
        static types<Tail...> result(eat<X>..., no_decay<Tail>*...);
    };

    template <unsigned N, class S> struct drop;

    template <unsigned N, class...T>
    struct drop<N, types<T...> >
            : decltype(
              drop_impl<typename make_places<N>::type>::result(
                      (no_decay<T>*)0 ...))
    {};

//
//   indices -- a sequence of unsigned integers
//__________________________________________________________
    template <unsigned...N> struct indices
    {
        typedef indices type;
    };

// Glue two sets of indices together
    template <class I1, class I2> struct append_indices;

    template <unsigned...N1, unsigned...N2>
    struct append_indices<indices<N1...>, indices<N2...> >
    {
        typedef indices<N1..., (sizeof...(N1)+N2)...> type;
    };

// generate indices [0,N) in O(log(N)) time
    template <unsigned N>
    struct make_indices
            : append_indices<
                    typename make_indices<N/2>::type
                    , typename make_indices<N-N/2>::type
            >
    {};
    template <> struct make_indices<0> { typedef indices<> type; };
    template <> struct make_indices<1> { typedef indices<0> type; };

//
//   take -- return the first N types in the sequence
//   Note: this is an O(N) algorithm.
//___________________________________________________________
    template <class I, class S> struct take_impl;

    template <unsigned...N, class S>
    struct take_impl<indices<N...>, S>
    {
        typedef types<typename nth_type<N,S>::type...> type;
    };

    template <unsigned N, class S>
    struct take
            : take_impl<typename make_indices<N>::type, S>
    {};

//
//   front -- return the first type in the sequence
//___________________________________________________________
    template <class S> struct front;

    template <class H, class...T>
    struct front<types<H,T...> >
    {
        typedef H type;
    };

//
//   back -- return the last type in the sequence
//___________________________________________________________
    template <class S> struct back;

    template <class...T>
    struct back<types<T...> >
            : nth_type<sizeof...(T)-1, types<T...> >
    {};

    template <class S> struct begin : front<S> {};

} // namespace typelist

/// ===========================================================================
/// making Dave's type container to fulfill mpl's Sequence concept

#include <boost/mpl/long.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/variant.hpp>
#include <boost/mpl/advance_fwd.hpp>
#include <boost/mpl/distance_fwd.hpp>

namespace typelist {

    struct xvector_tag;
    namespace aux {
        struct v_iter_tag;
    }

    template<typename Vector, long n_>
    struct v_iter {
        typedef aux::v_iter_tag tag;
        typedef boost::mpl::random_access_iterator_tag category;
        typedef typename nth_type<n_, typename Vector::types>::type type;

        typedef Vector vector_;
        typedef boost::mpl::long_<n_> pos;

        enum {
            next_ = n_ + 1
            , prior_ = n_ - 1
            , pos_ = n_
        };

        typedef v_iter<Vector,next_> next;
        typedef v_iter<Vector,prior_> prior;
    };

    struct none;
// Drop-in replacement of the 50 size limited boost::mpl::vector
    template <typename... Ts>
    struct xvector {
        // none is used to avoid accessing out-of-bounds type with nth_type.
        using types = typelist::types<Ts..., none>;
        using tag = xvector_tag;
        typedef v_iter<xvector,0> begin;
        typedef v_iter<xvector,sizeof...(Ts)> end;
    };
} // namespace typelist

namespace boost {
    namespace mpl {

        template<>
        struct clear_impl< typelist::xvector_tag >
        {
            template< typename Vector > struct apply
            {
                typedef typelist::xvector<> type;
            };
        };

        template <typename... Ts> struct xv_p_apply_helper;
        template<typename T, typename... Ts>
        struct xv_p_apply_helper<typelist::xvector<Ts...>, T> {
            using type = typelist::xvector<T, Ts...>;
        };
        template<>
        struct push_front_impl< typelist::xvector_tag >
        {
            template< typename Vector, typename T > struct apply
            {
                //typedef v_item<T,Vector,1> type;
                using type = typename xv_p_apply_helper<Vector, T>::type;
            };
        };

        template<> struct advance_impl<typelist::aux::v_iter_tag>
        {
            template< typename Iterator, typename N > struct apply
            {
                enum { pos_ = Iterator::pos_, n_ = N::value };
                typedef typelist::v_iter<
                        typename Iterator::vector_
                        , (pos_ + n_)
                > type;
            };
        };

        template<> struct distance_impl<typelist::aux::v_iter_tag>
        {
            template< typename Iter1, typename Iter2 > struct apply
            {
                enum { pos1_ = Iter1::pos_, pos2_ = Iter2::pos_ };
                typedef boost::mpl::long_<( pos2_ - pos1_ )> type;
                BOOST_STATIC_CONSTANT(long, value = ( pos2_ - pos1_ ));
            };
        };

    }
} // namespace boost::mpl

#endif /* !LIBDWARF_XVECTOR_HH */
