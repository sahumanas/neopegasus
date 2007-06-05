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


#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/AutoPtr.h>

#include "InternalCIMOMHandleRep.h"

PEGASUS_NAMESPACE_BEGIN

InternalCIMOMHandleMessageQueue::InternalCIMOMHandleMessageQueue()
    : MessageQueue(PEGASUS_QUEUENAME_INTERNALCLIENT),
    _output_qid(0),
    _return_qid(0),
    _response(0)
{
    // output queue is the binary message handler
    MessageQueue* out = MessageQueue::lookup(PEGASUS_QUEUENAME_BINARY_HANDLER);

    PEGASUS_ASSERT(out != 0);

    _output_qid = out->getQueueId();

    // input queue is this
    _return_qid = getQueueId();
}

InternalCIMOMHandleMessageQueue::~InternalCIMOMHandleMessageQueue()
{
    try
    {
        // ATTN: release any unprocessed responses
        _response.clear();
    }
    catch (...)
    {
    }
}

void InternalCIMOMHandleMessageQueue::handleEnqueue()
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleMessageQueue::handleEnqueue");

    Message* message = dequeue();

    switch(message->getType())
    {
    /*
    case CIM_GET_CLASS_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_CLASS_REQUEST_MESSAGE:
    case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
    case CIM_DELETE_CLASS_REQUEST_MESSAGE:
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
    case CIM_REFERENCES_REQUEST_MESSAGE:
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
        sendRequest(message);

        break;
    */
    case CIM_GET_CLASS_RESPONSE_MESSAGE:
    case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
    case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
    case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
    case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
    case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
    case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
    case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
    case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
    case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
    case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
    case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
    case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
    case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
    case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
    case CIM_REFERENCES_RESPONSE_MESSAGE:
    case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
    case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
    case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
    case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
        _response.enqueue(message);

        break;
    default:
        PEG_TRACE_CSTRING(
            TRC_DISCARDED_DATA,
            Tracer::LEVEL2,
            "Error: unexpected message type");

        delete message;

        break;
    }

    PEG_METHOD_EXIT();
}

CIMResponseMessage* InternalCIMOMHandleMessageQueue::sendRequest(
    CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::sendRequest");

    AutoMutex autoMutex(_mutex);

    // update message to include routing information
    request->dest = _output_qid;
    request->queueIds.push(_return_qid);
    request->queueIds.push(_output_qid);

    // locate destination
    MessageQueueService* service =
        dynamic_cast<MessageQueueService *>(
            MessageQueue::lookup(_output_qid));

    PEGASUS_ASSERT(service != 0);

    // forward request
    if (service->SendForget(request) == false)
    {
        PEG_METHOD_EXIT();
        throw Exception("Failed to send message");
    }

    // wait for response
    CIMResponseMessage* response =
        dynamic_cast<CIMResponseMessage *>(_response.dequeue_wait());

    PEG_METHOD_EXIT();
    return response;
}

static void _deleteContentLanguage(void* data)
{
   if (data != 0)
   {
       ContentLanguageList* cl = static_cast<ContentLanguageList*>(data);

       delete cl;
   }
}

static OperationContext _filterOperationContext(const OperationContext& context)
{
    OperationContext temp;

    if (context.contains(IdentityContainer::NAME))
    {
        // propagate the identity container if it exists (get() with throw
        // an exception if it does not)
        temp.insert(context.get(IdentityContainer::NAME));
    }
    else
    {
        temp.insert(IdentityContainer(String::EMPTY));
    }

    if (context.contains(AcceptLanguageListContainer::NAME))
    {
        // propagate the accept languages container if it exists
        // (get() with throw an exception if it does not exist)
        temp.insert(context.get(AcceptLanguageListContainer::NAME));
    }
    else
    {
        // If the container is not found then try to use the
        // AcceptLanguageList from the current thread
        AcceptLanguageList* pal = Thread::getLanguages();

        if (pal != 0)
        {
            temp.insert(AcceptLanguageListContainer(*pal));
        }
        else
        {
            temp.insert(AcceptLanguageListContainer(AcceptLanguageList()));
        }
    }

    if (context.contains(ContentLanguageListContainer::NAME))
    {
        // propagate the accept languages container if it exists
        // (get() with throw an exception if it does not)
        temp.insert(context.get(ContentLanguageListContainer::NAME));
    }
    else
    {
        temp.insert(ContentLanguageListContainer(ContentLanguageList()));
    }

    return temp;
}

InternalCIMOMHandleRep::InternalCIMOMHandleRep()
{
}

InternalCIMOMHandleRep::~InternalCIMOMHandleRep()
{
}

