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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Exception_h
#define Pegasus_Exception_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/Array.h>

#ifdef PEGASUS_INTERNALONLY
# include <Pegasus/Common/MessageLoader.h>
#endif

PEGASUS_NAMESPACE_BEGIN

class ExceptionRep;
class CIMInstance;
class CIMConstInstance;


/**
    The Exception class is the base class for exceptions generated by the
    Pegasus infrastructure.
*/
class PEGASUS_COMMON_LINKAGE Exception
{
public:

    /**
        Constructs an Exception with a specified error message.
        @param message A message String containing a description of the error
    */
    Exception(const String& message);

    /**
        Constructs an Exception by copying another Exception object.
        @param exception The Exception object to copy
    */
    Exception(const Exception& exception);

#ifdef PEGASUS_INTERNALONLY
    Exception(const MessageLoaderParms& msgParms);
#endif

    /**
        Destructs an Exception object.
    */
    virtual ~Exception();

    /**
        Assigns the value from a specified Exception object.
        @param exception The Exception object to copy
        @return A reference to this Exception object with the new assignment
    */
    Exception& operator=(const Exception& exception);

    /**
        Gets the error description message.
        @return A message String containing a description of the error
    */
    virtual const String& getMessage() const;

#ifdef PEGASUS_INTERNALONLY
    // Note: Not virtual to preserve binary compatibility.
    const ContentLanguageList& getContentLanguages() const;

    // Not virtual to preserve binary compatibility.
    void setContentLanguages(const ContentLanguageList& langs);
#endif

protected:

    /**
        The default constructor is not available for the Exception class.
    */
    Exception();

    /**
        An internal representation of the Exception attributes.
    */
    ExceptionRep * _rep;
};

/**
    An IndexOutOfBoundsException occurs when a resource is requested at a
    specified index and no resource exists at that index.
*/
class PEGASUS_COMMON_LINKAGE IndexOutOfBoundsException : public Exception
{
public:
    /**
        Constructs an IndexOutOfBoundsException with a default error message.
    */
    IndexOutOfBoundsException();
};

/**
    An AlreadyExistsException occurs on an attempt to create or attach a
    resource for which an equivalent resource already exists.
*/
class PEGASUS_COMMON_LINKAGE AlreadyExistsException : public Exception
{
public:
    /**
        Constructs an AlreadyExistsException with a specified error message.
        @param message A message String containing a description of the error
    */
    AlreadyExistsException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    AlreadyExistsException(MessageLoaderParms& msgParms);
#endif
};

/**
    An InvalidNameException occurs on an attempt to create a name object with
    an invalid format.
*/
class PEGASUS_COMMON_LINKAGE InvalidNameException : public Exception
{
public:
    /**
        Constructs an InvalidNameException for a specified name.
        @param name A String containing the invalid name
    */
    InvalidNameException(const String& name);

#ifdef PEGASUS_INTERNALONLY
    InvalidNameException(MessageLoaderParms& msgParms);
#endif
};

/**
    An InvalidNamespaceNameException occurs on an attempt to create a
    namespace name object with an invalid format.
*/
class PEGASUS_COMMON_LINKAGE InvalidNamespaceNameException : public Exception
{
public:
    /**
        Constructs an InvalidNamespaceNameException for a specified namespace
        name.
        @param name A String containing the invalid namespace name
    */
    InvalidNamespaceNameException(const String& name);

#ifdef PEGASUS_INTERNALONLY
    InvalidNamespaceNameException(MessageLoaderParms& msgParms);
#endif
};

/**
    An UninitializedObjectException occurs on an attempt to use an object which
    has not been initialized.
*/
class PEGASUS_COMMON_LINKAGE UninitializedObjectException : public Exception
{
public:
    /**
        Constructs an UninitializedObjectException with a default error message.
    */
    UninitializedObjectException();
};

/**
    A TypeMismatchException occurs when values with incompatible types are used
    together.
*/
class PEGASUS_COMMON_LINKAGE TypeMismatchException : public Exception
{
public:
    /**
        Constructs a TypeMismatchException with a default error message.
    */
    TypeMismatchException();

    /**
        Constructs a TypeMismatchException with a specified error message.
        @param message A message String containing a description of the error
    */
    TypeMismatchException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    TypeMismatchException(MessageLoaderParms& msgParms);
#endif

};

