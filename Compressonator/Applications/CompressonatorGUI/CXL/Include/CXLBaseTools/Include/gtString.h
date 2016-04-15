//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtString.h 
/// 
//=====================================================================

//------------------------------ gtString.h ------------------------------

#ifndef __GTSTRING_H
#define __GTSTRING_H

// STL:
#include <string>

// Local:
#include <CXLBaseTools/Include/AMDTDefinitions.h>
#include <CXLBaseTools/Include/gtIgnoreCompilerWarnings.h>
#include <CXLBaseTools/Include/gtGRBaseToolsDLLBuild.h>

// ----------------------------------------------------------------------------------
// Class Name:           gtString
// General Description: A string represented as characters array.
// Author:      AMD Developer Tools Team
// Creation Date:        17/5/2003
// Implementation notes:
//   We use the STL string as a member instead of inheriting it because:
//   a. We would like to be able to switch easily to another string implementation.
//   b. We would like the user of this class to be able to use it without the need of
//      the template brackets <>.
// ----------------------------------------------------------------------------------
class GT_API gtString
{
public:
    gtString();
    gtString(const gtString& otherString);
    gtString(const wchar_t* pOtherString);
    gtString(const wchar_t* pOtherString, int len);
    gtString(wchar_t character);
    ~gtString();

#if AMDT_HAS_CPP0X
    gtString(gtString&& otherString);
    gtString& operator=(gtString&& otherString);
    gtString& assign(gtString&& otherString);
#endif

    const wchar_t* asCharArray() const;
    const char* asUTF8CharArray() const;
    const char* asASCIICharArray() const;
    const char* asASCIICharArray(int length) const;
    int asUtf8(std::string& utf8String) const;
    int length() const;
    int lengthInBytes() const;
    bool isEmpty() const;

    gtString& makeEmpty();

    gtString& assign(wchar_t character, int count = 1);
    gtString& assign(const wchar_t* pOtherString);
    gtString& assign(const wchar_t* pOtherString, int length);
    gtString& assign(const gtString& otherString);

    gtString& append(wchar_t character);
    gtString& append(const wchar_t* pOtherString);
    gtString& append(const wchar_t* pOtherString, int length);
    gtString& append(const gtString& otherString);
    gtString& appendFormattedString(const wchar_t* pFormatString, ...);
    gtString& appendUnsignedIntNumber(unsigned int uintNumber);

    gtString& prepend(wchar_t character);
    gtString& prepend(const wchar_t* pOtherString);
    gtString& prepend(const wchar_t* pOtherString, int length);
    gtString& prepend(const gtString& otherString);
    gtString& prependFormattedString(const wchar_t* pFormatString, ...);

    int find(const gtString& subString, int searchStartPosition = 0) const;
    int find(wchar_t character, int searchStartPosition = 0) const;
    int findNextLine(int searchStartPosition = 0) const;
    int lineNumberFromCharacterIndex(int characterIndex, bool oneBased = true) const;
    int reverseFind(const gtString& subString, int searchStartPosition = -1) const;
    int reverseFind(wchar_t character, int searchStartPosition = -1) const;
    int count(const gtString& subString, int countStartPosition = 0) const;
    int count(wchar_t character, int countStartPosition = 0) const;
    bool startsWith(const gtString& prefixString) const;
    bool endsWith(const gtString& suffixString) const;
    bool onlyContainsCharacters(const gtString& validCharacterList) const;

    int findFirstOf(const gtString& characters, int searchStartPosition = 0) const;
    int findFirstNotOf(const gtString& characters, int searchStartPosition = 0) const;
    int findLastOf(const gtString& characters) const;

    bool isAlpha(const gtString& validCharacterList = L"");
    bool isAlnum(const gtString& validCharacterList = L"");

    // TO_DO: Unicode turn to operators after compilation is done:
    // We do not want these functions to be used frequently since they require
    // unicode to ASCII conversion, therefore, at the meantime we do not
    // support them as operators, to avoid automatic conversions:
    gtString& fromASCIIString(const char* pOtherString);
    gtString& fromASCIIString(const char* pOtherString, int stringLength);
    gtString& fromUtf8String(const char* pOtherString);
    gtString& fromUtf8String(const std::string& utf8String);
    bool isEqual(const char* pOtherString);

    gtString& operator=(wchar_t c);
    gtString& operator=(const wchar_t* pOtherString);
    gtString& operator=(const gtString& otherString);
    gtString& operator+=(wchar_t character) { return append(character); };
    gtString& operator+=(const wchar_t* pOtherString) { return append(pOtherString); };
    gtString& operator+=(const gtString& otherString) { return append(otherString); };
    const wchar_t& operator[](int i) const;
    wchar_t& operator[](int i);

