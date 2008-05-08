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

#include "Network.h"
#include "Config.h"
#include <iostream>
#include "Constants.h"
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/Network.h>
#include "Socket.h"
#include "TLS.h"
#include "HTTPConnector.h"
#include "HTTPConnection.h"
#include "HostAddress.h"
#include <Pegasus/Common/StringConversion.h>

#ifdef PEGASUS_OS_PASE
# include <as400_protos.h>
# include <Pegasus/Common/PaseCcsid.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

static Boolean _MakeAddress(
    const char* hostname,
    int port,
    sockaddr_in& address,
    void **addrInfoRoot)
{
    if (!hostname)
        return false;

#ifdef PEGASUS_ENABLE_IPV6
    struct sockaddr_in6 serveraddr;
    struct addrinfo  hints;
    struct addrinfo *result;

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family   = AF_UNSPEC ;
    hints.ai_socktype = SOCK_STREAM;

    // Giving hint as AI_NUMERICHOST to the getaddrinfo function avoids
    // name resolution done by getaddrinfo at DNS. This will improve
    // performance.

    // Check for valid IPv4 address.
    if (1 == HostAddress::convertTextToBinary(AF_INET, hostname, &serveraddr))
    {
        hints.ai_family = AF_INET;
        hints.ai_flags |= AI_NUMERICHOST;
    } // check for valid IPv6 address
    else if (1 == HostAddress::convertTextToBinary(AF_INET6, hostname,
         &serveraddr))
    {
        hints.ai_family = AF_INET6;
        hints.ai_flags |= AI_NUMERICHOST;
    }

    char scratch[22];
    Uint32 n;    
    const char * portStr = Uint32ToString(scratch, port, n);
    if (System::getAddrInfo(hostname, portStr, &hints, &result))
    {
        return false;
    }
    *addrInfoRoot = result;
#else

////////////////////////////////////////////////////////////////////////////////
// This code used to check if the first character of "hostname" was alphabetic
// to indicate hostname instead of IP address. But RFC 1123, section 2.1,
// relaxed this requirement to alphabetic character *or* digit. So bug 1462
// changed the flow here to call inet_addr first to check for a valid IP
// address in dotted decimal notation. If it's not a valid IP address, then
// try to validate it as a hostname.
// RFC 1123 states: The host SHOULD check the string syntactically for a
// dotted-decimal number before looking it up in the Domain Name System.
// Hence the call to inet_addr() first.
////////////////////////////////////////////////////////////////////////////////

    unsigned long tmp_addr = inet_addr((char *)hostname);
    struct hostent* hostEntry;

// Note: 0xFFFFFFFF is actually a valid IP address (255.255.255.255).
//       A better solution would be to use inet_aton() or equivalent, as
//       inet_addr() is now considered "obsolete".

    if (tmp_addr == 0xFFFFFFFF)  // if hostname is not an IP address
    {
        char hostEntryBuffer[8192];
        struct hostent hostEntryStruct;
        hostEntry = System::getHostByName(hostname, 
            &hostEntryStruct, 
            (char*) &hostEntryBuffer, 
            sizeof (hostEntryBuffer));

        if (!hostEntry)
        {
            return false;
        }

        memset(&address, 0, sizeof(address));
        memcpy(&address.sin_addr, hostEntry->h_addr, hostEntry->h_length);
        address.sin_family = hostEntry->h_addrtype;
        address.sin_port = htons(port);
    }
    else    // else hostname *is* a dotted-decimal IP address
    {
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = tmp_addr;
        address.sin_port = htons(port);
    }
#endif // PEGASUS_ENABLE_IPV6

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnectorRep
//
////////////////////////////////////////////////////////////////////////////////

struct HTTPConnectorRep
{
    Array<HTTPConnection*> connections;
};

////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnector
//
////////////////////////////////////////////////////////////////////////////////

HTTPConnector::HTTPConnector(Monitor* monitor)
    : Base(PEGASUS_QUEUENAME_HTTPCONNECTOR),
      _monitor(monitor), _entry_index(-1)
{
    _rep = new HTTPConnectorRep;
    Socket::initializeInterface();
}

HTTPConnector::~HTTPConnector()
{
    PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnector::~HTTPConnector()");
    delete _rep;
    Socket::uninitializeInterface();
    PEG_METHOD_EXIT();
}

void HTTPConnector::handleEnqueue(Message *message)
{
    if (!message)
        return;

    switch (message->getType())
    {
        // It might be useful to catch socket messages later to implement
        // asynchronous establishment of connections.

        case SOCKET_MESSAGE:
            break;

        case CLOSE_CONNECTION_MESSAGE:
        {
            CloseConnectionMessage* closeConnectionMessage =
                (CloseConnectionMessage*)message;

            for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
            {
                HTTPConnection* connection = _rep->connections[i];
                SocketHandle socket = connection->getSocket();

                if (socket == closeConnectionMessage->socket)
                {
                    _monitor->unsolicitSocketMessages(socket);
                    _rep->connections.remove(i);
                    delete connection;
                    break;
                }
            }
        }

        default:
            // ATTN: need unexpected message error!
            break;
    }

    delete message;
}


void HTTPConnector::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
        return;

    handleEnqueue(message);
}

