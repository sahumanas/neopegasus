//%////-*-c++-*-////////////////////////////////////////////////////////////////
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
// Author: Mike Day (mdday@us.ibm.com) << Fri Mar 29 09:28:11 2002 mdd >>
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_authorization_h
#define Pegasus_authorization_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <bitset>

PEGASUS_NAMESPACE_BEGIN


// these constants will certainly change with implementation
class PEGASUS_COMMON_LINKAGE peg_identity_types
{
   public:
      static const Uint32 USERNAME;
      static const Uint32 LDAP_DN;
      static const Uint32 GUID;
      static const Uint32 X509;
      static const Uint32 PKCS6;
      static const Uint32 HTTP_DIGEST_USER_AND_REALM;
      static const Uint32 UNIX_ID;
};

// these constants will certainly change with implementation
class PEGASUS_COMMON_LINKAGE peg_credential_types
{
   public:
      static const Uint32 CLEAR_PASSWORD;
      static const Uint32 CRYPT_PASSWORD;
      static const Uint32 HTTP_DIGEST;
};


class PEGASUS_COMMON_LINKAGE pegasus_identity_rep
{
   public:
      pegasus_identity_rep(Uint32 identity_type, 
		       void *identity,
		       Uint32 credential_type,
		       void *credential)
	 : _identity_type(identity_type),
	   _identity(identity),
	   _credential_type(credential_type),
	   _credential(credential),
	   _reference(1), 
	   _thread_safety()
      {

      }
      
      virtual ~pegasus_identity_rep(void)
      {
	 _thread_safety.unlock();
      }
      
      pegasus_identity_rep * operator = (pegasus_identity_rep *rep)
      {
	 _thread_safety.lock(pegasus_thread_self());
	 if (rep != this )
	 {
	    _reference++;
	 }
	 _thread_safety.unlock();
	 return this;
      }
            
      void reference(void) 
      {
	 _reference++;
      }
      
      void dereference(void)
      {
	 _thread_safety.lock(pegasus_thread_self());
	 _reference--;
	 _thread_safety.unlock();
      }
      
      Uint32 get_reference(void)
      {
	 Uint32 ret;
	 _thread_safety.lock(pegasus_thread_self());
	 ret =  _reference.value();
	 _thread_safety.unlock();
	 return ret;
      }
      Uint32 get_reference_no_lock(void)
      {
	 return _reference.value();
      }
      

      Uint32 get_identity_type(void)
      {
	 return _identity_type;
      }
      
      Uint32 get_credential_type(void)
      {
	 
	 return _credential_type;
      }
      
      void *get_identity(void)
      {
	 if( _reference.value() )
	    return _identity;
	 return NULL;
      }
      
      void *get_credential(void)
      {
	 if( _reference.value() )
	    return _credential;
	 return NULL;
      }
		       
      void lock(void) 
      {
	 _thread_safety.lock(pegasus_thread_self());
      }
      
      void unlock(void)
      {
	 _thread_safety.unlock();
      }
      

   private:
      Uint32 _identity_type;
      void *_identity;
      Uint32 _credential_type;
      void *_credential;
      AtomicInt _reference;
      Mutex _thread_safety;
            
      pegasus_identity_rep();
};


class PEGASUS_COMMON_LINKAGE pegasus_base_identity
{
   public:
      pegasus_base_identity(Uint32 identity_type, 
			    void *identity,
			    Uint32 credential_type,
			    void *credential);
      
      virtual ~pegasus_base_identity(void);
      virtual Boolean authenticate(void) = 0;
   protected:
      pegasus_base_identity & operator=(const pegasus_base_identity & id);
      pegasus_base_identity(const pegasus_base_identity & id);

      void *get_base_identity(void)
      {
	 return _rep->get_identity();
      }
      
      void *get_base_credential(void)
      {
	 return _rep->get_credential();
      }
      
   private:
      pegasus_identity_rep *_rep;
};



class PEGASUS_COMMON_LINKAGE pegasus_basic_identity : pegasus_base_identity
{
   public:
      typedef pegasus_base_identity Base;
      
      pegasus_basic_identity(String & username, String & password);
      pegasus_basic_identity( const pegasus_basic_identity & id);
      
      virtual ~pegasus_basic_identity(void);
      
      const String & get_username(void) ;
      const String & get_password(void) ;

      Boolean authenticate(void);
      
   private:
      pegasus_basic_identity(void);
      pegasus_basic_identity & operator= (pegasus_base_identity * id);
      
};



PEGASUS_NAMESPACE_END

#endif
