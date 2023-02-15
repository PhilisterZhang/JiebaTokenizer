/*
 * =====================================================================================
 *
 *       Filename:  make_unique.h
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/30/2018 11:40:39 AM
 * =====================================================================================
 */
#ifndef UTIL_MAKE_UNIQUE_H_
#define UTIL_MAKE_UNIQUE_H_

#include <memory>
#include <utility>
#include <cstddef>
#include <type_traits>

namespace std {
    template<class T> struct _Unique_if {
        typedef unique_ptr<T> _Single_object;
    };

    template<class T> struct _Unique_if<T[]> {
        typedef unique_ptr<T[]> _Unknown_bound;
    };

    template<class T, size_t N> struct _Unique_if<T[N]> {
        typedef void _Known_bound;
    };

    template<class T, class... Args>
        typename _Unique_if<T>::_Single_object
        make_unique(Args&&... args) {
            return unique_ptr<T>(new T(std::forward<Args>(args)...));
        }

    template<class T>
        typename _Unique_if<T>::_Unknown_bound
        make_unique(size_t n) {
            typedef typename remove_extent<T>::type U;
            return unique_ptr<T>(new U[n]());
        }

    template<class T, class... Args>
        typename _Unique_if<T>::_Known_bound
        make_unique(Args&&...) = delete;
}

#endif
/*  vim: set ts=4 sw=4 sts=4 tw=100 : */

