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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//         Warren Otsuka (warren_otsuka@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cctype>
#include <cassert>
#include <cstdlib>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Logger.h>
#include "Handler.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define D(X) // X
//#define D(X) X

Boolean Handler::_messageTrace = false;
Boolean Handler::_messageLogTrace = false;

Handler::Handler()
{
    D( cout << "Handler::Handler()" << endl; )
}

Handler::~Handler()
{
    D( cout << "Handler::~Handler()" << endl; )
}

Boolean Handler::handleOpen(Channel* channel)
{
    D( cout << "Handler::handleOpen()" << endl; )

    channel->disableBlocking();
    clear();
    return true;
}

void Handler::clear()
{
    D( cout << "Handler::clear()" << endl; )

    _state = WAITING;
    _message.clear();
    _lines.clear();
    _contentOffset = 0;
    _contentLength = 0;
}

void Handler::skipWhitespace(const char*& p)
{
    while (isspace(*p))
        p++;
}

const char* Handler::getFieldValue(const char* fieldName) const
{
    const char* message = _message.getData();
    Uint32 fieldNameLength = strlen(fieldName);

    for (Uint32 i = 1; i < _lines.size(); i++)
    {
        const char* line = &message[_lines[i]];

        if (String::compareNoCase(line, fieldName, fieldNameLength) == 0 &&
            line[fieldNameLength] == ':')
        {
            const char* p = line + fieldNameLength + 1;

            while (isspace(*p))
                p++;

            return p;
        }
    }

    return 0;
}

const char* Handler::getFieldValueSubString(const char* str) const
{
    const char* message = _message.getData();

    for (Uint32 i = 1; i < _lines.size(); i++)
    {
        const char* line = &message[_lines[i]];

        if (strstr(line, str))
        {
            const char* p = strchr(line, ':');

            if (!p)
                return 0;

            p++;

            while (isspace(*p))
                p++;

            return p;
        }
    }

    return 0;
}

const char* Handler::getContent() const
{
    return _message.getData() + _contentOffset;
}

Uint32 Handler::getContentLength() const
{
    const char* p = getFieldValue("Content-Length");

    if (!p)        return PEG_NOT_FOUND;

    return atoi(p);
}

void Handler::print() const
{
    // Print out the header:

    const char* message = _message.getData();

    for (Uint32 i = 0; i < _lines.size(); i++)
    {
        cout << &message[_lines[i]] << "\r\n";
    }

    cout << "\r\n";

    // Print out the content:

    const char* content = _message.getData() + _contentOffset;
    const char* end = content + _contentLength;

    ((Array<Sint8>&)_message).append('\0');
    XmlWriter::indentedPrint(cout, content);
    //    ((Array<Sint8>&)_message).remove(_message.size() - 1);
}

/** Handler::log generates a log entry for the message.
   It depends on the message being defined in -message
*/
void Handler::log() const
{
    ostrstream os;
    const char* message = _message.getData();
    
    // log header
    for (Uint32 i = 0; i < _lines.size(); i++)
    {
        os << &message[_lines[i]] << "\r\n";
    }
    os << "\r\n";

    // Log message
    const char* content = _message.getData() + _contentOffset;
    const char* end = content + _contentLength;

    ((Array<Sint8>&)_message).append('\0');
    XmlWriter::indentedPrint(os, content);
    ((Array<Sint8>&)_message).remove(_message.size() - 1);
    os.put('\0');
    char* tmp = os.str();

    Logger::put(Logger::TRACE_LOG, "Handler",Logger::INFORMATION,
       "RCVD================\n $0", tmp);
}

/** Handler::log generates a log entry for the message.
   It depends on the message being defined in -message.
   This is a generalized message logger so that the specific logs
   and supplementary information are provided with the call.
   @param logFileType
   @param subsystem
   @param severity
   @param msg text message
   @parm message The pointer to the message to be logged
*/
/* Temporarily disabled during test
void Handler::logMessage(LogFileType logFileType, String& subsystem, Uint32 
                severity, String& formatMsg) const
{
    ostrstream os;
    const char* message = _message.getData();
    
    // log header
    for (Uint32 i = 0; i < _lines.size(); i++)
    {
        os << &message[_lines[i]] << "\r\n";
    }
    os << "\r\n";

    // Log message
    const char* content = _message.getData() + _contentOffset;
    const char* end = content + _contentLength;

    ((Array<Sint8>&)_message).append('\0');
    XmlWriter::indentedPrint(os, content);
    ((Array<Sint8>&)_message).remove(_message.size() - 1);
    os.put('\0');
    char* tmp = os.str();

    // add $0 to string input.
    Logger::put(logFileType, subsystem,severity,
               formatMsg, tmp);

}
************/

/** Test for HTTP terminator characters.  The normal sequence is
    /r/n/r/n as a sequence.
    Note we extended this to test for two LF (/n/n) since a number of
    browsers and other client tools use this.
*/
char* Handler::FindTerminator(const char* data, Uint32 size)
{
    const char* p = data;
    const char* end = p + size;

    while (p != end)
    {
        if (*p == '\r')
        {
            Uint32 n = end - p;

            if (n >= 4 && p[1] == '\n' && p[2] == '\r' && p[3] == '\n')
                return (char*)p;
        }

        // Test for two lf in series.  This is not what is specified in
        // the standard but we found several clients use this termination
        if (*p == '\n')
        {
            Uint32 n = end - p;

            if (n >= 2 && p[1] == '\n')
                return (char*)p;
        }

        p++;
    }

    return 0;
}

