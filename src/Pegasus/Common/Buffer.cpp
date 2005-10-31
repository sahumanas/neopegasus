//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Michael E. Brasher (mike-brasher@austin.rr.com -- Inova Europe)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include "Buffer.h"

PEGASUS_NAMESPACE_BEGIN

BufferRep Buffer::_empty_rep = { 0, 0, {0} };

static const size_t MIN_CAPACITY = 32;

static Uint32 _next_pow_2(Uint32 x)
{
    if (x < MIN_CAPACITY)
	return MIN_CAPACITY;

    x--;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    x++;

    return x;
}

static inline BufferRep* _allocate(size_t cap)
{
    BufferRep* rep = (BufferRep*)malloc(sizeof(BufferRep) + cap);
    rep->cap = cap;
    return rep;
}

static inline BufferRep* _reallocate(BufferRep* rep, size_t cap)
{
    rep = (BufferRep*)realloc(rep, sizeof(BufferRep) + cap);
    rep->cap = cap;
    return rep;
}

Buffer::Buffer(const Buffer& x)
{
    _rep = _allocate(x._rep->cap);
    memcpy(_rep->data, x._rep->data, x._rep->size);
    _rep->size = x._rep->size;
}

Buffer::Buffer(const char* data, size_t size)
{
    _rep = _allocate(size);
    _rep->size = size;
    memcpy(_rep->data, data, size);
}

Buffer& Buffer::operator=(const Buffer& x)
{
    if (&x != this)
    {
	if (x._rep->size > _rep->cap)
	{
	    if (_rep != &_empty_rep)
		free(_rep);

	    _rep = _allocate(x._rep->cap);
	}

	memcpy(_rep->data, x._rep->data, x._rep->size);
	_rep->size = x._rep->size;
    }
    return *this;
}

void Buffer::_reserve_aux(size_t cap)
{
    if (_rep == &_empty_rep)
    {
	_rep = _allocate(cap);
	_rep->size = 0;
    }
    else
	_rep = _reallocate(_rep, _next_pow_2(cap));
}

void Buffer::_append_char_aux()
{
    if (_rep == &_empty_rep)
    {
	_rep = _allocate(MIN_CAPACITY);
	_rep->size = 0;
    }
    else
	_rep = _reallocate(_rep, _rep->cap ? (2 * _rep->cap) : MIN_CAPACITY);
}

void Buffer::insert(size_t pos, const char* data, size_t size)
{
    if (pos > _rep->size)
	return;

    size_t cap = _rep->size + size;	
    size_t rem = _rep->size - pos;

    if (cap > _rep->cap)
    {
	BufferRep* rep = _allocate(cap);
	rep->size = cap;

	memcpy(rep->data, _rep->data, pos);
	memcpy(rep->data + pos, data, size);
	memcpy(rep->data + pos + size, _rep->data + pos, rem);

	if (_rep != &_empty_rep)
	    free(_rep);

	_rep = rep;
    }
    else
    {
        memmove(_rep->data + pos + size, _rep->data + pos, rem);
	memcpy(_rep->data + pos, data, size);
	_rep->size += size;
    }
}

void Buffer::remove(size_t pos, size_t size)
{
    if (pos + size > _rep->size)
	return;

    size_t rem = _rep->size - (pos + size);

    if (rem)
	memmove(_rep->data + pos, _rep->data + pos + size, rem);

    _rep->size -= size;
}

PEGASUS_NAMESPACE_END