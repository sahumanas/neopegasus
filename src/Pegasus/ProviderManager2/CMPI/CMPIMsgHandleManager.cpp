//%2008////////////////////////////////////////////////////////////////////////
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
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/ArrayRep.h>
#include <Pegasus/Common/ArrayIterator.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIMsgHandleManager.h>
#include <Pegasus/Common/Tracer.h>


PEGASUS_NAMESPACE_BEGIN



CMPIMsgHandleManager* CMPIMsgHandleManager::_handleManagerInstance = NULL;
ReadWriteSem CMPIMsgHandleManager::_rwsemHandleTable;


CMPIMsgHandleManager* CMPIMsgHandleManager::getCMPIMsgHandleManager(void)
{
    if (_handleManagerInstance == NULL)
    {
        WriteLock writeLock(_rwsemHandleTable);
        if (_handleManagerInstance == NULL)
        {
            _handleManagerInstance = new CMPIMsgHandleManager();
        }
    }

    return _handleManagerInstance;
};


CMPIMsgHandleManager::~CMPIMsgHandleManager()
{
    // Nothing to do here so far, since all data is on stack.
}

CMPIMsgHandleManager::CMPIMsgHandleManager()
{
    handleTable.reserveCapacity(10);
}


CMPIMsgFileHandle
CMPIMsgHandleManager::getNewHandle(MessageLoaderParms* data)
{
    CMPIMsgFileHandle newHandle = NULL;

    WriteLock writeLock(_rwsemHandleTable);

    ArrayIterator<MessageLoaderParms*> iterator(handleTable);

    Uint32 lastEntry = iterator.size();
    Uint32 freeEntry;

    // Look for the next free entry
    for (freeEntry=0; freeEntry < lastEntry; freeEntry++)
    {
        if (iterator[freeEntry] == NULL)
        {
            break;
        }
    }

    // Check if we need to append a new entry
    if (freeEntry == lastEntry)
    {
        handleTable.append(data);
    }
    else
    {
        iterator[freeEntry]=data;
    }

    // The array index is the actual handle.
    // Just need to convert it to CMPIMsgFileHandle
#ifdef PEGASUS_POINTER_64BIT
    Uint64 cnvHandle = (Uint64)freeEntry;
    newHandle = (CMPIMsgFileHandle)cnvHandle;
#else
    newHandle = (CMPIMsgFileHandle)freeEntry;
#endif

    return newHandle;
}

MessageLoaderParms*
CMPIMsgHandleManager::getDataForHandle(CMPIMsgFileHandle handle)
{
    Uint32 index;
    MessageLoaderParms* data;


#ifdef PEGASUS_POINTER_64BIT
    index = (Uint64)handle;
#else
    index = (Uint32)handle;
#endif

    ReadLock readLock(_rwsemHandleTable);
    data = handleTable[index];
    if (data == NULL)
    {
        throw IndexOutOfBoundsException();
    }

    return data;
}

MessageLoaderParms*
CMPIMsgHandleManager::releaseHandle(CMPIMsgFileHandle handle)
{
    Uint32 index;
    MessageLoaderParms* data;


#ifdef PEGASUS_POINTER_64BIT
    index = (Uint64)handle;
#else
    index = (Uint32)handle;
#endif

    WriteLock writeLock(_rwsemHandleTable);

    data = handleTable[index];
    if (data == NULL)
    {
        throw IndexOutOfBoundsException();
    }
    handleTable[index] = NULL;

    return data;
}

PEGASUS_NAMESPACE_END