Boolean _IsBodylessMessage(const char* line)
{
    //ATTN: Make sure this is the right place to check for HTTP/1.1 and
    //      HTTP/1.0 that is part of the authentication challenge header.
    // ATTN-RK-P2-20020305: How do we make sure we have the complete list?
    const char* METHOD_NAMES[] =
    {
        "GET",
        "HTTP/1.1 400",
        "HTTP/1.0 400",
        "HTTP/1.1 401",
        "HTTP/1.0 401",
        "HTTP/1.1 501",
        "HTTP/1.0 501"
    };

    const Uint32 METHOD_NAMES_SIZE = sizeof(METHOD_NAMES) / sizeof(char*);

    for (Uint32 i = 0; i < METHOD_NAMES_SIZE; i++)
    {
        Uint32 n = strlen(METHOD_NAMES[i]);

        if (strncmp(line, METHOD_NAMES[i], n) == 0 && isspace(line[n]))
            return true;
    }

    return false;
}

Boolean Handler::handleInput(Channel* channel)
{
    D( cout << "Handler::handleInput()" << endl; )

    //--------------------------------------------------------------------------
    // If in the waiting state, set start state to header state.
    //--------------------------------------------------------------------------

    if (_state == WAITING)
        _state = LINES;

    //--------------------------------------------------------------------------
    // Read all incoming data (non-blocking):
    //--------------------------------------------------------------------------

    char buffer[4096];
    Uint32 totalBytesRead = 0;
    int bytesRead;

    while ((bytesRead = channel->read(buffer, sizeof(buffer))) > 0)
    {
        _message.append(buffer, bytesRead);
        totalBytesRead += bytesRead;
    }

    if (totalBytesRead == 0)
    {
        // We check here for end of message body because some
        // implementations signal the end of the message by closing
        // the connection rather than using a Content-Length field.
        // If the content length field was encountered, then the handler
        // has already been called and the state already set back to
        // waiting.

        if (_state == CONTENT)
        {
            _state = DONE;
            _contentLength = _message.size() - _contentOffset;
            D( cout << "handleMessage(); closed connection" << endl; )

            if (handleMessage() != 0)
                return false;

            clear();
            _state = WAITING;
        }

        return false;
    }

    //--------------------------------------------------------------------------
    // Process what we have so far:
    //--------------------------------------------------------------------------

    switch (_state)
    {
        case LINES:
        {
            char* m = (char*)_message.getData();
            Uint32 mSize = _message.size();
            char* term = FindTerminator(m, mSize);

            if (term)
            {
                *term = '\0';

                for (char* p = strtok(m, "\r\n"); p; p = strtok(NULL, "\r\n"))
                    _lines.append(p - m);

                _contentOffset = (term + 4) - m;
                _contentLength = 0;

                // If this is a bodyless operation, go straight to done state

                if( _IsBodylessMessage( m ) )
                {
                    _state = DONE;
                    D( cout << "handleMessage(); bodyless message" << endl; )
                      D( cout << "handleMessage(); _message ==\n" << _message.getData() << endl; )

                    if (handleMessage() != 0)
                        return false;

                    _state = WAITING;
                    break;
                }

                // Now we move into content fetching state:

                _state = CONTENT;

                // Attempt to extract the content-length (if it returns
                // -1, then the content is finished when the connection
                // is closed.

                _contentLength = getContentLength();

                // Fall through to next case!
            }
        }

        case CONTENT:
        {
            Uint32 currentContentLength = _message.size() - _contentOffset;

            if (_contentLength != PEG_NOT_FOUND
                && currentContentLength == _contentLength)
            {
                _state = DONE;

                D(cout << "handleMessage(); content-length" << endl;)

                // Null terminate the content:

                _message.append('\0');

                if (handleMessage() != 0)
                    return false;

                // clear();
                _state = WAITING;
            }

            break;
        }

        default:
            assert(0);
            break;
    }

    return true;
}

Boolean Handler::handleOutput(Channel* channel)
{
    return true;
}

void Handler::handleClose(Channel* channel)
{
    D( cout << "Handler::handleClose()" << endl; )
}

int Handler::handleMessage()
{
    D( cout << "Handler::handleMessage()" << endl; )
      ((Array <Sint8>&) _message).append ('\0');
    if (_messageTrace)
    {
        cout << "========== RECEIVED ==========" << endl;
        print();
    }

    // printMessage(cout, _message);

    if (_messageLogTrace)
        log();

    //log(Logger::TRACE_LOG, "Handler",Logger::INFORMATION, 
    //          "RCVD================\n $0");

    return 0;
}

void Handler::printMessage(ostream& os, const Array<Sint8>& message)
{
    // Find separator between HTTP headers and content:

    const char* p = message.getData();
    const char* separator = 0;

    for (; *p; p++)
    {
        if (p[0] == '\r' && p[1] == '\n' && p[2] == '\r' && p[3] == '\n')
        {
            p += 4;
            separator = p - 1;
            break;
        }
    }

    if (!separator)
        return;

    *((char*)separator) = '\0';

    // Print HTTP Headers:

    os << message.getData() << endl;

    // Print the body:

    ((Array<Sint8>&)message).append('\0');

    XmlWriter::indentedPrint(os, p);

    *((char*)separator) = '\n';
    ((Array<Sint8>&)message).remove(message.size() - 1);
}

PEGASUS_NAMESPACE_END
