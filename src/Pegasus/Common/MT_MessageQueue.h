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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_MT_MessageQueue_h
#define Pegasus_MT_MessageQueue_h

#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Exception.h>

#define PEGASUS_DEFAULT_HIGHWATERMARK 256
#define PEGASUS_DEFAULT_LOWWATERMARK 0

PEGASUS_NAMESPACE_BEGIN

// REVIEW: where is this class used?

// REVIEW: Can it derive from MessageQueue class?

/** The MessageQueue class represents a queue abstraction and is used by
    modules to exchange messages. Methods are provided for enqueuing,
    dequeuing, removing, iterating messages. Some methods are virtual and
    may be overriden but subclasses to modify the behavior.
*/
class PEGASUS_COMMON_LINKAGE MT_MessageQueue
{
public:

    /** This constructor places this object on a queue table which is
	maintained by this class. Each message queue has a queue-id (which
	may be obtained by calling getQueueId()). The queue-id may be passed
	to lookupQueue() to obtain a pointer to the corresponding queue).
    */
    MT_MessageQueue();

    /** Removes this queue from the queue table. */
    virtual ~MT_MessageQueue();

    /** Enques a message (places it at the back of the queue).
	@param message pointer to message to be enqueued.
	@exception throws NullPointer exception if message parameter is null.
    */
    void enqueue(Message* message);

    /** Dequeues a message (removes it from the front of the queue).
	@return pointer to message or zero if queue is empty.
    */
    Message* dequeue();

    /** Removes the given message from the queue.
	@param message to be removed.
	@exception throws NullPointer if message parameter is null.
	@exception throws NoSuchMessageOnQueue is message paramter is not
	    on this queue.
    */
    void remove(Message* message);

    /** Find the message with the given type.
	@parameter type type of message to be found.
	@return pointer to message if found; null otherwise.
    */
    Message* findByType(Uint32 type);

    /** Const version of findByType(). */
    const Message* findByType(Uint32 type) const;

    /** Find the message with the given key.
	@parameter key key of message to be found.
	@return pointer to message if found; null otherwise.
    */
    Message* findByKey(Uint32 key);

    /** Const version of findByKey(). */
    const Message* findByKey(Uint32 key) const;

    /** Finds the messages with the given type and key.
	@param type type of message to be found.
	@param type key of message to be found.
	@return pointer to message if found; null otherwise.
    */
    Message* find(Uint32 type, Uint32 key);

    /** Const version of find(). */
    const Message* find(Uint32 type, Uint32 key) const;

    /** Returns pointer to front message. */
    //Message* front() { return _front; }

    /** Const version of front(). */
    //const Message* front() const { return _front; }

    /** Returns pointer to back message. */
    //Message* back() { return _back; }

    /** Const version of back(). */
    //const Message* back() const { return _back; }

    /** Returns true if there are no messages on the queue. */
    //Boolean isEmpty() const { return _front == 0; }

    /** Returns the number of messages on the queue. */
    //Uint32 getCount() const { return _count; }

    /** Retrieve the queue id for this queue. */
    MessageQueue* getQueue() const { return _mqueue; }

    /** Retrieve the queue id for this queue. */
    //Uint32 getQueueId() const { return _queueId; }

    /** Prints the contents of this queue by calling the print() method
	of each message.
	@param os stream onto which the output is placed.
    */
    void print(PEGASUS_STD(ostream)& os) const;

    /** Lock this queue. */
    virtual void lock();

    /** Unlock this queue. */
    virtual void unlock();

    /** This method is called after a message has been enqueued. This default
	implementation does nothing. Derived classes may override this to
	take some action each time a message is enqueued (for example, this
	method could handle the incoming message in the thread of the caller
	of enqueue()).
    */
    virtual void handleEnqueue();

    MessageQueue * getQueue() { return _mqueue;}

    /** Lookup a message queue from a queue id. */
    static MessageQueue* lookup(Uint32 queueId);

private:

    friend class MessageQueue;

    MessageQueue * _mqueue;

    // Both condition mutexes together protect the queue
    Mutex * _sharedMutex;
    Condition * _full;
    Condition * _empty;

    AtomicInt _readers;
    AtomicInt _writers;

    Boolean _preferReader;

    Uint32 _highWaterMark;
    Uint32 _lowWaterMark;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MT_MessageQueue_h */
