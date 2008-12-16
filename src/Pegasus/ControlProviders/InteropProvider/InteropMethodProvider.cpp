//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include "InteropProvider.h"
#include "InteropProviderUtils.h"
#include "InteropConstants.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

/*
    ATTN: This method provider is used by ProviderRegistrationProvider to
    update InteropProvider Cache. If SLP is enabled we invoke SLPProvider's
    'update' method to update registrations in this method. Note that
    'updateCache' method is not defined as part of class
    PG_ProviderProfileCapabilities. This method is used purely for internal
    purpose for the communication between InteropProvider and
    ProviderRegistrationProvider.
*/

void InteropProvider::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & objectReference,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParameters,
    MethodResultResponseHandler & handler)
{
    if(objectReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP)
        && objectReference.getClassName().equal(
            PEGASUS_CLASSNAME_PG_PROVIDERPROFILECAPABILITIES)
        && methodName.equal("updateCache"))
    {
        handler.processing();
#ifdef PEGASUS_ENABLE_SLP
        sendUpdateRegMessageToSLPProvider(context);
#endif
        updateProfileCache++;
        handler.complete();
    }
    else
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
            objectReference.getNameSpace().getString());
    }
}

PEGASUS_NAMESPACE_END

