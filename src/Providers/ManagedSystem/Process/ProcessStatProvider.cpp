//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//=============================================================================
//
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By:
//         David Kennedy       <dkennedy@linuxcare.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//         Al Stone, Hewlett-Packard Company <ahs3@fc.hp.com>
//         Jim Metcalfe, Hewlett-Packard Company
//         Carlos Bonilla, Hewlett-Packard Company
//         Mike Glantz, Hewlett-Packard Company <michael_glantz@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////


/* ==========================================================================
   Includes.
   ========================================================================== */

#include "ProcessStatProvider.h"


// ==========================================================================
// Class names.  These values are the names of the classes that
// are common for all of the providers.
//
// We use CIM_UnitaryComputerSystem as the value of the key
// CSCreationClassName, because this class has properties that
// are important for clients of this provider
// ==========================================================================

#define CLASS_CIM_UNITARY_COMPUTER_SYSTEM "CIM_UnitaryComputerSystem"
#define CLASS_CIM_OPERATING_SYSTEM        "CIM_OperatingSystem"

// Use PG_UnixProcess* until DMTF finalizes CIM_UnixProcess*
#define CLASS_UNIX_PROCESS_STAT           "PG_UnixProcessStatisticalInformation"
#define CLASS_UNIX_PROCESS                "PG_UnixProcess"

/* ==========================================================================
   The number of keys for the classes.
   ========================================================================== */

#define NUMKEYS_UNIX_PROCESS_STAT                    7

/* ==========================================================================
   Property names.  These values are returned by the provider as
   the property names.
   ========================================================================== */

// inherited from ManagedElement

#define PROPERTY_CAPTION                     "Caption"
#define PROPERTY_DESCRIPTION                 "Description"

// Keys

#define PROPERTY_CS_CREATION_CLASS_NAME      "CSCreationClassName"
#define PROPERTY_CS_NAME                     "CSName"
#define PROPERTY_OS_CREATION_CLASS_NAME      "OSCreationClassName"
#define PROPERTY_OS_NAME                     "OSName"
#define PROPERTY_PROCESS_CREATION_CLASS_NAME "ProcessCreationClassName"
#define PROPERTY_HANDLE                      "Handle"
#define PROPERTY_NAME                        "Name"

// Local properties

#define PROPERTY_CPU_TIME                    "CPUTime"
#define PROPERTY_REAL_TEXT                   "RealText"
#define PROPERTY_REAL_DATA                   "RealData"
#define PROPERTY_REAL_STACK                  "RealStack"
#define PROPERTY_VIRTUAL_TEXT                "VirtualText"
#define PROPERTY_VIRTUAL_DATA                "VirtualData"
#define PROPERTY_VIRTUAL_STACK               "VirtualStack"
#define PROPERTY_VIRTUAL_MEMORY_MAPPED_FILESIZE  "VirtualMemoryMappedFileSize"
#define PROPERTY_VIRTUAL_SHARED_MEMORY       "VirtualSharedMemory"
#define PROPERTY_CPU_TIME_DEAD_CHILDREN      "CpuTimeDeadChildren"
#define PROPERTY_SYSTEM_TIME_DEAD_CHILDREN   "SystemTimeDeadChildren"
#define PROPERTY_REAL_SPACE                  "RealSpace"


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


ProcessStatProvider::ProcessStatProvider()
{
  // cout << "ProcessStatProvider::ProcessStatProvider()" << endl;
}

ProcessStatProvider::~ProcessStatProvider()
{
  // cout << "ProcessStatProvider::~ProcessStatProvider()" << endl;
}

