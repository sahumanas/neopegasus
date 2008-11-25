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
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Print.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Client/CIMClientRep.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

//==============================================================================
//
// TestBinaryClient host port
//
//     This program enumerates instances of CIM_ManagedElement using the
//     OpenPegasus binary protocol.
//
//==============================================================================

static void _SetBinaryResponse(CIMClient& client, Boolean flag)
{
    CIMClientRep* rep = *(reinterpret_cast<CIMClientRep**>(&client));
    rep->setBinaryResponse(flag);
}

int main(int argc, char** argv)
{
    // Check args:

    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " host port" << endl;
        exit(1);
    }

    // Extract args:

    String host = argv[1];
    Uint32 port = atoi(argv[2]);

    if (port == 0)
    {
        cerr << argv[0] << ": illegal value for port number" << endl;
        exit(1);
    }

    // Connect and enumerate instances.

    CIMClient client;
    _SetBinaryResponse(client, true);

    try
    {
        client.connect(host, port, String(), String());

        Array<CIMInstance> result = client.enumerateInstances("root/cimv2",
            "CIM_ManagedElement");

        for (Uint32 i = 0; i < result.size(); i++)
        {
#if defined(PEGASUS_DEBUG)
            PrintInstance(cout, result[i]);
#endif
        }
    }
    catch (Exception& e)
    {
        cerr << e.getMessage() << endl;
        exit(1);
    }

    cout << "TestBinaryClient +++++ passed all tests" << endl;

    return 0;
}