CIMResponseMessage* InternalCIMOMHandleRep::do_request(
    CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::do_request");

    /*
    Uint32 timeout = 0;

    try
    {
        const TimeoutContainer* p = dynamic_cast<const TimeoutContainer*>(
            &(context.get(TimeoutContainer::NAME)));
        if (p)
        {
            timeout = p->getTimeOut();
        }
    }
    catch (Exception &)
    {
    }

    try
    {
        if (timeout)
        {
            _msg_avail.time_wait(timeout);
        }
        else
        {
            _msg_avail.wait();
        }
    }
    catch (TimeOut&)
    {
        PEG_TRACE_CSTRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                        "timeout waiting for response");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.EMPTY_CIM_RESPONSE",
            "Empty CIM Response"));
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Unexpected Exception");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.EMPTY_CIM_RESPONSE",
            "Empty CIM Response"));
    }
    */

    Message* temp = _queue.sendRequest(request);

    CIMResponseMessage* response = dynamic_cast<CIMResponseMessage*>(temp);

    if (response == 0)
    {
        delete response;

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.EMPTY_CIM_RESPONSE",
            "Empty CIM Response"));
    }

    if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMException e(response->cimException);

        delete response;

        PEG_METHOD_EXIT();
        throw e;
    }

    if (response->operationContext.contains(ContentLanguageListContainer::NAME))
    {
        // If the response has a Content-Language then save it into
        // thread-specific storage
        ContentLanguageListContainer container =
            response->operationContext.get(ContentLanguageListContainer::NAME);

        if (container.getLanguages().size() > 0)
        {
            Thread* currentThread = Thread::getCurrent();

            if (currentThread != 0)
            {
                // deletes the old tsd and creates a new one
                currentThread->put_tsd(
                    "cimomHandleContentLanguages",
                    _deleteContentLanguage,
                    sizeof(ContentLanguageList*),
                    new ContentLanguageList(container.getLanguages()));
            }
        }
    }

    PEG_METHOD_EXIT();
    return response;
}


//
// CIM Operations
//

CIMClass InternalCIMOMHandleRep::getClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::getClass");

    // encode request
    CIMGetClassRequestMessage* request =
        new CIMGetClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);
//  request->operationContext.get(AcceptLanguageListContainer::NAME);

    AutoPtr<CIMGetClassResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMGetClassResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    CIMClass cimClass = response->cimClass;

    PEG_METHOD_EXIT();
    return cimClass;
}


Array<CIMClass> InternalCIMOMHandleRep::enumerateClasses(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::enumerateClasses");

    CIMEnumerateClassesRequestMessage* request =
        new CIMEnumerateClassesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMEnumerateClassesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMEnumerateClassesResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMClass> cimClasses = response->cimClasses;

    PEG_METHOD_EXIT();
    return cimClasses;
}


Array<CIMName> InternalCIMOMHandleRep::enumerateClassNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::enumerateClassNames");

    CIMEnumerateClassNamesRequestMessage* request =
        new CIMEnumerateClassNamesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            deepInheritance,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMEnumerateClassNamesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMEnumerateClassNamesResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMName> cimClassNames = response->classNames;

    PEG_METHOD_EXIT();
    return cimClassNames;
}


void InternalCIMOMHandleRep::createClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::createClass");

    CIMCreateClassRequestMessage* request =
        new CIMCreateClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            newClass,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMCreateClassResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMCreateClassResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    PEG_METHOD_EXIT();
    return;
}


void InternalCIMOMHandleRep::modifyClass(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMClass& modifiedClass)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::modifyClass");

    CIMModifyClassRequestMessage* request =
        new CIMModifyClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            modifiedClass,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMModifyClassResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMModifyClassResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    PEG_METHOD_EXIT();
    return;
}


void InternalCIMOMHandleRep::deleteClass(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::deleteClass");

    // encode request
    CIMDeleteClassRequestMessage* request =
        new CIMDeleteClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMDeleteClassResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMDeleteClassResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    PEG_METHOD_EXIT();
    return;
}


CIMInstance InternalCIMOMHandleRep::getInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::getInstance");

    // encode request
    CIMGetInstanceRequestMessage* request =
        new CIMGetInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMGetInstanceResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMGetInstanceResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    CIMInstance cimInstance = response->cimInstance;

    PEG_METHOD_EXIT();
    return cimInstance;
}

Array<CIMInstance> InternalCIMOMHandleRep::enumerateInstances(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::enumerateInstances");

    // encode request
    CIMEnumerateInstancesRequestMessage* request =
        new CIMEnumerateInstancesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMEnumerateInstancesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMInstance> cimInstances = response->cimNamedInstances;

    PEG_METHOD_EXIT();
    return cimInstances;
}


Array<CIMObjectPath> InternalCIMOMHandleRep::enumerateInstanceNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::enumerateInstanceNames");

    // encode request
    CIMEnumerateInstanceNamesRequestMessage* request =
        new CIMEnumerateInstanceNamesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObjectPath> cimObjectPaths = response->instanceNames;

    PEG_METHOD_EXIT();
    return cimObjectPaths;
}

CIMObjectPath InternalCIMOMHandleRep::createInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMInstance& newInstance)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::createInstance");

    CIMCreateInstanceRequestMessage* request =
        new CIMCreateInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            newInstance,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMCreateInstanceResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMCreateInstanceResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    CIMObjectPath cimReference = response->instanceName;

    PEG_METHOD_EXIT();
    return cimReference;
}