/**
    A DynamicCastFailedException occurs when a polymorphic object is used as
    an incorrect derived type.
*/
class PEGASUS_COMMON_LINKAGE DynamicCastFailedException : public Exception
{
public:
    /**
        Constructs a DynamicCastFailedException with a default error message.
    */
    DynamicCastFailedException();
};

/**
    An InvalidDateTimeFormatException occurs when a DateTime string has an
    incorrect format.
*/
class PEGASUS_COMMON_LINKAGE InvalidDateTimeFormatException : public Exception
{
public:
    /**
        Constructs an InvalidDateTimeFormatException with a default error
        message.
    */
    InvalidDateTimeFormatException();
};

/**
    A DateTimeOutOfRangeException occurs when a DateTime value cannot be
    represented in the standard format.
*/
class PEGASUS_COMMON_LINKAGE DateTimeOutOfRangeException : public Exception
{
public:
    /**
        Constructs a DateTimeOutOfRangeException with a specified error
        message.
        @param message A message String containing a description of the error
    */
    DateTimeOutOfRangeException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    DateTimeOutOfRangeException(MessageLoaderParms& msgParms);
#endif
};

/**
    A MalformedObjectNameException occurs when an object name string has an
    incorrect format.
*/
class PEGASUS_COMMON_LINKAGE MalformedObjectNameException : public Exception
{
public:
    /**
        Constructs a MalformedObjectNameException for a specified object name.
        @param objectName A String containing the invalid object name
    */
    MalformedObjectNameException(const String& objectName);

#ifdef PEGASUS_INTERNALONLY
    MalformedObjectNameException(MessageLoaderParms& msgParms);
#endif
};

/**
    A BindFailedException occurs when a resource (such as an indication
    listen socket) is not bound successfully.
*/
class PEGASUS_COMMON_LINKAGE BindFailedException : public Exception
{
public:
    /**
        Constructs a BindFailedException with a specified error message.
        @param message A message String containing a description of the error
    */
    BindFailedException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    BindFailedException(MessageLoaderParms& msgParms);
#endif
};

/**
    An InvalidLocatorException occurs when a resource address string has an
    incorrect format.
*/
class PEGASUS_COMMON_LINKAGE InvalidLocatorException : public Exception
{
public:
    /**
        Constructs an InvalidLocatorException for a specified locator string.
        @param locator A String containing the invalid locator
    */
    InvalidLocatorException(const String& locator);

#ifdef PEGASUS_INTERNALONLY
    InvalidLocatorException(MessageLoaderParms& msgParms);
#endif
};

/**
    A CannotCreateSocketException occurs when a socket creation fails.
*/
class PEGASUS_COMMON_LINKAGE CannotCreateSocketException : public Exception
{
public:
    /**
        Constructs a CannotCreateSocketException with a default error message.
    */
    CannotCreateSocketException();
};

/**
    A CannotConnectException occurs when a connection attempt fails.
*/
class PEGASUS_COMMON_LINKAGE CannotConnectException : public Exception
{
public:
    /**
        Constructs a CannotConnectException with a specified error message.
        @param message A message String containing a description of the error
    */
    CannotConnectException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    CannotConnectException(MessageLoaderParms& msgParms);
#endif
};

/**
    An AlreadyConnectedException occurs on an attempt to connect a resource
    that is already connected.
*/
class PEGASUS_COMMON_LINKAGE AlreadyConnectedException: public Exception
{
public:
    /**
        Constructs an AlreadyConnectedException with a default error message.
    */
    AlreadyConnectedException();
};

/**
    A NotConnectedException occurs on an attempt to use a resource that
    requires a connection and is not connected.
*/
class PEGASUS_COMMON_LINKAGE NotConnectedException: public Exception
{
public:
    /**
        Constructs a NotConnectedException with a default error message.
    */
    NotConnectedException();
};

/**
    A ConnectionTimeoutException occurs when a timeout expires while waiting
    for a response on a connection.
*/
class PEGASUS_COMMON_LINKAGE ConnectionTimeoutException: public Exception
{
public:
    /**
        Constructs a ConnectionTimeoutException with a default error message.
    */
    ConnectionTimeoutException();
};

// EXP_PULL_BEGIN

class PEGASUS_COMMON_LINKAGE InvalidEnumerationContextException:
    public Exception
{
public:
    ///
    InvalidEnumerationContextException();
};

// EXP_PULL_END

