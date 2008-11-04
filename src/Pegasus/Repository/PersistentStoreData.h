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

#ifndef Pegasus_PersistentStoreData_h
#define Pegasus_PersistentStoreData_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>

PEGASUS_NAMESPACE_BEGIN

class ClassAssociation
{
public:

    ClassAssociation(
        const CIMName& assocClassName_,
        const CIMName& fromClassName_,
        const CIMName& fromPropertyName_,
        const CIMName& toClassName_,
        const CIMName& toPropertyName_)
        : assocClassName(assocClassName_),
          fromClassName(fromClassName_),
          fromPropertyName(fromPropertyName_),
          toClassName(toClassName_),
          toPropertyName(toPropertyName_)
    {
    }

    ClassAssociation()
    {
    }

    CIMName assocClassName;
    CIMName fromClassName;
    CIMName fromPropertyName;
    CIMName toClassName;
    CIMName toPropertyName;
};

class InstanceAssociation
{
public:

    InstanceAssociation(
        const String& assocInstanceName_,
        const CIMName& assocClassName_,
        const String& fromInstanceName_,
        const CIMName& fromClassName_,
        const CIMName& fromPropertyName_,
        const String& toInstanceName_,
        const CIMName& toClassName_,
        const CIMName& toPropertyName_)
        : assocInstanceName(assocInstanceName_),
          assocClassName(assocClassName_),
          fromInstanceName(fromInstanceName_),
          fromClassName(fromClassName_),
          fromPropertyName(fromPropertyName_),
          toInstanceName(toInstanceName_),
          toClassName(toClassName_),
          toPropertyName(toPropertyName_)
    {
    }

    String assocInstanceName;
    CIMName assocClassName;
    String fromInstanceName;
    CIMName fromClassName;
    CIMName fromPropertyName;
    String toInstanceName;
    CIMName toClassName;
    CIMName toPropertyName;
};

class NamespaceDefinition
{
public:

    NamespaceDefinition(const CIMNamespaceName& name_)
        : name(name_),
          shareable(false),
          updatesAllowed(true),
          parentNameSpace(),
          remoteInfo()
    {
    }

    CIMNamespaceName name;
    Boolean shareable;
    Boolean updatesAllowed;
    CIMNamespaceName parentNameSpace;
    String remoteInfo;    // Only used with Remote CMPI
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_PersistentStoreData_h */