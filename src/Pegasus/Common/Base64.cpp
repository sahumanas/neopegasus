//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <strstream>
#include <string>
#include "Base64.h"
#include <Pegasus/Common/ArrayInternal.h>

#ifdef PEGASUS_OS_OS400
// OS400 is EBCDIC so it needs to convert string and characters to ASCII
// prior to decode and encode.  The encoding and decoding are done in ASCII.
#include <Pegasus/Common/OS400ConvertChar.h>  
#endif

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

#ifdef PEGASUS_PLATFORM_AIX_RS_IBMCXX
#define inline
#endif

//*************************************************************
/*  Encode static method takes an array of 8-bit values and
    returns a base-64 stream.
    ATTN: KS feb 2002 - This is probably a very slow an inefficient
    implementation and could be improved if it is required for
    production.  Today it is only for test programs.
*/
Array<Sint8> Base64::encode(const Array<Uint8>& vby)
{
    Array<Sint8> retArray;
    // If nothing in input string, return empty string
    if (vby.size() == 0)
        return retArray;
    // for every character in the input array taken 3 bytes at a time

#ifdef PEGASUS_OS_OS400
    //For OS400 convert from ebcdic to ascii
    EtoA((char *)vby.getData(), vby.size());
#endif

    for (Uint32 i=0; i < vby.size(); i+=3)
    {
        
        // Create from 3 8 bit values to 4 6 bit values
        Uint8 by1=0,by2=0,by3=0;
        by1 = vby[i];
        if (i+1<vby.size())
        {
            by2 = vby[i+1];
        };
        if (i+2<vby.size())
        {
            by3 = vby[i+2];
        }

        Uint8 by4=0,by5=0,by6=0,by7=0;
        by4 = by1>>2;
        by5 = ((by1&0x3)<<4)|(by2>>4);
        by6 = ((by2&0xf)<<2)|(by3>>6);
        by7 = by3&0x3f;

        retArray.append(_Encode(by4));
        retArray.append(_Encode(by5));

        if (i+1<vby.size())
            retArray.append( _Encode(by6));
        else
// NOTE:  If this code changes the OS400 code below may also need applicable changes
#ifdef PEGASUS_OS_OS400
            retArray.append(0x3D); /* ascii '=' */
#else
            retArray.append('='); 
#endif


        if (i+2<vby.size())
            retArray.append( _Encode(by7));
        else
// NOTE:  If this code changes the OS400 code below may also need applicable changes
#ifdef PEGASUS_OS_OS400
            retArray.append(0x3D); /* ascii '=' */
#else
            retArray.append('=');
#endif

        /* ATTN: Need to fix this. It adds unwanted cr-lf after 4 chars.

        if (i % (76/4*3) == 0)
        {
            retArray.append( '\r');
            retArray.append( '\n');
        }
        */
    };

#ifdef PEGASUS_OS_OS400
    //For OS400 convert from ascii to ebcdic
    AtoE((char *)retArray.getData(), retArray.size());
#endif

    return retArray;
};
/*I checked for the zero length. The algorithm would also work for zero length input stream, but I�m pretty adamant about handling border conditions. They are often the culprits of run-time production failures.
The algorithm goes thru each three bytes of data at a time. The first thing I do is to shift the bits around from three 8-bit values to four 6-bit values. Then I encode the 6-bit values and add then one at a time to the output stream. This is actually quite inefficient. The STL character array is being allocated one byte at a time. The algorithm would be much faster, if I pre-allocated that array. I�ll leave that as an optimization practical exercise for the reader.
*/

