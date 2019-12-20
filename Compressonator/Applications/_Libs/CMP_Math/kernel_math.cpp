///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016, Intel Corporation
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of 
// the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ISPC
#include "..\..\Applications\_Plugins\Common\Common_Def.h"
#else
#include "Common_Def.h"
#endif

//-------------C EXPORTS -------------
//CGV_FLOAT sq(CGV_FLOAT v)
//{
//   return v*v;
//}
// EXPORT  void cmp_sq(CGU_INT N, CGU_FLOAT v[],CGU_FLOAT out[])
// {
//  #ifdef CMP_USE_FOREACH_ASPM
//     foreach (i = 0 ... N)
//     {
//  #else
//     for (CGU_INT i = 0; i<N; i++)
//     {
//  #endif
//         out[i] = sq(v[i]);
//     }
// }