    bool operator<(const gtString& otherString) const;
    bool operator>(const gtString& otherString) const;

    int compareNoCase(const gtString& otherString) const;
    int compare(const wchar_t* pOtherString) const;
    int compare(const gtString& otherString) const;
    int compare(int pos, int len, const wchar_t* pOtherString) const;
    int compare(int pos, int len, const gtString& otherString) const;

    /// Compare the other string a non case sensitive comparison:
    /// \param otherString the string to compare to
    /// \return true if both strings are equal
    bool isEqualNoCase(const gtString& otherString) const;

    gtString& removeTrailing(wchar_t c);
    gtString& trim();
    void getSubString(int startPosition, int endPosition, gtString& subString) const;
    gtString& truncate(int startPosition, int endPosition);
    gtString& extruct(int startPosition, int endPosition);

    int replace(const gtString& oldSubString, const gtString& newSubString, bool replaceAllOccurrences = true);

    int replace(int startPos, int endPos, const gtString& oldSubString, const gtString& newSubString, bool replaceAllOccurrences = true);

    gtString& toUpperCase(int startPosition = 0, int endPosition = -1);
    gtString& toLowerCase(int startPosition = 0, int endPosition = -1);

    bool isIntegerNumber() const;
    bool toIntNumber(int& intNumber) const;
    bool toUnsignedIntNumber(unsigned int& uintNumber) const;
    bool toLongNumber(long& longNumber) const;
    bool toUnsignedLongNumber(unsigned long& ulongNumber) const;
    bool toLongLongNumber(long long& longLongNumber) const;
    bool toUnsignedLongLongNumber(unsigned long long& unsignedLongLongNumber) const;
    bool toUnsignedInt64Number(gtUInt64& unsignedInt64Number) const;

    gtString& addThousandSeperators();

    gtString& fromMemorySize(gtUInt64 memoryInSize);

    gtString& removeChar(wchar_t c);

    void reserve(size_t requestedLength = 0);
    void resize(size_t newSize);

private:
    // Friend operators:
    friend GT_API bool operator==(const gtString& str1, const gtString& str2);
    friend GT_API bool operator==(const gtString& str1, const wchar_t* pString);
    friend GT_API bool operator==(const wchar_t* pString, const gtString& str2);
    friend GT_API gtString& operator<<(gtString& target, const char* input);
    friend GT_API gtString& operator<<(gtString& target, const wchar_t* input);
    friend GT_API gtString& operator<<(gtString& target, const gtString& input);
    friend GT_API gtString& operator<<(gtString& target, int input);
    friend GT_API gtString& operator<<(gtString& target, unsigned int input);
    friend GT_API gtString& operator<<(gtString& target, long input);
    friend GT_API gtString& operator<<(gtString& target, unsigned long input);
    friend GT_API gtString& operator<<(gtString& target, float input);
    friend GT_API gtString& operator<<(gtString& target, double input);

private:
    // The internal implementation of this class is an SGI STL string:
    std::wstring _impl;

    // The char* ASCII char array returned:
    mutable char* _stringAsASCIICharArray;
};


// Operators:
GT_API bool operator==(const gtString& str1, const gtString& str2);
GT_API bool operator==(const gtString& str1, const wchar_t* pString);
GT_API bool operator==(const wchar_t* pString, const gtString& str2);

GT_API bool operator!=(const gtString& str1, const gtString& str2);
GT_API bool operator!=(const gtString& str1, const wchar_t* pString);
GT_API bool operator!=(const wchar_t* pString, const gtString& str2);

// Aid functions:
GT_API bool gtIsDigit(wchar_t c);
GT_API int gtWideStringToUtf8String(const std::wstring& org, std::string& dst);
GT_API int gtUtf8StringToWideString(const std::string& org, std::wstring& dst);

// Aid functions - Windows only:
GT_API size_t gtASCIIStringToUnicodeString(const char* pANSIString, wchar_t* pUnicodeStringBuff, size_t UnicodeStringBuffSize);
GT_API size_t gtUnicodeStringToASCIIString(const wchar_t* pUnicodeString, char* pANSIStringBuff, size_t ANSIStringBuffSize);

// Implements strtok for unicode:
// TO_DO: Unicode: implement (can copy implementation from WX)
GT_API wchar_t* gtStrTok(wchar_t* str, const wchar_t* delim);


#endif //__GTSTRING_H

