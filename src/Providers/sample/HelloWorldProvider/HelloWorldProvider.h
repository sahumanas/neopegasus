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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HelloWorldProvider_h
#define Pegasus_HelloWorldProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>

PEGASUS_NAMESPACE_BEGIN

class HelloWorldProvider :
	public CIMInstanceProvider
	//public CIMIndicationProvider // Out of date with CIMIndicationProvider
{
public:
	HelloWorldProvider(void);
	virtual ~HelloWorldProvider(void);

	// CIMProvider interface
	virtual void initialize(CIMOMHandle & cimom);
	virtual void terminate(void);

	// CIMInstanceProvider interface
	virtual void getInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const Boolean includeQualifiers,
		const Boolean includeClassOrigin,
		const CIMPropertyList & propertyList,
		InstanceResponseHandler & handler);

	virtual void enumerateInstances(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const Boolean includeQualifiers,
		const Boolean includeClassOrigin,
		const CIMPropertyList & propertyList,
		InstanceResponseHandler & handler);

	virtual void enumerateInstanceNames(
		const OperationContext & context,
		const CIMObjectPath & ref,
		ObjectPathResponseHandler & handler);

	virtual void modifyInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const CIMInstance & obj,
		const Boolean includeQualifiers,
		const CIMPropertyList & propertyList,
		ResponseHandler & handler);

	virtual void createInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const CIMInstance & obj,
		ObjectPathResponseHandler & handler);

	virtual void deleteInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		ResponseHandler & handler);

	// CIMIndicationProvider interface
	virtual void provideIndication(
		const OperationContext & context,
		const CIMObjectPath & classReference,
		const CIMDateTime & minimumInterval,
		const CIMDateTime & maximumInterval,
		const CIMPropertyList & propertyList,
		InstanceResponseHandler & handler);

	virtual void updateIndication(
		const OperationContext & context,
		const CIMObjectPath & classReference,
		const CIMDateTime & minimumInterval,
		const CIMDateTime & maximumInterval,
		const CIMPropertyList & propertyList,
		InstanceResponseHandler & handler);

	virtual void cancelIndication(
		const OperationContext & context,
		const CIMObjectPath & classReference,
		InstanceResponseHandler & handler);

	virtual void checkIndication(
		const OperationContext & context,
		const CIMObjectPath & classReference,
		const CIMPropertyList & propertyList,
		InstanceResponseHandler & handler);

       virtual void enableIndication(
                const OperationContext & context,
                const String & nameSpace,
                const Array<String> & classNames,
                const CIMPropertyList & propertyList,
                const Uint16 repeatNotificationPolicy,
                const String & otherRepeatNotificationPolicy,
                const CIMDateTime & repeatNotificationInterval,
                const CIMDateTime & repeatNotificationGap,
                const Uint16 repeatNotificationCount,
                const String & condition,
                const String & queryLanguage,
                const CIMInstance & subscription,
                InstanceResponseHandler & handler);

        virtual void disableIndication(
                const OperationContext & context,
                const String & nameSpace,
                const Array<String> & classNames,
                const CIMInstance & subscription,
                InstanceResponseHandler & handler);

        virtual void modifyIndication(
                const OperationContext & context,
                const String & nameSpace,
                const Array<String> & classNames,
                const CIMPropertyList & propertyList,
                const Uint16 repeatNotificationPolicy,
                const String & otherRepeatNotificationPolicy,
                const CIMDateTime & repeatNotificationInterval,
                const CIMDateTime & repeatNotificationGap,
                const Uint16 repeatNotificationCount,
                const String & condition,
                const String & queryLanguage,
                const CIMInstance & subscription,
                InstanceResponseHandler & handler);


protected:
	Array<CIMObjectPath> _enumerateInstanceNames(
		const OperationContext & context,
		const CIMObjectPath & classReference);

protected:
	CIMOMHandle         _cimom;
	Array<CIMInstance>	_instances;

};

PEGASUS_NAMESPACE_END

#endif