/*
================================================================================
NAME              : createInstance
DESCRIPTION       : Create a UnixProcessStatisticalInformation instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::createInstance(const OperationContext       &context,
                    const CIMReference           &instanceName,
                    const CIMInstance            &instanceObject,
                    ResponseHandler<CIMReference> &handler)

{
  // There is no useful meaning to creating an instance
  // of this class
  throw NotSupported(String::EMPTY);
}

/*
================================================================================
NAME              : deleteInstance
DESCRIPTION       : Delete a UnixProcessStatisticalInformation instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::deleteInstance(const OperationContext       &context,
                    const CIMReference           &instanceReference,
                    ResponseHandler<CIMInstance> &handler)

{ 
  // There is no useful meaning to deleting an instance
  // of this class
  throw NotSupported(String::EMPTY);
}

/*
================================================================================
NAME              : enumerateInstances
DESCRIPTION       : Enumerates all UnixProcessStatisticalInformation instances.
                    An array of instance references is returned.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : LocalOnly, DeepInheritance and propertyList are not
                  : respected by this provider. Localization is not supported
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::enumerateInstances(
	const OperationContext & context,
	const CIMReference & classReference,
	const Uint32 flags,
	const CIMPropertyList & propertyList,
	ResponseHandler<CIMInstance> & handler)
{   
  // cout << "ProcessStatProvider::enumerateInstances()" << endl;

  Process _p;
  int pIndex;
  String className;

  /* Validate the classname.  */
  className = classReference.getClassName();
  _checkClass(className);

  /* Notify processing is starting. */
  handler.processing();

  // Get the process information and construct and deliver
  // an instance for each process
  // HP-UX NOTE: loadProcessInfo will change the value of pIndex
  // in order to skip over unused entries in the process table
  // Do not depend on this counter incrementing sequentially!
  for (pIndex = 0; _p.loadProcessInfo(pIndex); pIndex++)
    handler.deliver(_constructInstance(CLASS_UNIX_PROCESS_STAT, _p));

  /* Notify processing is complete. */
  handler.complete();

  return;
}  /* enumerateInstances */


/*
================================================================================
NAME              : enumerateInstanceNames
DESCRIPTION       : Enumerates all UnixProcessStatisticalInformation instance names.
                    An array of instance references is returned.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Localization is not supported
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::enumerateInstanceNames(const OperationContext &ctx,
                            const CIMReference &ref,
                            ResponseHandler<CIMReference> &handler)
{
  // cout << "ProcessStatProvider::enumerateInstanceNames()" << endl;

  int pIndex;
  Process _p;
  String className;
    
  /* Validate the classname.  */
  className = ref.getClassName();
  _checkClass(className);

    String csName = _getCSName();
    String osName = _getOSName();
    String ns = ref.getNameSpace();


  /* Notify processing is starting. */
  handler.processing();

  /* Get the process information and construct the key bindings for */
  /* each process.                                                  */
  // Note that loadProcessInfo modifies pIndex to point to the
  // next process structure
  for (pIndex = 0; _p.loadProcessInfo(pIndex); pIndex++)
  {
    Array<KeyBinding> keyBindings;

    // Construct the key bindings
    keyBindings.append(KeyBinding(PROPERTY_CS_CREATION_CLASS_NAME,
                                  CLASS_CIM_UNITARY_COMPUTER_SYSTEM,
                                  KeyBinding::STRING));
		
    keyBindings.append(KeyBinding(PROPERTY_CS_NAME,
                                  csName,
                                  KeyBinding::STRING));
    
    keyBindings.append(KeyBinding(PROPERTY_OS_CREATION_CLASS_NAME,
                                  CLASS_CIM_OPERATING_SYSTEM,
                                  KeyBinding::STRING));
		
    keyBindings.append(KeyBinding(PROPERTY_OS_NAME,
                                  osName,
                                  KeyBinding::STRING));

    keyBindings.append(KeyBinding(PROPERTY_PROCESS_CREATION_CLASS_NAME,
                                  CLASS_UNIX_PROCESS,
                                  KeyBinding::STRING));

    keyBindings.append(KeyBinding(PROPERTY_HANDLE,
                                  _p.getHandle(),
                                  KeyBinding::STRING));

    // We return a time stamp to uniquely identify this
    // instance of UnixProcessStatisticalInformation
    keyBindings.append(KeyBinding(PROPERTY_NAME,
                                  _p.getCurrentTime(),
                                  KeyBinding::STRING));

    // Deliver the names
    handler.deliver(CIMReference(String::EMPTY, // hostname
                                 ns,
                                 CLASS_UNIX_PROCESS_STAT,
                                 keyBindings));

  } // for

  /* Notify processing is complete. */
  handler.complete();

  return;
}  /* enumerateInstanceNames */

