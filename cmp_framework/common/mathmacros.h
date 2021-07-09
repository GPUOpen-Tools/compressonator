#ifndef _MATH_MACROS_H_
#define _MATH_MACROS_H_

// requires C++14 and above
// #pragma warning( push )
// #pragma warning(disable:4018)
// 
// template <typename primNumType, typename secNumType>
// inline decltype(auto) cmp_minT( const primNumType& a, const secNumType& b ) {
//     return ( a < b ? a : b );
// }
// template <typename primNumType, typename secNumType>
// inline decltype(auto) cmp_maxT( const primNumType& a, const secNumType& b ) {
//     return ( a > b ? a : b );
// }
// 
// #pragma warning(pop)

#ifndef cmp_maxT
#define cmp_maxT(x, y) (((x) > (y)) ? (x) : (y))
#endif
// 
#ifndef cmp_minT
#define cmp_minT(x, y) (((x) < (y)) ? (x) : (y))
#endif


#endif //_MATH_MACROS_H_