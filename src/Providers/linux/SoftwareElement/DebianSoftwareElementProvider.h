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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DebianSoftwareElementProvider_h
#define Pegasus_DebianSoftwareElementProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMInstance.h>
#include "SoftwareElementProvider.h"
#include "PackageInformation.h"

#include <iostream>

#define DEBIANCLASSNAME "Linux_Debian_SoftwareElement"
#define DEBIANPROVIDERNAME "LinuxSoftwareElementProvider"
#define DEFAULT_DEBIAN_DATABASE "/var/lib/dpkg"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define DEBUG(X) Logger::put(Logger::DEBUG_LOG, "Linux_Debian_SoftwareElementProvider", Logger::INFORMATION, "$0", X)

class DebianSoftwareElementProvider : public SoftwareElementProvider
{
   public:
      DebianSoftwareElementProvider();
      ~DebianSoftwareElementProvider();

      void getInstance(const OperationContext& context,
		       const CIMObjectPath& ref,
		       const Boolean includeQualifiers,
		       const Boolean includeClassOrigin,
		       const CIMPropertyList& propertyList,
		       InstanceResponseHandler& handler );

      void enumerateInstances(const OperationContext& context,
			      const CIMObjectPath& ref,
			      const Boolean includeQualifiers,
			      const Boolean includeClassOrigin,
			      const CIMPropertyList& propertyList,
			      InstanceResponseHandler& handler );

      void enumerateInstanceNames(const OperationContext& context,
				  const CIMObjectPath &ref,
				  ObjectPathResponseHandler& handler );

      void modifyInstance(const OperationContext& context,
			  const CIMObjectPath& ref,
			  const CIMInstance& instanceObject,
			  const Boolean includeQualifiers,
			  const CIMPropertyList& propertyList,
			  ResponseHandler& handler );

      void createInstance(const OperationContext& context,
			  const CIMObjectPath& ref,
			  const CIMInstance& instanceObject,
			  ObjectPathResponseHandler& handler );

      void deleteInstance(const OperationContext& context,
			  const CIMObjectPath& ref,
			  ResponseHandler& handler );

      void initialize(CIMOMHandle& handle);
      void terminate(void);

   protected:
      virtual CIMObjectPath fill_reference(const String& nameSpace, 
	    				  const String& className, 
					  const String& inDirectory,
				          const PackageInformation* ptr);

      virtual CIMInstance build_instance(const String& className, 
	    				 const PackageInformation* ptr);
};


PEGASUS_NAMESPACE_END

#endif
