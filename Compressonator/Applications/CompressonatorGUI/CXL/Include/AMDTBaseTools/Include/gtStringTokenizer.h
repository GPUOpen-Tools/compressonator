//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtStringTokenizer.h 
/// 
//=====================================================================

//------------------------------ gtStringTokenizer.h ------------------------------

#ifndef __GTSTRINGTOKENIZER
#define __GTSTRINGTOKENIZER

// Local:
#include <AMDTBaseTools/Include/gtString.h>


// ----------------------------------------------------------------------------------
// Class Name:           gtStringTokenizer
// General Description:
//   Breaks a string into tokens.
//
// Author:      AMD Developer Tools Team
// Creation Date:        22/8/2003
// ----------------------------------------------------------------------------------
class GT_API gtStringTokenizer
{
public:
    gtStringTokenizer(const gtString& str, const gtString& delimiters);
    ~gtStringTokenizer();

    bool getNextToken(gtString& token);

private:

    // Assignment operator is not allowed:
    gtStringTokenizer& operator=(const gtStringTokenizer& other);

private:
    // The string to be "tokenized":
    wchar_t* _pString;

    // The string last char:
    wchar_t* _pStringLastChar;

    // The current position in the input string:
    wchar_t* _pCurrentPosition;

    // The tokens delimiters packed as a string:
    const gtString _delimitersString;

    // Is this the first call used by linux:
    bool _firstNextTokenCall;
};


#endif  // __GTSTRINGTOKENIZER