/**
    An SSLException indicates an error with an SSL context, operation, or
    configuration.
*/
class PEGASUS_COMMON_LINKAGE SSLException: public Exception
{
public:
    /**
        Constructs an SSLException with a specified error message.
        @param message A message String containing a description of the error
    */
    SSLException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    SSLException(MessageLoaderParms& msgParms);
#endif
};

/**
    The CIMException class models an error response to a CIM operation.
*/
class PEGASUS_COMMON_LINKAGE CIMException : public Exception
{
public:

    /**
        Constructs a CIMException with a status code and error description
        message.
        @param code A CIMStatusCode containing a DMTF defined status code
            specifying the type of the error
        @param message A message String containing a more specific description
            of the error
    */
    CIMException(
        CIMStatusCode code = CIM_ERR_SUCCESS,
        const String& message = String::EMPTY);

    /**
        Constructs a CIMException with a status code, error description
        message, and a CIM_Error instance attached to the exception.
        @param code A CIMStatusCode containing a DMTF defined status code
            specifying the type of the error
        @param message A message String containing a more specific description
            of the error
        @param instance A CIMInstance containing the CIM_Error instance to be
            added to the CIMException.  This instance is NOT validated for
            type correctness nor correlation with the specified status code
            and error description.
    */
    CIMException(
        CIMStatusCode code,
        const String& message,
        const CIMInstance& instance);

    /**
        Constructs a CIMException with a status code, error description
        message, and an Array of CIM_Error instances attached to the exception.
        @param code A CIMStatusCode containing a DMTF defined status code
            specifying the type of the error
        @param message A message String containing a more specific description
            of the error
        @param instances An Array of CIMInstance objects containing the
            CIM_Error instances to be added to the CIMException.  These
            instances are NOT validated for type correctness nor correlation
            with the specified status code and error description.
    */
    CIMException(
        CIMStatusCode code,
        const String& message,
        const Array<CIMInstance>& instances);

#ifdef PEGASUS_INTERNALONLY
    CIMException(
        CIMStatusCode code,
        const MessageLoaderParms& msgParms);

    CIMException(
        CIMStatusCode code,
        const MessageLoaderParms& msgParms,
        const CIMInstance& instance);
    CIMException(
        CIMStatusCode code,
        const MessageLoaderParms& msgParms,
        const Array<CIMInstance>& instances);
#endif

    /**
        Constructs a CIMException by copying another CIMException object.
        @param cimException The CIMException object to copy
    */
    CIMException(const CIMException& cimException);

    /**
        Assigns the value from a specified CIMException object.
        @param cimException The CIMException object to copy
        @return A reference to this CIMException object with the new assignment
    */
    CIMException& operator=(const CIMException& cimException);

    /**
        Destructs a CIMException object.
    */
    virtual ~CIMException();

    /**
        Gets the CIMStatusCode for the current CIMException. This is the
        code that defines the ERROR that was executed and transmitted
        to the Client.
        @return a single CIMStatusCode
    */
    CIMStatusCode getCode() const;

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

    /**
        Gets the count of CIM_Error instances attached to the CIMException.
        Normally this method is used by a client to determine if any
        CIM_Error instances are attached to a CIMException.
        @return Uint32 count of CIM_Error instances attached to the
            CIMException.
    */
    Uint32 getErrorCount() const;

    /**
        Gets a single CIM_Error instance that is attached to the CIMException.
        Example:
        <pre>
            try
            {
                ... Execute CIM Operation
            }
            catch (CIMException& e)
            {
                for (Uint32 i = 0 ; i < e.getErrorCount() ; i++)
                    ... process e.getError(i)
            }
        </pre>
        @param index A Uint32 index specifying the index of the CIM_Error
            instance to get from the CIMException.  The index must be less than
            the getErrorCount() return value.
        @return A CIMConstInstance containing the CIM_Error instance at the
            specified index.
        @exception IndexOutOfRangeException If the index is greater or equal to
            the number of CIM_Error instances attached to the CIMExcepton.
    */

    CIMConstInstance getError(Uint32 index) const;

    /**
        Adds a CIM_Error instance to a CIMException.
        @param instance A CIMInstance containing the CIM_Error instance to be
            added to the CIMException.  This instance is NOT validated for
            type correctness nor correlation with the status code and error
            description already set in the CIMException.
    */
    void addError(const CIMInstance& instance);

#endif /* PEGASUS_USE_EXPERIMENTAL_INTERFACES */
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_Exception_h */
