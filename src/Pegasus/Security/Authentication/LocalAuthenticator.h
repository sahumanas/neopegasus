//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies of substantial portions of this software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_LocalAuthenticator_h
#define Pegasus_LocalAuthenticator_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include "Linkage.h"

PEGASUS_NAMESPACE_BEGIN

   
/**
    This is an abstract class that the local authenticators will extend and 
    provide the implementation. Sample implementation can be seen in 
    SecureLocalAuthenticator.cpp
*/ 
class PEGASUS_SECURITY_LINKAGE LocalAuthenticator
{
public:

    /** constructor. */ 
    LocalAuthenticator() { };

    /** destructor. */ 
    virtual ~LocalAuthenticator() { };

    /** authenticate. */ 
    virtual Boolean authenticate(String userName, String password) = 0;

    /** authenticate. */ 
    virtual Boolean authenticate(
        String userName, 
        String secretReceived, 
        String secretKept) = 0;

    /** getAuthResponseHeader. */ 
    virtual String getAuthResponseHeader(
        String requestHeader, 
        String& challenge) = 0;
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_LocalAuthenticator_h */
