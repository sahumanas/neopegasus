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
// Author: Susan Campbell, Hewlett-Packard Company (scampbell@hp.com)
//
// Modified By:
//         Denise Eckstein, Hewlett-Packard Company <denise_eckstein@hp.com>
//         Lyle Wilkinson, Hewlett-Packard Company <lyle_wilkinson@hp.com>
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _DNSINFO_H
#define _DNSINFO_H

#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

class DNSInfo
{
  public:
     DNSInfo(void);
     ~DNSInfo(void);

     void getDNSInfo(const int argc, const char** argv);

     // utility methods for common functions
     void errorExit(const String &message);
     void gatherProperties(CIMInstance &inst, Boolean cimFormat);
     void displayProperties();

  private:
     String dnsSystemName;       // the hostname
     String dnsSystemCreationClassName;
     String dnsName;             // the name of the DNS Service
     String dnsCreationClassName;
     String dnsCaption;
     String dnsDescription;
     Array<String> dnsSearchList;
     Array<String> dnsAddresses;
     void _usage();
};

#endif
