//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//==============================================================================
//
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Barbara Packard (barbara_packard@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

//%/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "WMIFlavor.h"

PEGASUS_NAMESPACE_BEGIN

WMIFlavor::WMIFlavor(const CIMFlavor & flavor) : CIMFlavor(flavor) //, _ui32(0)
{
}

WMIFlavor::WMIFlavor(const LONG flavor) // : _ui32(0)
{
   if(flavor & WBEM_FLAVOR_FLAG_PROPAGATE_TO_INSTANCE) {
//      _ui32 |= TOINSTANCE;
	   addFlavor(TOINSTANCE);
   }

   if(flavor & WBEM_FLAVOR_FLAG_PROPAGATE_TO_DERIVED_CLASS) {
//      _ui32 |= TOSUBCLASS;
	   addFlavor(TOSUBCLASS);
   }

   if(flavor & WBEM_FLAVOR_OVERRIDABLE) {
//      _ui32 |= OVERRIDABLE;
	   addFlavor(OVERRIDABLE);
   }

   if(flavor & WBEM_FLAVOR_MASK_PROPAGATION) {
      //_ui32 = NONE;
   }

   if(flavor &WBEM_FLAVOR_DONT_PROPAGATE) {
      //_ui32 = NONE;
   }

   if(flavor & WBEM_FLAVOR_NOT_OVERRIDABLE) {
      //_ui32 = NONE;
   }

   if(flavor & WBEM_FLAVOR_MASK_PERMISSIONS) {
      //_ui32 = NONE;
   }

   if(flavor & WBEM_FLAVOR_ORIGIN_LOCAL) {
      //_ui32 = NONE;
   }

   if(flavor & WBEM_FLAVOR_ORIGIN_PROPAGATED) {
      //_ui32 = NONE;
   }

   if(flavor & WBEM_FLAVOR_ORIGIN_SYSTEM) {
      //_ui32 = NONE;
   }

   if(flavor & WBEM_FLAVOR_MASK_ORIGIN) {
      //_ui32 = NONE;
   }

   if(flavor & WBEM_FLAVOR_NOT_AMENDED) {
      //_ui32 = NONE;
   }

   if(flavor & WBEM_FLAVOR_AMENDED) {
      //_ui32 = NONE;
   }

   if(flavor & WBEM_FLAVOR_MASK_AMENDED) {
      //_ui32 = NONE;
   }
}

/*

WMIFlavor::operator Uint32(void) const
{
   return(_ui32);
}

WMIFlavor::operator LONG(void) const
{
   long l = 0;

   if(_ui32 & NONE) {
   }

   if(_ui32 & OVERRIDABLE) {
   }

   if(_ui32 & TOSUBCLASS) {
   }

   if(_ui32 & TOINSTANCE) {
   }

   if(_ui32 & TRANSLATABLE) {
   }

   return(l);
}
*/

PEGASUS_NAMESPACE_END
