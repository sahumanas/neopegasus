//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author: Michael E. Brasher
//
// $Log: WindowsChannel.cpp,v $
// Revision 1.1  2001/04/08 08:28:20  mike
// Added more windows channel implementation code.
//
//
//END_HISTORY

#include <iostream>
#include "WindowsChannel.h"
#include <winsock.h>

using namespace std;

PEGASUS_NAMESPACE_BEGIN

// ATTN-A: manage lifetime of all these objects. Do a walkthrough!

////////////////////////////////////////////////////////////////////////////////
//
// Routines for starting and stoping WinSock:
//
////////////////////////////////////////////////////////////////////////////////

static Uint32 _wsaCount = 0;

static void _WSAInc()
{
    if (_wsaCount == 0)
    {
	WSADATA tmp;

	if (WSAStartup(0x202, &tmp) == SOCKET_ERROR)
	    WSACleanup();
    }

    _wsaCount++;
}

static void _WSADec()
{
    _wsaCount--;

    if (_wsaCount == 0)
	WSACleanup();
}

////////////////////////////////////////////////////////////////////////////////
//
// WindowsChannel
//
////////////////////////////////////////////////////////////////////////////////

WindowsChannel::WindowsChannel(Uint32 desc, ChannelHandler* handler) 
    : _desc(desc), _blocking(true), _handler(handler)
{

}

WindowsChannel::~WindowsChannel()
{
    if (_desc != -1)
	::closesocket(_desc);

    if (_handler)
	delete _handler;
}

Sint32 WindowsChannel::read(void* ptr, Uint32 size)
{
    if (_desc == -1)
	return -1;

    return ::recv(_desc, (char*)ptr, size, 0);
}

Sint32 WindowsChannel::write(const void* ptr, Uint32 size)
{
    if (_desc == -1)
	return -1;

    Sint32 result = ::send(_desc, (const char*)ptr, size, 0);

    return result;
}

Sint32 WindowsChannel::readN(void* ptr, Uint32 size)
{
    // ATTN-A: need a timeout here!

    // Enable blocking temporarily:

    Boolean blocking = getBlocking();

    if (!blocking)
	enableBlocking();

    // Try to read size bytes:

    char* p = (char*)ptr;
    Uint32 r = size;
    Uint32 m = 0;

    while (r)
    {
	Sint32 n = read(p, r);

	if (n == -1)
	    return m;

	m += n;
	p += n;
	r -= n;
    }

    // Restore non-blocking if applicable:

    if (!blocking)
	disableBlocking();

    // Return number of bytes actually read:

    return m;
}

Sint32 WindowsChannel::writeN(const void* ptr, Uint32 size)
{
    // ATTN-A: need a timeout here!

    // Enable blocking temporarily:

    Boolean blocking = getBlocking();

    if (!blocking)
	enableBlocking();

    // Try to write size bytes:

    const char* p = (const char*)ptr;
    Uint32 r = size;
    Uint32 m = 0;

    while (r)
    {
	Sint32 n = write(p, r);

	if (n == -1)
	    return m;

	m += n;
	p += n;
	r -= n;
    }

    // Restore non-blocking if applicable:

    if (!blocking)
	disableBlocking();

    // Return number of bytes actually written:

    return m;
}

void WindowsChannel::enableBlocking()
{
    unsigned long flag = 0;
    ioctlsocket(_desc, FIONBIO, &flag);
    _blocking = true;
}

void WindowsChannel::disableBlocking()
{
    unsigned long flag = 1;
    ioctlsocket(_desc, FIONBIO, &flag);
    _blocking = false;
}

Boolean WindowsChannel::wouldBlock() const
{
    return GetLastError() == WSAEWOULDBLOCK;
}

Boolean WindowsChannel::handle(Sint32 desc, Uint32 reasons)
{
    if (desc != _desc)
	return false;

    if (reasons | Selector::READ)
    {
	if (!_handler->handleInput(this))
	{
	    _handler->handleClose(this);
	    return false;
	}
    }
    else if (reasons | Selector::WRITE)
    {
	if (!_handler->handleOutput(this))
	{
	    _handler->handleClose(this);
	    return false;
	}
    }

    // ATTN-A: what about Selector::EXCEPTION and handleException()?

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// WindowsChannelConnector
//
////////////////////////////////////////////////////////////////////////////////

static inline char* _Clone(const char* str)
{
    return strcpy(new char[strlen(str) + 1], str);
}

static Boolean _MakeAddress(
    const char* hostname, 
    int port, 
    sockaddr_in& address)
{
    if (!hostname)
	return false;
	
    struct hostent *entry;
	
    if (isalpha(hostname[0]))
	entry = gethostbyname(hostname);
    else
    {
	unsigned long tmp = inet_addr((char *)hostname);
	entry = gethostbyaddr((char *)&tmp, sizeof(tmp), AF_INET);
    }

    if (!entry)
	return false;

    memset(&address, 0, sizeof(address));
    memcpy(&address.sin_addr, entry->h_addr, entry->h_length);
    address.sin_family = entry->h_addrtype;
    address.sin_port = htons(port);

    return true;
}

static Boolean _ParseAddress(
    const char* address, 
    char*& hostname,
    int& port)
{
    if (!address)
	return false;

    // Extract the hostname:port expression (e.g., www.book.com:8080):

    hostname = _Clone(address);
    char* p = strchr(hostname, ':');

    if (!p)
    {
	delete [] hostname;
	return false;
    }

    *p++ = '\0';

    char* end = 0;
    port = strtol(p, &end, 10);

    if (!end || *end != '\0')
    {
	delete [] hostname;
	return false;
    }

    return true;
}

WindowsChannelConnector::WindowsChannelConnector(
    ChannelHandlerFactory* factory,
    Selector* selector)
    : ChannelConnector(factory), _selector(selector)
{
    _WSAInc();
}

WindowsChannelConnector::~WindowsChannelConnector()
{
    _WSADec();
}


Channel* WindowsChannelConnector::connect(const char* addressString)
{
    if (!_factory)
	return 0;

    // Parse the address:

    char* hostname;
    int port;

    if (!_ParseAddress(addressString, hostname, port))
	return 0;

    // Make the internet address:

    sockaddr_in address;

    if (!_MakeAddress(hostname, port, address))
    {
	delete [] hostname;
        return false;
    }

    // Create the socket:

    int desc = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (desc < 0)
	return 0;

    // Conect the socket to the address:

    if (::connect(desc, (sockaddr*)&address, sizeof(address)) < 0)
	return 0;

    // Create the channel:

    ChannelHandler* handler = _factory->create();

    WindowsChannel* channel = new WindowsChannel(desc, handler);

    // Register the channel to receive events:

    _selector->addHandler(
	desc, 
	Selector::READ | Selector::WRITE | Selector::EXCEPTION,
	channel);

    return channel;
}

void WindowsChannelConnector::disconnect(Channel* channel)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// WindowsChannelAcceptor
//
////////////////////////////////////////////////////////////////////////////////

WindowsChannelAcceptor::WindowsChannelAcceptor(ChannelHandlerFactory* factory)
    : ChannelAcceptor(factory)
{

}

WindowsChannelAcceptor::~WindowsChannelAcceptor()
{

}

void WindowsChannelAcceptor::bind(const char* bindString)
{

}

void WindowsChannelAcceptor::accept(Channel* channel)
{

}

PEGASUS_NAMESPACE_END