/*
================================================================================
NAME              : getInstance
DESCRIPTION       : Returns a single instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : LocalOnly, DeepInheritance and propertyList are not
                  : respected by this provider. Localization is not supported
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::getInstance(const OperationContext &ctx,
                 const CIMReference           &instanceName,
                 const Uint32                  flags,
                 const CIMPropertyList        &propertyList,
                 ResponseHandler<CIMInstance> &handler)
{	
  // cout << "ProcessStatProvider::getInstance()" << endl;

  KeyBinding kb;
  String className;
  String handle;
  int i;
  int keysFound; // this will be used as a bit array
  Process _p;

  // Validate the classname
  className = instanceName.getClassName();
  _checkClass(className);

  // Extract the key values
  Array<KeyBinding> kbArray = instanceName.getKeyBindings();

  // Leave immediately if wrong number of keys
  if ( kbArray.size() != NUMKEYS_UNIX_PROCESS_STAT )
    throw InvalidParameter("Wrong number of keys");

  // Validate the keys.
  // Each loop iteration will set a bit in keysFound when a valid
  // key is found. If the expected bits aren't all set when
  // the loop finishes, it's a problem
  for(i=0, keysFound=0; i<NUMKEYS_UNIX_PROCESS_STAT; i++)
  {
    kb = kbArray[i];

    String keyName = kb.getName();
    String keyValue = kb.getValue();

    // CSCreationClassName can be empty or must match
    if (String::equalNoCase(keyName, PROPERTY_CS_CREATION_CLASS_NAME))
    {
      if (String::equal(keyValue, String::EMPTY) ||
          String::equalNoCase(keyValue, CLASS_CIM_UNITARY_COMPUTER_SYSTEM))
        keysFound |= 1;
      else
        throw InvalidParameter(keyValue+": bad value for key "+keyName);
    }
	
    // CSName can be empty or must match
    else if (String::equalNoCase(keyName, PROPERTY_CS_NAME))
    {
      if (String::equal(keyValue, String::EMPTY) ||
          String::equalNoCase(keyValue, _getCSName()))
        keysFound |= 2;
      else
        throw InvalidParameter(keyValue+": bad value for key "+keyName);
    }

    // OSCreationClassName can be empty or must match
    else if (String::equalNoCase(keyName,PROPERTY_OS_CREATION_CLASS_NAME))
    {
      if (String::equal(keyValue, String::EMPTY) ||
	  String::equalNoCase(keyValue, CLASS_CIM_OPERATING_SYSTEM))
        keysFound |= 4;
      else
        throw InvalidParameter(keyValue+": bad value for key "+keyName);
    }

    // OSName can be empty or must match
    else if (String::equalNoCase(keyName, PROPERTY_OS_NAME))
    {
      if (String::equal(keyValue, String::EMPTY) ||
	  String::equalNoCase(keyValue, _getOSName()))
        keysFound |= 8;
      else
        throw InvalidParameter(keyValue+": bad value for key "+keyName);
    }

    // CreationClassName can be empty or must match
    else if (String::equalNoCase(keyName, PROPERTY_PROCESS_CREATION_CLASS_NAME))
    {
      if (String::equal(keyValue, String::EMPTY) ||
	  String::equalNoCase(keyValue, CLASS_UNIX_PROCESS))
        keysFound |= 16;
      else
        throw InvalidParameter(keyValue+": bad value for key "+keyName);
    }

    // Handle must be a valid pid, but we will know that later
    // For now, just verify that it's present
    else if (String::equalNoCase(keyName, PROPERTY_HANDLE))
    {
      handle = keyValue;
      keysFound |= 32;
    }

    // Name must be present, but we don't care what it is
    else if (String::equalNoCase(keyName, PROPERTY_NAME))
      keysFound |= 64;

    // Key name was not recognized by any of the above tests
    else throw InvalidParameter(keyName+ ": Unrecognized key");
		
  } /* for */

  // We could get here if we didn't get all the keys, which
  // could happen if the right number of keys were supplied,
  // and they all had valid names and values, but there were
  // any duplicates (e.g., two Handles, no OSName)
  if(keysFound != (1<<NUMKEYS_UNIX_PROCESS_STAT)-1)
    throw InvalidParameter("Bad object name");
	    
  /* Find the instance.  First convert the instance id which is the */
  /* process handle to an integer.  This is necessary because the   */
  /* handle is the process id on HP-UX which must be passed to      */
  /* pstat_getproc() as an integer.                                 */

  /* Get the process information. */
  if (_p.findProcess(handle))
  {
    /* Notify processing is starting. */
    handler.processing();

    /* Return the instance. */
    handler.deliver(_constructInstance(CLASS_UNIX_PROCESS_STAT,_p));

    /* Notify processing is complete. */
    handler.complete();
    return;
  }

  throw ObjectNotFound(handle+": No such process");

  return; // can never execute, but required to keep compiler happy
}