HTTPConnection* HTTPConnector::connect(
    const String& host,
    const Uint32 portNumber,
    SSLContext * sslContext,
    Uint32 timeoutMilliseconds,
    MessageQueue* outputMessageQueue)
{
    PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnector::connect()");

#ifdef PEGASUS_OS_PASE
    // PASE needs ccsid 819 to perform socket operation 
    int orig_ccsid;
    orig_ccsid = _SETCCSID(-1);
    if (orig_ccsid == -1)
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "HTTPConnector::connect() Can not get current PASE CCSID.");
        orig_ccsid = 1208;
    }
    PaseCcsid ccsid(819, orig_ccsid);
#endif

    SocketHandle socket = PEGASUS_INVALID_SOCKET;
    // Use an AutoPtr to ensure the socket handle is closed on exception
    AutoPtr<SocketHandle, CloseSocketHandle> socketPtr(&socket);

#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
    if (0 == host.size())
    {
        // Set up the domain socket for a local connection

        sockaddr_un address;

        memset(&address, 0, sizeof(address));
        address.sun_family = AF_UNIX;
        strcpy(address.sun_path, PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);

        socket = Socket::createSocket(AF_UNIX, SOCK_STREAM, 0);
        if (socket == PEGASUS_INVALID_SOCKET)
        {
            PEG_METHOD_EXIT();
            throw CannotCreateSocketException();
        }
            
        Socket::disableBlocking(socket);

        // Connect the socket to the address:

        if (!Socket::timedConnect(
                socket,
                reinterpret_cast<sockaddr*>(&address),
                sizeof(address),
                timeoutMilliseconds))
        {
            MessageLoaderParms parms(
                "Common.HTTPConnector.CONNECTION_FAILED_LOCAL_CIM_SERVER",
                "Cannot connect to local CIM server. Connection failed.");
            PEG_METHOD_EXIT();
            throw CannotConnectException(parms);
        }
    }
    else
