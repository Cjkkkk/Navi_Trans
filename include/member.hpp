#ifndef MEMBER_HPP
#define MEMBER_HPP

#include <exception>
#include <sstream>
#include <limits>
#include <vector>
#include <cstring>
#include <iostream>

#include "./pack.hpp"
#include "./types.hpp"
#include "./traits.hpp"


namespace navi_trans {
    template <typename T, T>
    struct Member; 

    template <typename T, typename Class, T Class::*Pointer> 
    struct Member<T Class::*, Pointer>{ 
        using type = T; 
        static T& resolve(Class& c) { 
            return c.*Pointer; 
        }
        static const T& const_resolve(const Class& c) { 
            return c.*Pointer; 
        }
    }; 

    template <typename ...member>
    struct MemberList
    { 
        using members = std::tuple<member...>; 
        enum: std::size_t {Count = sizeof...(member)};
    };

    template <typename T>
    struct MemberListTraits; 

}

#endif