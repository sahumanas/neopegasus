//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//    Author: Vageesh Umesh (vagumesh@in.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/**
    This file has testcases that are added to cover the functions defined in
    AuthenticationInfo class.
*/

#include <cstdlib>
#include <iostream>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Common/String.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    AuthenticationInfo authInfo(true) ;

    /**
        Added to cover the Function
        void AuthenticationInfo::setAuthenticatedUser(const String& userName)
    */
#define TEST_USER "My Test User"
    PEGASUS_TEST_ASSERT(authInfo.getAuthenticatedUser() == String::EMPTY);
    authInfo.setAuthenticatedUser(TEST_USER);
    String ret_setAuthenticatedUser = authInfo.getAuthenticatedUser();
    PEGASUS_TEST_ASSERT(ret_setAuthenticatedUser == TEST_USER);

    /**
        Added to cover the Function
        void AuthenticationInfo::setAuthenticatedPassword(const String& password)
    */
#define TEST_PASSWORD "My Test Password"
    PEGASUS_TEST_ASSERT(authInfo.getAuthenticatedPassword() == String::EMPTY);
    authInfo.setAuthenticatedPassword(TEST_PASSWORD);
    String ret_setAuthenticatedPassword = authInfo.getAuthenticatedPassword();
    PEGASUS_TEST_ASSERT(ret_setAuthenticatedPassword == TEST_PASSWORD);

    /**
	    Added to cover the Function
        void AuthenticationInfo::setAuthSecret(const String& secret)
    */
#define TEST_SECRET "My Test Secret"
    PEGASUS_TEST_ASSERT(authInfo.getAuthSecret() == String::EMPTY);
    authInfo.setAuthSecret(TEST_SECRET);
    String ret_setAuthSecret = authInfo.getAuthSecret();
    PEGASUS_TEST_ASSERT(ret_setAuthSecret == TEST_SECRET);

    /**
	    Added to cover the Function
        void AuthenticationInfo::setPrivileged(Boolean privileged)
    */
    PEGASUS_TEST_ASSERT(authInfo.isPrivileged() == false);
    authInfo.setPrivileged(true);
    PEGASUS_TEST_ASSERT(authInfo.isPrivileged() == true);

    /**
        Added to cover the Function
        void AuthenticationInfo::setExportConnection(Boolean exportConnection)
    */
    PEGASUS_TEST_ASSERT(authInfo.isExportConnection() == false);
    authInfo.setExportConnection(true);
    PEGASUS_TEST_ASSERT(authInfo.isExportConnection() == true);

    /**
        Added to cover the Function
        void AuthenticationInfo::setAuthType(const String& authType)
    */
    PEGASUS_TEST_ASSERT(authInfo.getAuthType() == String::EMPTY);
    authInfo.setAuthType(AuthenticationInfoRep::AUTH_TYPE_SSL);
    String ret_setauthtype = authInfo.getAuthType();
    PEGASUS_TEST_ASSERT(ret_setauthtype == AuthenticationInfoRep::AUTH_TYPE_SSL);

    /**
        Added to cover the Function
        void AuthenticationInfo::setAuthChallenge(const String& challenge)
    */
#define TEST_CHALLENGE "My Test Challenge"
    PEGASUS_TEST_ASSERT(authInfo.getAuthChallenge() == String::EMPTY);
    authInfo.setAuthChallenge(TEST_CHALLENGE);
    String ret_setauthchallenge = authInfo.getAuthChallenge();
    PEGASUS_TEST_ASSERT(ret_setauthchallenge == TEST_CHALLENGE);

    /**
        Added to cover the Function
        void AuthenticationInfo::setAuthStatus(AuthStatus status)
    */
    PEGASUS_TEST_ASSERT(authInfo.getAuthStatus() == AuthenticationInfoRep::NEW_REQUEST);
    PEGASUS_TEST_ASSERT(authInfo.isAuthenticated() == false);
    authInfo.setAuthStatus(AuthenticationInfoRep::AUTHENTICATED);
    PEGASUS_TEST_ASSERT(authInfo.getAuthStatus() == AuthenticationInfoRep::AUTHENTICATED);
    PEGASUS_TEST_ASSERT(authInfo.isAuthenticated() == true);

    /**
        Added to cover the Function
        void AuthenticationInfo::setClientCertificateChain(
        Array<SSLCertificateInfo*> clientCertificate)
    */
    Array<SSLCertificateInfo*> clientCertificate;
    authInfo.setClientCertificateChain(clientCertificate);
    Array<SSLCertificateInfo*> ret_clientCertificate=authInfo.getClientCertificateChain();
    PEGASUS_TEST_ASSERT(clientCertificate == ret_clientCertificate);

    AuthenticationInfo authInfo1;
    AuthenticationInfo authInfo2 (authInfo);

    PEGASUS_TEST_ASSERT(authInfo2.getAuthStatus() == AuthenticationInfoRep::AUTHENTICATED);
    PEGASUS_TEST_ASSERT(authInfo2.isAuthenticated() == true);

    authInfo1 = authInfo;

    PEGASUS_TEST_ASSERT(authInfo1.getAuthStatus() == AuthenticationInfoRep::AUTHENTICATED);
    PEGASUS_TEST_ASSERT(authInfo1.isAuthenticated() == true);
}

int main( int argc, char *argv[] )
{
    try
    {
        test01();
    }
    catch (Exception& e)
    {
        PEGASUS_STD (cout) << "Exception: " << e.getMessage() << PEGASUS_STD(endl);
        exit(1);
    }

    PEGASUS_STD (cout) << argv[0] << " +++++ passed all tests" << PEGASUS_STD (endl);
    return 0;
}