#endif
    {
        // Set up the IP socket connection

        // Make the internet address:
#ifdef PEGASUS_ENABLE_IPV6
        struct addrinfo *addrInfo, *addrInfoRoot;
#endif
        sockaddr_in address;
        if (!_MakeAddress((const char*)host.getCString(), portNumber, address,
#ifdef PEGASUS_ENABLE_IPV6
             (void**)(void*)&addrInfoRoot
#else
             0
#endif
             ))        
        {
            char scratch[22];
            Uint32 n;    
            const char * portStr = Uint32ToString(scratch, portNumber, n);
            PEG_METHOD_EXIT();
            throw InvalidLocatorException(host+":"+String(portStr,n));
        }

#ifdef PEGASUS_ENABLE_IPV6
        addrInfo = addrInfoRoot;
        while (addrInfo)
        {   
            // Create the socket:
            socket = Socket::createSocket(addrInfo->ai_family, 
                addrInfo->ai_socktype, addrInfo->ai_protocol);
#else
            socket = Socket::createSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
            if (socket == PEGASUS_INVALID_SOCKET)
            {
#ifdef PEGASUS_ENABLE_IPV6
                freeaddrinfo(addrInfoRoot);
#endif
                PEG_METHOD_EXIT();
                throw CannotCreateSocketException();
            }

#ifndef PEGASUS_OS_TYPE_WINDOWS
            // We need to ensure that the socket number is not higher than
            // what fits into FD_SETSIZE,because we else won't be able to select
            // on it and won't ever communicate correct on that socket.
            if (socket >= FD_SETSIZE)
            {
# ifdef PEGASUS_ENABLE_IPV6
                freeaddrinfo(addrInfoRoot);
# endif
                // the socket is useless to us, close it
                Socket::close(socket);
        
                PEG_TRACE(
                    (TRC_DISCARDED_DATA,
                     Tracer::LEVEL2,
                     "createSocket() returned too large socket number %d."
                         "Cannot connect to %s:%d. Connection failed.",
                     socket,
                     (const char*) host.getCString(),
                     portNumber));

                PEG_METHOD_EXIT();
                throw CannotCreateSocketException();
    }
#endif

            Socket::disableBlocking(socket);

            // Connect the socket to the address:
            if (!Socket::timedConnect(
                    socket,
#ifdef PEGASUS_ENABLE_IPV6
                    reinterpret_cast<sockaddr*>(addrInfo->ai_addr),
                    addrInfo->ai_addrlen,
#else
                    reinterpret_cast<sockaddr*>(&address),
                    sizeof(address),
#endif
                    timeoutMilliseconds))
            {
#ifdef PEGASUS_ENABLE_IPV6
                addrInfo = addrInfo->ai_next;
                if (addrInfo)
                {
                    Socket::close(socket);
                    continue;
                }
#endif
                char scratch[22];
                Uint32 n;    
                const char * portStr = Uint32ToString(scratch, portNumber, n);
                MessageLoaderParms parms(
                    "Common.HTTPConnector.CONNECTION_FAILED_TO",
                    "Cannot connect to $0:$1. Connection failed.",
                    host,
                    portStr);
#ifdef PEGASUS_ENABLE_IPV6
                freeaddrinfo(addrInfoRoot);
#endif
                PEG_METHOD_EXIT();
                throw CannotConnectException(parms);
            }
#ifdef PEGASUS_ENABLE_IPV6
            break;
        }
        freeaddrinfo(addrInfoRoot);
#endif
    }

    // Create HTTPConnection object:

    SharedPtr<MP_Socket> mp_socket(new MP_Socket(socket, sslContext, 0));
    // mp_socket now has responsibility for closing the socket handle
    socketPtr.release();

    if (mp_socket->connect(timeoutMilliseconds) < 0)
    {
        char scratch[22];
        Uint32 n;    
        const char * portStr = Uint32ToString(scratch, portNumber, n);
        MessageLoaderParms parms(
            "Common.HTTPConnector.CONNECTION_FAILED_TO",
            "Cannot connect to $0:$1. Connection failed.",
            host,
            portStr);
        PEG_METHOD_EXIT();
        throw CannotConnectException(parms);
    }

    AutoPtr<HTTPConnection> connection(new HTTPConnection(
        _monitor,
        mp_socket,
        String::EMPTY,
        this,
        static_cast<MessageQueueService *>(outputMessageQueue)));

    // Solicit events on this new connection's socket:

    if (-1 == (_entry_index = _monitor->solicitSocketMessages(
            connection->getSocket(),
            SocketMessage::READ | SocketMessage::EXCEPTION,
            connection->getQueueId(), MonitorEntry::TYPE_CONNECTOR)))
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "HTTPConnector::connect: Attempt to allocate entry in "
                "_entries table failed.");
        (connection->getMPSocket()).close();
    }

    _rep->connections.append(connection.get());
    PEG_METHOD_EXIT();
    return connection.release();
}

void HTTPConnector::destroyConnections()
{
    // For each connection created by this object:

    for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
    {
        _deleteConnection(_rep->connections[i]);
    }

    _rep->connections.clear();
}


void HTTPConnector::disconnect(HTTPConnection* currentConnection)
{
    //
    // find and delete the specified connection
    //

    Uint32 index = PEG_NOT_FOUND;
    for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
    {
        if (currentConnection == _rep->connections[i])
        {
            index = i;
            break;
        }
     }

    PEGASUS_ASSERT(index != PEG_NOT_FOUND);

    SocketHandle socket = currentConnection->getSocket();
    _monitor->unsolicitSocketMessages(socket);
    _rep->connections.remove(index);
    delete currentConnection;
}

void HTTPConnector::_deleteConnection(HTTPConnection* httpConnection)
{
    SocketHandle socket = httpConnection->getSocket();

    // Unsolicit SocketMessages:

    _monitor->unsolicitSocketMessages(socket);

    // Destroy the connection (causing it to close):

    delete httpConnection;
}

PEGASUS_NAMESPACE_END