void InternalCIMOMHandleRep::modifyInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::modifyInstance");

    CIMModifyInstanceRequestMessage* request =
        new CIMModifyInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            modifiedInstance,
            includeQualifiers,
            propertyList,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMModifyInstanceResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMModifyInstanceResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    PEG_METHOD_EXIT();
    return;
}


void InternalCIMOMHandleRep::deleteInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::deleteInstance");

    CIMDeleteInstanceRequestMessage* request =
        new CIMDeleteInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMDeleteInstanceResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMDeleteInstanceResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    PEG_METHOD_EXIT();
    return;
}


Array<CIMObject> InternalCIMOMHandleRep::execQuery(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const String& queryLanguage,
    const String& query)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::execQuery");

    CIMExecQueryRequestMessage* request =
        new CIMExecQueryRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            queryLanguage,
            query,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMExecQueryResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMExecQueryResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObject> cimObjects = response->cimObjects;

    PEG_METHOD_EXIT();
    return cimObjects;
}


Array<CIMObject> InternalCIMOMHandleRep::associators(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::associators");

    CIMAssociatorsRequestMessage* request =
        new CIMAssociatorsRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            objectName,
            assocClass,
            resultClass,
            role,
            resultRole,
            includeQualifiers,
            includeClassOrigin,
            propertyList,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMAssociatorsResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMAssociatorsResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObject> cimObjects = response->cimObjects;

    PEG_METHOD_EXIT();
    return cimObjects;
}


Array<CIMObjectPath> InternalCIMOMHandleRep::associatorNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::associatorNames");

    CIMAssociatorNamesRequestMessage* request =
        new CIMAssociatorNamesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            objectName,
            assocClass,
            resultClass,
            role,
            resultRole,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMAssociatorNamesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMAssociatorNamesResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObjectPath> cimObjectPaths = response->objectNames;

    PEG_METHOD_EXIT();
    return cimObjectPaths;
}


Array<CIMObject> InternalCIMOMHandleRep::references(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::references");

    CIMReferencesRequestMessage* request =
        new CIMReferencesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            objectName,
            resultClass,
            role,
            includeQualifiers,
            includeClassOrigin,
            propertyList,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMReferencesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMReferencesResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObject> cimObjects = response->cimObjects;

    PEG_METHOD_EXIT();
    return cimObjects;
}


Array<CIMObjectPath> InternalCIMOMHandleRep::referenceNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::referenceNames");

    CIMReferenceNamesRequestMessage* request =
        new CIMReferenceNamesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            objectName,
            resultClass,
            role,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMReferenceNamesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMReferenceNamesResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObjectPath> cimObjectPaths = response->objectNames;

    PEG_METHOD_EXIT();
    return cimObjectPaths;
}


CIMValue InternalCIMOMHandleRep::getProperty(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::getProperty");

    CIMGetPropertyRequestMessage* request =
        new CIMGetPropertyRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            propertyName,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMGetPropertyResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMGetPropertyResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    CIMValue cimValue = response->value;

    PEG_METHOD_EXIT();
    return cimValue;
}


void InternalCIMOMHandleRep::setProperty(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::setProperty");

    CIMSetPropertyRequestMessage* request =
        new CIMSetPropertyRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            propertyName,
            newValue,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMSetPropertyResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMSetPropertyResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    PEG_METHOD_EXIT();
    return;
}


CIMValue InternalCIMOMHandleRep::invokeMethod(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::invokeMethod");

    CIMInvokeMethodRequestMessage* request =
        new CIMInvokeMethodRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            methodName,
            inParameters,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMInvokeMethodResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMInvokeMethodResponseMessage*>(
            do_request(request)));

        if (response.get() == 0)
        {
            PEG_TRACE_CSTRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch (CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    CIMValue cimValue = response->retValue;
    outParameters = response->outParameters;

    PEG_METHOD_EXIT();
    return cimValue;
}


//
// Public CIMOMHandle Methods
//

void InternalCIMOMHandleRep::disallowProviderUnload()
{
    CIMOMHandleRep::disallowProviderUnload();
}

void InternalCIMOMHandleRep::allowProviderUnload()
{
    CIMOMHandleRep::allowProviderUnload();
}

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
OperationContext InternalCIMOMHandleRep::getResponseContext()
{
    OperationContext ctx;

    Thread* curThrd = Thread::getCurrent();
    if (curThrd == NULL)
    {
        ctx.insert(ContentLanguageListContainer(ContentLanguageList()));
    }
    else
    {
        ContentLanguageList* contentLangs = (ContentLanguageList*)
            curThrd->reference_tsd("cimomHandleContentLanguages");
        curThrd->dereference_tsd();

        if (contentLangs == NULL)
        {
            ctx.insert(ContentLanguageListContainer(ContentLanguageList()));
        }
        else
        {
            ctx.insert(ContentLanguageListContainer(*contentLangs));
            // delete the old tsd to free the memory
            curThrd->delete_tsd("cimomHandleContentLanguages");
        }
    }

    return ctx;
}
#endif

PEGASUS_NAMESPACE_END
