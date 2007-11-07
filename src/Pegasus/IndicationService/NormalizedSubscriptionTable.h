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

#ifndef Pegasus_NormalizedSubscriptionTable_h
#define Pegasus_NormalizedSubscriptionTable_h

#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/ReadWriteSem.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/HashTable.h>

PEGASUS_NAMESPACE_BEGIN

/**
    NormalizedSubscriptionTable holds the all normalized subscriptions. This is
    used to avoid creation of duplicate subscriptions and handling of similar
    subscription requests at the same time.
*/

class PEGASUS_SERVER_LINKAGE NormalizedSubscriptionTable
{
public:
    NormalizedSubscriptionTable(const Array<CIMInstance> &subscriptions);
    ~NormalizedSubscriptionTable();

    /**
        Normalizes the subscription path. Host name will be removed from both
        filter and handler objectPaths. Namespace will also be removed from
        filter or handler if filter's or handler's namespace is same as
        subscription namespace.
        @param subPath subscription path
        @return normalized subscription path.
   */
    static CIMObjectPath normalize(const CIMObjectPath &subPath);
    /**
        Adds subscription path to the subscriptionTable after
        subscription path normalization.
        @param subPath, subscription path.
        @param value, associated key value.
        @return true if inserted successfully else false.
    */
    Boolean add(const CIMObjectPath &subPath, const Boolean &value = true);

    /**
        Deletes subscription path from the subscriptionTable after
        subscription path normalization.
        @param subPath subscription path.
        @return true if deleted successfully else false.
    */
    Boolean remove(const CIMObjectPath &subPath);

    /**
        Checks for subscription path from the subscriptionTable after
        subscription path normalization.
        @param subPath subscription path.
        @param value, output arg, contains associated key value if found.
        @return true if found else false.
    */
    Boolean exists(const CIMObjectPath &subPath, Boolean &value);

private:
    NormalizedSubscriptionTable& operator = 
        (const NormalizedSubscriptionTable&);
    NormalizedSubscriptionTable(const NormalizedSubscriptionTable&);


    /**
        NormalizedSubscriptionTable maintains all the subscription objectpaths
        in normalized form.
    */
    HashTable <CIMObjectPath, Boolean, EqualFunc <CIMObjectPath>,
        HashFunc <CIMObjectPath> > _subscriptionTable;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_NormalizedSubscriptionTable_h */

