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
// Author:
//
// $Log: Config.h,v $
// Revision 1.6  2001/04/13 22:20:03  mike
// new
//
// Revision 1.5  2001/04/13 21:06:36  mike
// new
//
// Revision 1.4  2001/04/13 19:57:17  mike
// Fixed several memory leaks.
// Fixed build crash problem (caused by screwed up NT flags).
//
// Revision 1.3  2001/04/13 18:20:51  mike
// Ported so Solaris.
// Fixed memory leaks.
//
// Revision 1.2  2001/04/11 00:39:18  mike
// More porting
//
// Revision 1.1.1.1  2001/01/14 19:50:39  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Config.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Config_h
#define Pegasus_Config_h

#include <iostream>

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include <Pegasus/Common/Platform_WIN32_IX86_MSVC.h>
#elif defined (PEGASUS_PLATFORM_LINUX_IX86_GNU)
# include <Pegasus/Common/Platform_LINUX_IX86_GNU.h>
#else
# error "<Pegasus/Common/Config.h>: Unsupported Platform"
#endif

#define PEGASUS_TRACE \
    std::cout << __FILE__ << '(' << __LINE__ << ')' << std::endl

#endif  /* Pegasus_Config_h */