/*
================================================================================
NAME              : modifyInstance
DESCRIPTION       : Modify a UnixProcessStatisticalInformation instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::modifyInstance(const OperationContext       &context,
                    const CIMReference           &instanceName,
                    const CIMInstance            &instanceObject,
		    const Uint32                 flags,
		    const CIMPropertyList        &propertyList,
                    ResponseHandler<CIMInstance> &handler)
{
  // There is no useful meaning for this operation
  // on this class
  throw NotSupported(String::EMPTY);
}

/*
================================================================================
NAME              : initialize
DESCRIPTION       : Initializes the provider.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::initialize(CIMOMHandle &ch)
{
  // cout << "ProcessStatProvider::initialize()" << endl;
  _cimomHandle = ch;

  // call platform-specific routine to get values
  Process _p;
  _hostName = _p.getCSName();
  _osName = _p.getOSName();

  return;  
}  /* initialize */

/*
================================================================================
NAME              : terminate
DESCRIPTION       : Terminates the provider.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::terminate()
{
  // cout << "ProcessStatProvider::terminate()" << endl;
}


// ================================================================================
// NAME              : _constructInstance
// DESCRIPTION       : Constructs instance by adding its properties. The
//                   : Process instance argument has already been filled in
//                   : with data from an existing process
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   :
// NOTES             : 
// PARAMETERS        : className, Process
// ================================================================================

CIMInstance ProcessStatProvider::_constructInstance(const String &className,
                                         const Process &_p) 
{
  String s;
  Uint32 i32;
  Uint64 i64;
  CIMDateTime d;

  CIMInstance inst(className);

// CIM_ManagedElement

//   string Caption
  if (_p.getCaption(s))
    inst.addProperty(CIMProperty(PROPERTY_CAPTION,s));

//   string Description
  if (_p.getDescription(s))
    inst.addProperty(CIMProperty(PROPERTY_DESCRIPTION,s));

// =================================================
// Keys defined in UnixProcessStatisticalInformation
// =================================================

//   [ key ] string CSCreationClassName
  inst.addProperty(CIMProperty(PROPERTY_CS_CREATION_CLASS_NAME,
                               CLASS_CIM_UNITARY_COMPUTER_SYSTEM));

//   [ key ] string CSName
  inst.addProperty(CIMProperty(PROPERTY_CS_NAME, _getCSName()));

//   [ key ] string OSCreationClassName
  inst.addProperty(CIMProperty(PROPERTY_OS_CREATION_CLASS_NAME,
                               CLASS_CIM_OPERATING_SYSTEM));

//   [ key ] string OSName
  inst.addProperty(CIMProperty(PROPERTY_OS_NAME, _getOSName()));

//   [ key ] string ProcessCreationClassName
  inst.addProperty(CIMProperty(PROPERTY_PROCESS_CREATION_CLASS_NAME,
                               CLASS_UNIX_PROCESS));

//   [ key ] string Handle
  inst.addProperty(CIMProperty(PROPERTY_HANDLE,_p.getHandle()));

//   [ key ] string Name: return a time stamp for client to
//   be able to distinguish different samples of this data
//   Note that this key will be ignored by getInstance()
  inst.addProperty(CIMProperty(PROPERTY_NAME, _p.getCurrentTime()));

// ===============================================
// Properties in UnixProcessStatisticalInformation
// ===============================================

//    uint32 CPUTime
  if (_p.getCPUTime(i32))
    inst.addProperty(CIMProperty(PROPERTY_CPU_TIME,i32));

//    uint64 RealText
  if (_p.getRealText(i64))
    inst.addProperty(CIMProperty(PROPERTY_REAL_TEXT,i64));

//    uint64 RealData
  if (_p.getRealData(i64))
    inst.addProperty(CIMProperty(PROPERTY_REAL_DATA,i64));

//    uint64 RealStack
  if (_p.getRealStack(i64))
    inst.addProperty(CIMProperty(PROPERTY_REAL_STACK,i64));

//    uint64 VirtualText
  if (_p.getVirtualText(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_TEXT,i64));

//    uint64 VirtualData
  if (_p.getVirtualData(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_DATA,i64));

//    uint64 VirtualStack
  if (_p.getVirtualStack(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_STACK,i64));

//    uint64 VirtualMemoryMappedFileSize
  if (_p.getVirtualMemoryMappedFileSize(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_MEMORY_MAPPED_FILESIZE,i64));

//    uint64 VirtualSharedMemory
  if (_p.getVirtualSharedMemory(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_SHARED_MEMORY,i64));

//    uint64 CpuTimeDeadChildren
  if (_p.getCpuTimeDeadChildren(i64))
    inst.addProperty(CIMProperty(PROPERTY_CPU_TIME_DEAD_CHILDREN,i64));

//    uint64 SystemTimeDeadChildren
  if (_p.getSystemTimeDeadChildren(i64))
    inst.addProperty(CIMProperty(PROPERTY_SYSTEM_TIME_DEAD_CHILDREN,i64));

//    uint64 RealSpace
  if (_p.getRealSpace(i64))
    inst.addProperty(CIMProperty(PROPERTY_REAL_SPACE,i64));
 
  return inst;
}


/*
================================================================================
NAME              : _getOSName
DESCRIPTION       : Call uname() and get the operating system name.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
String &ProcessStatProvider::_getOSName()
{
  // This routine could do something dynamic; for nooow
  // it returns a string that was captured when the
  // initialize() method was invoked
  return _osName;
}


/*
================================================================================
NAME              : _getCSName
DESCRIPTION       : return the hostname.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             :
================================================================================
*/
String &ProcessStatProvider::_getCSName()
{
  // This routine could do something dynamic; for nooow
  // it returns a string that was captured when the
  // initialize() method was invoked
  return _hostName;
}


/*
================================================================================
NAME              : _checkClass
DESCRIPTION       : tests the argument for valid classname,
                  : throws exception if not
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             :
================================================================================
*/
void ProcessStatProvider::_checkClass(String& className)
{
  if (!String::equalNoCase(className, CLASS_UNIX_PROCESS_STAT))
    throw NotSupported(className+": Class not supported");
}
