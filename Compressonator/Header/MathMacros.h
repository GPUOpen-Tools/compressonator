#ifndef _MATH_MACROS_H_
#define _MATH_MACROS_H_

template <typename primNumType, typename secNumType>
inline decltype(auto) min( const primNumType& a, const secNumType& b )
{
    return ( a < b ? a : b );
}

template <typename primNumType, typename secNumType>
inline decltype(auto) max( const primNumType& a, const secNumType& b )
{
    return ( a > b ? a : b );
}

#endif //_MATH_MACROS_H_