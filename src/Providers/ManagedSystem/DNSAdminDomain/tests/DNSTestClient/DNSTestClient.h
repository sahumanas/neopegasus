//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Paulo F. Borges (pfborges@wowmail.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef _DNSTESTCLIENT_H
#define _DNSTESTCLIENT_H

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>	
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

// Miscelaneous defines
#define RESOLV_CONF 			"/etc/resolv.conf"
#define NAMESPACE 				"root/cimv2"	
#define CLASS_NAME				"PG_DNSAdminDomain"
#define CAPTION					"DNS Admin Domain"
#define DESCRIPTION				"This is the PG_DNSAdminDomain object"
#define NAME_FORMAT				"IP"	

class DNSTestClient
{
  public:
     DNSTestClient(CIMClient &client);
     ~DNSTestClient(void);

     void testEnumerateInstanceNames(CIMClient &client, Boolean verbose);
     void testEnumerateInstances(CIMClient &client, Boolean verbose);
     void testGetInstance(CIMClient &client, Boolean verbose);

     // utility methods for common test functions
     void errorExit(const String &message);
     void testLog(const String &message);

     // class is hardcoded, but flag for level of logging
     Boolean goodCreationClassName(const String &ccn, 
                                   Boolean verbose);
     // have DNS-specific checks 
     Boolean goodName(const String &name, Boolean verbose);
     
     // have searchlist array checks
     Boolean goodSearchList(const Array<String> &src, Boolean verbose);
     
	 // have addresses array checks
     Boolean goodAddresses(const Array<String> &addr, Boolean verbose);
    
     // caption is hardcoded		
	 Boolean goodCaption(const String &cap, Boolean verbose);
	 
	 // description is hardcoded
	 Boolean goodDescription(const String &des, Boolean verbose);
	 
	 // nameformat is hardcoded
	 Boolean goodNameFormat(const String &nf, Boolean verbose);

  private:
  	// Verify if string is equal, then return true
	Boolean IsEqual(char text[], const char txtcomp[]);

	// Verify if found string in array
	Boolean FindInArray(Array<String> src, String text);
	
     // validate keys of the class
     void _validateKeys(CIMObjectPath &cimRef, Boolean verboseTest);
     
     // validate properties of the class
     void _validateProperties(CIMInstance &cimInst, Boolean verboseTest);
};

#endif
