// This is contrib/prip/vmap/vmap_
#ifndef vmap_map_iterator_wrapper_h_
#define vmap_map_iterator_wrapper_h_
//:
// \file
// \brief provides a wrapper in order to use any iterator on pairs (e.g. vcl_map::iterator) as if it was an iterator on the second element.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <map>

//: A wrapper in order to use any iterator on pairs (e.g. vcl_map::iterator) as if it was an iterator on the second element.
template <typename ref, typename ptr, class it>
class vmap_map_iterator_wrapper
{
public:
        //:
        vmap_map_iterator_wrapper()
        {}

        //:
        vmap_map_iterator_wrapper(const vmap_map_iterator_wrapper &right)
                : _current(right._current)
        {}
        
	//:
        vmap_map_iterator_wrapper(const it &right)
                : _current(right)
        {}

        //:
        ~vmap_map_iterator_wrapper()
        {}

        //:
        vmap_map_iterator_wrapper & operator=(const vmap_map_iterator_wrapper &right)
        {
            _current=right._current ;
            return *this ;
        }
	
	//:
        vmap_map_iterator_wrapper & operator=(const it &right)
        {
            _current=right ;
            return *this ;
        }

        //:
        int operator==(const vmap_map_iterator_wrapper &right) const
        {
            return _current==right._current ;
        }

        //:
        int operator!=(const vmap_map_iterator_wrapper &right) const
        {
            return _current!=right._current ;
        }

        //:
        vmap_map_iterator_wrapper & operator ++ ()
        {
            ++_current ;
            return *this ;
        }

        //:
        vmap_map_iterator_wrapper & operator -- ()
        {
            --_current ;
            return *this ;
        }

        /*const T & operator * () const
        {
            return (*_current).second ;
        }

        T & operator * ()
        {
            return (*_current).second ;
        }*/

        //:
        ref operator * () const
        {
            return (*_current).second ;
        }

        //:
        ptr operator->() const
        {
        	return &(operator*());
        }


    private:
        //:
        it _current;
    };


#endif
