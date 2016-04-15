//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtASCIIString.h 
/// 
//=====================================================================

//------------------------------ gtASCIIString.h ------------------------------

#ifndef __GTANSIISTRING_H
#define __GTANSIISTRING_H

// STL:
#include <string>
#include <list>

// Local:
#include <CXLBaseTools/Include/AMDTDefinitions.h>
#include <CXLBaseTools/Include/gtIgnoreCompilerWarnings.h>
#include <CXLBaseTools/Include/gtGRBaseToolsDLLBuild.h>

// Allow using types from the std namespace:
using namespace std;

// ----------------------------------------------------------------------------------
// Class Name:           gtASCIIString
// General Description: A string represented as characters array.
// Author:      AMD Developer Tools Team
// Creation Date:        17/5/2003
// Implementation notes:
//   We use the STL string as a member instead of inheriting it because:
//   a. We would like to be able to switch easily to another string implementation.
//   b. We would like the user of this class to be able to use it without the need of
//      the template brackets <>.
// ----------------------------------------------------------------------------------
class GT_API gtASCIIString
{
public:
    gtASCIIString();
    gtASCIIString(const gtASCIIString& otherString);
    gtASCIIString(const char* pOtherString);
    gtASCIIString(char character);
    ~gtASCIIString();

#if AMDT_HAS_CPP0X
    gtASCIIString(gtASCIIString&& otherString);
    gtASCIIString& operator=(gtASCIIString&& otherString);
#endif

    const char* asCharArray() const;
    int length() const;
    bool isEmpty() const;

    gtASCIIString& makeEmpty();
    gtASCIIString& append(char character);
    gtASCIIString& append(const char* pOtherString);
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    gtASCIIString& append(const wchar_t* pOtherString);
#endif
    gtASCIIString& append(const char* pOtherString, int length);
    gtASCIIString& append(const gtASCIIString& otherString);
    gtASCIIString& appendFormattedString(const char* pFormatString, ...);
    gtASCIIString& prepend(char character);
    gtASCIIString& prepend(const char* pOtherString);
    gtASCIIString& prepend(const char* pOtherString, int length);
    gtASCIIString& prepend(const gtASCIIString& otherString);
    gtASCIIString& prependFormattedString(const char* pFormatString, ...);

    int find(const gtASCIIString& subString, int searchStartPosition = 0) const;
    int find(char character, int searchStartPosition = 0) const;
    int findNextLine(int searchStartPosition = 0) const;
    int lineNumberFromCharacterIndex(int characterIndex, bool oneBased = true) const;
    int reverseFind(const gtASCIIString& subString, int searchStartPosition = -1) const;
    int reverseFind(char character, int searchStartPosition = -1) const;
    int count(const gtASCIIString& subString, int countStartPosition = 0) const;
    int count(char character, int countStartPosition = 0) const;
    bool startsWith(const gtASCIIString& prefixString) const;
    bool onlyContainsCharacters(const gtASCIIString& validCharacterList) const;
    void decodeHTML();

    gtASCIIString& operator=(char c);
    gtASCIIString& operator=(const char* pOtherString);
    gtASCIIString& operator=(const gtASCIIString& otherString);
    gtASCIIString& operator+=(char character) { return append(character); };
    gtASCIIString& operator+=(const char* pOtherString) { return append(pOtherString); };
    gtASCIIString& operator+=(const gtASCIIString& otherString) { return append(otherString); };
    const char& operator[](int i) const;
    char& operator[](int i);

    bool operator<(const gtASCIIString& otherString) const;
    bool operator>(const gtASCIIString& otherString) const;
    int compareNoCase(const gtASCIIString& otherString) const;

    gtASCIIString& removeTrailing(char c);
    void getSubString(int startPosition, int endPosition, gtASCIIString& subString) const;
    gtASCIIString& truncate(int startPosition, int endPosition);

    int replace(const gtASCIIString& oldSubString, const gtASCIIString& newSubString,
                bool replaceAllOccurrences = true);

    int replace(int startPos, int endPos, const gtASCIIString& oldSubString, const gtASCIIString& newSubString,
                bool replaceAllOccurrences = true);

    gtASCIIString& toUpperCase(int startPosition = 0, int endPosition = -1);
    gtASCIIString& toLowerCase(int startPosition = 0, int endPosition = -1);

    bool isIntegerNumber() const;
    bool toIntNumber(int& intNumber) const;
    bool toUnsignedIntNumber(unsigned int& uintNumber) const;
    bool toLongNumber(long& longNumber) const;
    bool toUnsignedLongNumber(unsigned long& ulongNumber) const;
    bool toLongLongNumber(long long& longLongNumber) const;
    bool toUnsignedLongLongNumber(unsigned long long& unsignedLongLongNumber) const;

    gtASCIIString& addThousandSeperators();

    gtASCIIString& fromMemorySize(gtUInt64 memoryInSize);

    gtASCIIString& removeChar(char c);

    void resize(size_t newSize);

private:
    // Friend operators:
    friend GT_API bool operator==(const gtASCIIString& str1, const gtASCIIString& str2);
    friend GT_API bool operator==(const gtASCIIString& str1, const char* pString);
    friend GT_API bool operator==(const char* pString, const gtASCIIString& str2);

private:
    // The internal implementation of this class is an SGI STL string:
    string _impl;


    // ----------------------------------------------------------------------------------
    // Legacy methods used by PerfStudio2. Assume that these are deprecated.
public:
    void Split(const gtASCIIString& rSep, bool bCaseSensitive, list<gtASCIIString>& outList) const;
    size_t find_first_not_of(const char* srcStr, const size_t startIndex = 0) const;
    size_t find_last_of(const char* src, const int startIndex = -1) const;
    gtASCIIString substr(int startPosition = 0, int count = static_cast<int>(string::npos)) const;
};


// Operators:
GT_API bool operator==(const gtASCIIString& str1, const gtASCIIString& str2);
GT_API bool operator==(const gtASCIIString& str1, const char* pString);
GT_API bool operator==(const char* pString, const gtASCIIString& str2);

GT_API bool operator!=(const gtASCIIString& str1, const gtASCIIString& str2);
GT_API bool operator!=(const gtASCIIString& str1, const char* pString);
GT_API bool operator!=(const char* pString, const gtASCIIString& str2);

// Aid functions:
GT_API bool gtIsDigit(char c);

// Aid functions - Windows only:
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    GT_API size_t gtASCIIStringToUnicodeString(const char* pANSIString, wchar_t* pUnicodeStringBuff, size_t UnicodeStringBuffSize);
    GT_API size_t gtUnicodeStringToASCIIString(const wchar_t* pUnicodeString, char* pANSIStringBuff, size_t ANSIStringBuffSize);
#endif

#endif //__gtASCIIString_H