/*  The decode static method takes a base-64 stream and converts it 
    to an array of 8-bit values.
*/
Array<Uint8> Base64::decode(const Array<Sint8> strInput)
{
#ifdef PEGASUS_OS_OS400
    //For OS400 convert from ebcdic to ascii
    EtoA((char *)strInput.getData(), strInput.size());
#endif

    //Strip any non-base64 characters from the input
    PEGASUS_STD(string) str;
    for (Uint32 j=0;j<strInput.size();j++)
    {
        if (_IsBase64(strInput[j]))
            str += strInput[j];
    }

    Array<Uint8> retArray;

    // Return if the input is zero length
    if (str.length() == 0)
        return retArray;

    //  comment
    for (size_t i=0; i < str.length();i+=4)
    {
// NOTE:  If this code changes the OS400 code below may also need applicable changes
#ifdef PEGASUS_OS_OS400
        char c1=0x41,c2=0x41,c3=0x41,c4=0x41;  /* ascii 'A' */
#else
        char c1='A',c2='A',c3='A',c4='A';
#endif

        c1 = str[i];
        if (i+1<str.length())
            c2 = str[i+1];
        if (i+2<str.length())
            c3 = str[i+2];
        if (i+3<str.length())
            c4 = str[i+3];

        Uint8 by1=0,by2=0,by3=0,by4=0;
        by1 = _Decode(c1);
        by2 = _Decode(c2);
        by3 = _Decode(c3);
        by4 = _Decode(c4);
        //cout << "base::64decode bytes" <<
        //      " 1 " << c1 << " " << by1 <<
        //      " 2 " << c2 << " " << by2 <<
        //      " 3 " << c3 << " " << by3 <<
        //      " 4 " << c4 << " " << by4 << endl;
        
        // append first byte by shifting
        retArray.append( (by1<<2)|(by2>>4) );

        // append second byte if not padding
// NOTE:  If this code changes the OS400 code below may also need applicable changes
#ifdef PEGASUS_OS_OS400
        if (c3 != 0x3d)  /* ascii '=' */
#else
        if (c3 != '=')
#endif
            retArray.append( ((by2&0xf)<<4)|(by3>>2) );

// NOTE:  If this code changes the OS400 code below may also need applicable changes
#ifdef PEGASUS_OS_OS400
        if (c4 != 0x3d)  /* ascii '=' */
#else
        if (c4 != '=')
#endif
            retArray.append( ((by3&0x3)<<6)|by4 );
    }

#ifdef PEGASUS_OS_OS400
    //For OS400 convert from ascii to ebcdic
    AtoE((char *)retArray.getData(), retArray.size());
#endif

    return retArray;
};

//**********************************************************
/*  Encode thanslates one six-bit pattern into a base-64 character.
    Unsigned char is used to represent a six-bit stream of date.
    
*/
inline PEGASUS_COMMON_LINKAGE char Base64::_Encode(Uint8 uc)
{
// NOTE:  If this code changes the OS400 code below may also need applicable changes
#if !defined(PEGASUS_OS_OS400)
    if (uc < 26)
        return 'A'+uc;

    if (uc < 52)
        return 'a'+(uc-26);

    if (uc < 62)
        return '0'+(uc-52);

    if (uc == 62)
        return '+';

    return '/';
#else
    if (uc < 26)
        return 0x41+uc;	/* ascii 'A' */

    if (uc < 52)
        return 0x61+(uc-26);  /* ascii 'a' */

    if (uc < 62)
        return 0x30+(uc-52);  /* ascii '0' */

    if (uc == 62)
        return 0x2B;  /* ascii '+' */

    return 0x2F;  /* ascii '/' */
#endif
};

 //Helper function returns true is a character is a valid base-64 character and false otherwise.

inline Boolean Base64::_IsBase64(char c)
{

// NOTE:  If this code changes the OS400 code below may also need applicable changes
#if !defined(PEGASUS_OS_OS400)
    if (c >= 'A' && c <= 'Z')
        return true;

    if (c >= 'a' && c <= 'z')
        return true;

    if (c >= '0' && c <= '9')
        return true;

    if (c == '+')
        return true;

    if (c == '/')
        return true;

    if (c == '=')
        return true;
#else
    if (c >= 0x41 && c <= 0x5A)  /* ascii 'A' & 'Z' */
        return true;

    if (c >= 0x61 && c <= 0x7A)  /* ascii 'a' & 'z' */
        return true;

    if (c >= 0x30 && c <= 0x39) /* ascii '0' & '9' */
        return true;

    if (c == 0x2B) /* ascii '+' */
        return true;

    if (c == 0x2F) /* ascii '/' */
        return true;

    if (c == 0x3D) /* ascii '=' */
        return true;
#endif

    return false;
};
 
 // Translate one base-64 character into a six bit pattern
inline Uint8 Base64::_Decode(char c)
{
// NOTE:  If this code changes the OS400 code below may also need applicable changes
#if !defined(PEGASUS_OS_OS400)
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;

    if (c >= '0' && c <= '9')
        return c - '0' + 52;

    if (c == '+')
        return 62;
#else
    if (c >= 0x41 && c <= 0x5A)  /* ascii 'A' & 'Z' */
        return c - 0x41;	/* ascii 'A' */
    if (c >= 0x61 && c <= 0x7A)  /* ascii 'a' & 'z' */
	return c - 0x61 + 26;    /* ascii 'a' */

    if (c >= 0x30 && c <= 0x39) /* ascii '0' & '9' */
        return c - 0x30 + 52;  /* ascii '0' */

    if (c == 0x2B)  /* ascii '+' */
        return 62;
#endif

    return 63;
};


PEGASUS_NAMESPACE_END
