//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


// 
// interface definition for the cimmofRepository class, a specialiazation
// of the Pegasus CIMRepository class with error handling.
//

#ifndef _CIMMOFREPOSITORY_H_
#define _CIMMOFREPOSITORY_H_

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Compiler/cimmofParser.h>

PEGASUS_USING_PEGASUS;

class PEGASUS_COMPILER_LINKAGE cimmofRepository : public CIMRepository {
 public:
  enum operationType {IGNORE_REPOSITORY, 
		      DO_NOT_ADD_TO_REPOSITORY,
		      USE_REPOSITORY};
  cimmofRepository(const String &path);
  ~cimmofRepository();

  // Add some methods for use at the compiler level
  virtual int addClass(CIMClass *classdecl);
  virtual int addInstance(CIMInstance *instance);
  virtual int addQualifier(CIMQualifierDecl *qualifier);

  virtual CIMQualifierDecl getQualifierDecl(const String &name);

};

#endif
