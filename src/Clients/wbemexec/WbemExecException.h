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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//         (carolann_graves@hp.com)
//
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//         (sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WbemExecException_h
#define Pegasus_WbemExecException_h

#include <Clients/cliutils/CommandException.h>

PEGASUS_NAMESPACE_BEGIN

/** 
  
    WbemExecException signals that an exception has occurred in executing the 
    wbemexec command.  
  
    @author  Hewlett-Packard Company
  
 */
class WbemExecException : public CommandException 
{
public:
    /**
      
        Constructs a WbemExecException with a message corresponding to the
        specified exception ID.
      
        @param  ID                the integer exception identifier
      
     */
    WbemExecException (Uint32 ID);

    /**
      
        Constructs a WbemExecException with a message corresponding to the
        specified ID, appended with the specified String.
      
        @param  ID                the integer exception identifier
        @param  appendString      the string to append to the exception message
      
     */
    WbemExecException (Uint32 ID, const String& appendString);

    /**
      
        Constructs a WbemExecException with the specified message.
      
        @param  exceptionMessage  a string containing the exception message
      
     */
    WbemExecException (const String& exceptionMessage);

    //
    //  Exception identifiers
    //
    //  These are used to index into the messageStrings array to retrieve the 
    //  appropriate exception message string.  When a new identifier is added, 
    //  the messageStrings array and MAX_ID must be updated appropriately.
    //
    
    /**
      
        Default exception identifier.  This identifier is used if the specified
        identifier is out of range.  This identifier corresponds to a default
        (generic) message in the array of exception message strings.
      
     */
    static const Uint32 DEFAULT_ID;
    
    /**
      
        Minimum valid exception identifier.
      
     */
    static const Uint32 MIN_ID;
    
    /**
      
        Exception identifier indicating the specified input file does not
        exist.
      
     */
    static const Uint32 MPOST_HTTP10_INVALID;
    
    /**
      
        Exception identifier indicating the specified input file does not
        exist.
      
     */
    static const Uint32 INPUT_FILE_NONEXISTENT;
    
    /**
      
        Exception identifier indicating the specified input file cannot be
        read.
      
     */
    static const Uint32 INPUT_FILE_NOT_READABLE;
    
    /**
      
        Exception identifier indicating the specified input file cannot be
        opened.
      
     */
    static const Uint32 INPUT_FILE_CANNOT_OPEN;
    
    /**
      
        Exception identifier indicating that no input was provided.
      
     */
    static const Uint32 NO_INPUT;
    
    /**
      
        Exception identifier indicating failure to make an HTTP connection.
      
     */
    static const Uint32 CONNECT_FAIL;
    
    /**
      
        Exception identifier indicating timed out waiting for response.
      
     */
    static const Uint32 TIMED_OUT;
    
    /**
      
        Exception identifier indicating invalid XML input.
      
     */
    static const Uint32 INVALID_XML;
    
    /**
      
        Exception identifier indicating invalid input.
      
     */
    static const Uint32 INVALID_INPUT;
    
    /**
       
       Exception identifier indicating malloc error.
  
    */
    static const Uint32 MALLOC_ERROR;

    /**
       
       Exception identifier indicating internal error.
  
    */
    static const Uint32 INTERNAL_ERROR;

    /**
      
        Maximum valid exception identifier.  This value must be updated when
        a new exception identifier and message are added.
      
     */
    static const Uint32 MAX_ID;

private:
    /**
      
        Exception message strings.  The exception identifier is used as an
        index into this array to retrieve the appropriate exception message 
        string.  When a new identifier is added, this array must be updated
        appropriately.
      
     */
    static const char*  _messageStrings [];
};

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_WbemExecException_h */
