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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusRepository_Repository_Decl_Context_h
#define PegasusRepository_Repository_Decl_Context_h

#include <Pegasus/Common/DeclContext.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

class CIMRepository;  // forward declaration for repository member

////////////////////////////////////////////////////////////////////////////////
//
// RepositoryDeclContext
//
//	This context is used by the resolve() methods to lookup dependent
//	objects during resolution.
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_REPOSITORY_LINKAGE RepositoryDeclContext : public DeclContext
{
public:

    RepositoryDeclContext(CIMRepository* repository);

    virtual ~RepositoryDeclContext();

    virtual CIMQualifierDecl lookupQualifierDecl(
	const String& nameSpace, 
	const String& qualifierName) const;

    virtual CIMClass lookupClass(
	const String& nameSpace, 
	const String& className) const;

protected:

    CIMRepository* _repository;
};

PEGASUS_NAMESPACE_END

#endif
