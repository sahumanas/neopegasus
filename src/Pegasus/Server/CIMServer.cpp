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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//         Mike Day (mdday@us.ibm.com)s
//         Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//         Dan Gorey, IBM (djgorey@us.ibm.com)
//         Heather Sterling, IBM (hsterl@us.ibm.com)
//         Amit K Arora, IBM (amita@in.ibm.com) for PEP 193
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include <cstdio>
#include <cctype>
#include <ctime>
#if defined(PEGASUS_OS_HPUX) && defined(PEGASUS_USE_RELEASE_DIRS)
#include <unistd.h>
#endif
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/Signal.h>

#include <Pegasus/Repository/CIMRepository.h>
#include "ProviderMessageFacade.h"
#include <Pegasus/ExportServer/CIMExportRequestDispatcher.h>
#include <Pegasus/ExportServer/CIMExportResponseEncoder.h>
#include <Pegasus/ExportServer/CIMExportRequestDecoder.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/HandlerService/IndicationHandlerService.h>
#include <Pegasus/IndicationService/IndicationService.h>
#include <Pegasus/ProviderManager2/ProviderManagerService.h>

#ifdef PEGASUS_ENABLE_SLP
#include <Pegasus/Client/CIMClient.h>
#endif

#include "CIMServer.h"
#include "CIMOperationRequestDispatcher.h"
#include "CIMOperationResponseEncoder.h"
#include "CIMOperationRequestDecoder.h"
#include "CIMOperationRequestAuthorizer.h"
#include "HTTPAuthenticatorDelegator.h"
#include "ShutdownProvider.h"
#include "ShutdownService.h"
#include "BinaryMessageHandler.h"
#include "SSLContextManager.h"
#include <Pegasus/Common/ModuleController.h>
#include <Pegasus/ControlProviders/ConfigSettingProvider/ConfigSettingProvider.h>
#include <Pegasus/ControlProviders/UserAuthProvider/UserAuthProvider.h>
#include <Pegasus/ControlProviders/ProviderRegistrationProvider/ProviderRegistrationProvider.h>
#include <Pegasus/ControlProviders/NamespaceProvider/NamespaceProvider.h>

#ifndef PEGASUS_DISABLE_PERFINST
#include <Pegasus/ControlProviders/Statistic/CIMOMStatDataProvider.h>
#endif

#ifdef PEGASUS_HAS_SSL
#include <Pegasus/ControlProviders/CertificateProvider/CertificateProvider.h>
#endif

#ifndef PEGASUS_DISABLE_CQL
#include <Pegasus/ControlProviders/QueryCapabilitiesProvider/CIMQueryCapabilitiesProvider.h>
#endif

#if !defined(PEGASUS_DISABLE_PERFINST) ||  defined(PEGASUS_ENABLE_SLP)
#include <Pegasus/ControlProviders/InteropProvider/InteropProvider.h>
#endif

// l10n
#include <Pegasus/Common/MessageLoader.h>


PEGASUS_NAMESPACE_BEGIN

static CIMServer *_cimserver = NULL;

// Need a static method to act as a callback for the control provider.
// This doesn't belong here, but I don't have a better place to put it.
static Message * controlProviderReceiveMessageCallback(
    Message * message,
    void * instance)
{
    ProviderMessageFacade * mpf =
        reinterpret_cast<ProviderMessageFacade *>(instance);
    return mpf->handleRequestMessage(message);
}

//
// Linux signal handler for SIGCHLD
//
#if defined(PEGASUS_OS_LINUX) && defined(PEGASUS_HAS_SIGNALS)
#include <sys/types.h>
#include <sys/wait.h>
void childSignalHandler(int s_n, PEGASUS_SIGINFO_T * s_info, void * sig)
{
    PEG_METHOD_ENTER(TRC_SERVER, "childSignalHandler");

    if (s_n == PEGASUS_SIGCHLD)
    {
        PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL4, "Caught SIGCHLD");
        pid_t cpid = 0;

#if GCC_VERSION > 30200
        while ((cpid = waitpid(0, NULL, WNOHANG | __WCLONE)) > 0);
#else
        while ((cpid = waitpid(0, NULL, WNOHANG)) > 0);
#endif
        if (cpid < 0)
        {
            Tracer::trace(TRC_SERVER, Tracer::LEVEL2,
                "waitpid error: %d", errno);
        }
    }
    PEG_METHOD_EXIT();
}
#endif

//
// Signal handler for shutdown signals, currently SIGHUP and SIGTERM
//
Boolean handleShutdownSignal = false;
void shutdownSignalHandler(int s_n, PEGASUS_SIGINFO_T * s_info, void * sig)
{
    PEG_METHOD_ENTER(TRC_SERVER, "shutdownSignalHandler");
    Tracer::trace(TRC_SERVER, Tracer::LEVEL2, "Signal %d received.", s_n);

    CIMServer::shutdownSignal();

    PEG_METHOD_EXIT();
}

void CIMServer::shutdownSignal()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::shutdownSignal()");
    handleShutdownSignal = true;
    _cimserver->tickle_monitor();
    PEG_METHOD_EXIT();
}


CIMServer::CIMServer(Monitor* monitor)
  : _dieNow(false), _monitor(monitor)
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::CIMServer()");
    _init();
    _cimserver = this;
    PEG_METHOD_EXIT();
}


void CIMServer::tickle_monitor(){
    _monitor->tickle();
}
void CIMServer::_init(void)
{

    String repositoryRootPath = String::EMPTY;
    int binaryMode;
    CIMRepository_Mode Mode;

#ifdef PEGASUS_ENABLE_SLP
    _runSLP = true;         // Boolean cannot be set in definition.

#endif

#if defined(PEGASUS_OS_HPUX) && defined(PEGASUS_USE_RELEASE_DIRS)
    chdir( PEGASUS_CORE_DIR );
#endif
    // -- Save the monitor or create a new one:
    repositoryRootPath =
        ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("repositoryDir"));

    // -- Create a repository:

#ifdef DO_NOT_CREATE_REPOSITORY_ON_STARTUP
    // If this code is enable, the CIMServer will fail to start
    // if the repository directory does not exit. If called,
    // the Repository will create an empty repository.

    // This check has been disabled to allow cimmof to call
    // the CIMServer to build the initial repository.
    if (!FileSystem::isDirectory(repositoryRootPath))
    {
        PEG_METHOD_EXIT();
    throw NoSuchDirectory(repositoryRootPath);

    }
#endif

    binaryMode = (ConfigManager::getInstance()->getCurrentValue(
        "enableBinaryRepository") == "true");

    Mode.flag = CIMRepository_Mode::NONE;
    if (binaryMode) 
      Mode.flag |= CIMRepository_Mode::BIN;

    _repository = new CIMRepository(repositoryRootPath, Mode);

    // -- Create a UserManager object:

    UserManager* userManager = UserManager::getInstance(_repository);

    // -- Create a CIMServerState object:

    _serverState.reset(new CIMServerState());

    _providerRegistrationManager = new ProviderRegistrationManager(_repository);

    // -- Create queue inter-connections:

    _providerManager = new ProviderManagerService(_providerRegistrationManager,_repository);

    _handlerService = new IndicationHandlerService(_repository);

    // Create the control service
    _controlService = new ModuleController(PEGASUS_QUEUENAME_CONTROLSERVICE);

    // Create the Configuration control provider
    ProviderMessageFacade * configProvider =
        new ProviderMessageFacade(new ConfigSettingProvider());
    ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                      PEGASUS_MODULENAME_CONFIGPROVIDER,
                                      configProvider,
                                      controlProviderReceiveMessageCallback,
                                      0, 0);

    // Create the User/Authorization control provider
    ProviderMessageFacade * userAuthProvider =
        new ProviderMessageFacade(new UserAuthProvider(_repository));
    ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                      PEGASUS_MODULENAME_USERAUTHPROVIDER,
                                      userAuthProvider,
                                      controlProviderReceiveMessageCallback,
                                      0, 0);

    // Create the Provider Registration control provider
    ProviderMessageFacade * provRegProvider = new ProviderMessageFacade(
        new ProviderRegistrationProvider(_providerRegistrationManager));
    ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                      PEGASUS_MODULENAME_PROVREGPROVIDER,
                                      provRegProvider,
                                      controlProviderReceiveMessageCallback,
                                      0, 0);

     // Create the Shutdown control provider
     ProviderMessageFacade * shutdownProvider =
         new ProviderMessageFacade(new ShutdownProvider(this));
     ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                       PEGASUS_MODULENAME_SHUTDOWNPROVIDER,
                                       shutdownProvider,
                                       controlProviderReceiveMessageCallback,
                                       0, 0);

     // Create the namespace control provider
     ProviderMessageFacade * namespaceProvider =
         new ProviderMessageFacade(new NamespaceProvider(_repository));
     ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                       PEGASUS_MODULENAME_NAMESPACEPROVIDER,
                                       namespaceProvider,
                                       controlProviderReceiveMessageCallback,
                                       0, 0);

     //
     // Create a SSLContextManager object
     //
     _sslContextMgr = new SSLContextManager();

#ifdef PEGASUS_HAS_SSL
        //Because this provider allows management of the cimserver truststore, 
        //the export truststore, and the local client truststore, it needs to be 
        //available regardless of the values of ssClientVerification and 
        //enableSSLExportVerification.
        ProviderMessageFacade * certificateProvider =
            new ProviderMessageFacade(new CertificateProvider(_repository, 
                                                              _sslContextMgr));
        ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                          PEGASUS_MODULENAME_CERTIFICATEPROVIDER,
                                          certificateProvider,
                                          controlProviderReceiveMessageCallback,
                                          0, 0);
#endif

#ifndef PEGASUS_DISABLE_PERFINST
   // Create the Statistical Data control provider
     ProviderMessageFacade * cimomstatdataProvider =
         new ProviderMessageFacade(new CIMOMStatDataProvider());
     ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                       PEGASUS_MODULENAME_CIMOMSTATDATAPROVIDER,                                       cimomstatdataProvider,
                                       controlProviderReceiveMessageCallback,
                                       0, 0);
#endif

#ifndef PEGASUS_DISABLE_CQL

   // Create the Query Capabilities control provider
     ProviderMessageFacade * cimquerycapprovider =
         new ProviderMessageFacade(new CIMQueryCapabilitiesProvider());
     ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                       PEGASUS_MODULENAME_CIMQUERYCAPPROVIDER,
                                       cimquerycapprovider,
                                       controlProviderReceiveMessageCallback,
                                       0, 0);
#endif


#if !defined(PEGASUS_DISABLE_PERFINST) ||  defined(PEGASUS_ENABLE_SLP)

// Create the interop control provider
     ProviderMessageFacade * interopProvider =
         new ProviderMessageFacade(new InteropProvider(_repository));
     ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                       PEGASUS_MODULENAME_INTEROPPROVIDER,
                                       interopProvider,
                                       controlProviderReceiveMessageCallback,
                                       0, 0);
#endif

    _cimOperationRequestDispatcher
    = new CIMOperationRequestDispatcher(_repository,
                                            _providerRegistrationManager);
    _binaryMessageHandler =
       new BinaryMessageHandler(_cimOperationRequestDispatcher);

    _cimOperationResponseEncoder
    = new CIMOperationResponseEncoder;

    //
    // get the configured authentication and authorization flags
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean enableAuthentication = false;

    if (String::equalNoCase(
        configManager->getCurrentValue("enableAuthentication"), "true"))
    {
        enableAuthentication = true;
    }

    //
    // Create Authorization queue only if authentication is enabled
    //
    if ( enableAuthentication )
    {
        _cimOperationRequestAuthorizer = new CIMOperationRequestAuthorizer(
            _cimOperationRequestDispatcher);

        _cimOperationRequestDecoder = new CIMOperationRequestDecoder(
            _cimOperationRequestAuthorizer,
            _cimOperationResponseEncoder->getQueueId());
    }
    else
    {
        _cimOperationRequestAuthorizer = 0;

        _cimOperationRequestDecoder = new CIMOperationRequestDecoder(
            _cimOperationRequestDispatcher,
            _cimOperationResponseEncoder->getQueueId());

    }

    _cimExportRequestDispatcher
    = new CIMExportRequestDispatcher();

    _cimExportResponseEncoder
    = new CIMExportResponseEncoder;

    _cimExportRequestDecoder = new CIMExportRequestDecoder(
    _cimExportRequestDispatcher,
    _cimExportResponseEncoder->getQueueId());

    _httpAuthenticatorDelegator = new HTTPAuthenticatorDelegator(
        _cimOperationRequestDecoder->getQueueId(),
        _cimExportRequestDecoder->getQueueId(),
        _repository);

    // IMPORTANT-NU-20020513: Indication service must start after ExportService
    // otherwise HandlerService started by indicationService will never
    // get ExportQueue to export indications for existing subscriptions

    _indicationService = 0;
    if (String::equal(
        configManager->getCurrentValue("enableIndicationService"), "true"))
    {
        _indicationService = new IndicationService
            (_repository, _providerRegistrationManager);
    }

    // Enable the signal handler to shutdown gracefully on SIGHUP and SIGTERM
    getSigHandle()->registerHandler(PEGASUS_SIGHUP, shutdownSignalHandler);
    getSigHandle()->activate(PEGASUS_SIGHUP);
    getSigHandle()->registerHandler(PEGASUS_SIGTERM, shutdownSignalHandler);
    getSigHandle()->activate(PEGASUS_SIGTERM);

    // Configure the signal handling behavior for SIGCHLD
#if defined (PEGASUS_OS_LINUX)
    // From signal manpage on Linux:
    //
    // "According to POSIX (3.3.1.3) it is unspecified what happens when
    // SIGCHLD is set to SIG_IGN.  Here the BSD and SYSV behaviours differ,
    // causing BSD software that sets the action for SIGCHLD to SIG_IGN to
    // fail on Linux."
    //
    // On Linux, when you perform a "system" it calls wait().  If you have
    // ignored SIGCHLD, a POSIX-conformant system is allowed to collect
    // zombies immediately rather than holding them for you to wait for.
    // And you end up with "system" returning non-zero return code instead
    // of zero return code for successful calls.
    getSigHandle()->registerHandler(PEGASUS_SIGCHLD, childSignalHandler);
    getSigHandle()->activate(PEGASUS_SIGCHLD);
#else
    SignalHandler::ignore(PEGASUS_SIGCHLD);  // Allows child death
#endif

    // Load and initialize providers registed with AutoStart = true
    _providerRegistrationManager->initializeProviders();

}




CIMServer::~CIMServer()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::~CIMServer()");

    // Note: do not delete the acceptor because it belongs to the Monitor
    // which takes care of disposing of it.

    if (_providerRegistrationManager)
    {
        delete _providerRegistrationManager;
    }

    PEG_METHOD_EXIT();
}

void CIMServer::addAcceptor(
    Boolean localConnection,
    Uint32 portNumber,
    Boolean useSSL,
    Boolean exportConnection)
{
    HTTPAcceptor* acceptor;
    if (exportConnection)
    {
      //
      // On export connection, create SSLContext with a indication
      // trust store.
      //
      acceptor = new HTTPAcceptor(
          _monitor,
          _httpAuthenticatorDelegator,
          localConnection,
          portNumber,
          useSSL ? _sslContextMgr->getSSLContext(SSLContextManager::EXPORT_CONTEXT) : 0,
          exportConnection,
          useSSL ? _sslContextMgr->getSSLContextObjectLock() : 0);
    }
    else
    {
      acceptor = new HTTPAcceptor(
          _monitor,
          _httpAuthenticatorDelegator,
          localConnection,
          portNumber,
          useSSL ? _sslContextMgr->getSSLContext(SSLContextManager::SERVER_CONTEXT) : 0,
          exportConnection,
          useSSL ? _sslContextMgr->getSSLContextObjectLock() : 0);
    }
    _acceptors.append(acceptor);
}

void CIMServer::bind()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::bind()");

    if (_acceptors.size() == 0)
    {
        MessageLoaderParms mlp = MessageLoaderParms(
            "Server.CIMServer.BIND_FAILED",
            "No CIM Server connections are enabled.");

        throw BindFailedException(mlp);
    }

    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
        _acceptors[i]->bind();
    }

    PEG_METHOD_EXIT();
}

void CIMServer::runForever()
{
    // Note: Trace code in this method will be invoked frequently.

    if(!_dieNow)
    {
#ifdef PEGASUS_ENABLE_SLP
    // Note - this func prevents multiple starting of slp provider
    startSLPProvider();
#endif

    if(false == _monitor->run(500000))
    {
      try
      {
        MessageQueueService::_check_idle_flag = 1;
        MessageQueueService::_polling_sem.signal();
        
        _providerManager->unloadIdleProviders();
      }
      catch(...)
      {
      }
    }

    if (handleShutdownSignal)
    {
      Tracer::trace(TRC_SERVER, Tracer::LEVEL3,
            "CIMServer::runForever - signal received.  Shutting down.");
      ShutdownService::getInstance(this)->shutdown(true, 10, false);
      // Set to false must be after call to shutdown.  See
      // stopClientConnection.
      handleShutdownSignal = false;
    }
    }
}

void CIMServer::stopClientConnection()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::stopClientConnection()");

    // tell Monitor to stop listening for client connections
    if (handleShutdownSignal)
        // If shutting down, this is in the same thread as runForever.
        // No need to wait for the thread to see the stop flag.
        _monitor->stopListeningForConnections(false);
    else
        // If not shutting down, this is not in the same thread as runForever.
        // Need to wait for the thread to see the stop flag.
        _monitor->stopListeningForConnections(true);

    //
    // Wait 150 milliseconds to allow time for the Monitor to stop 
    // listening for client connections.  
    //
    // This wait time is the timeout value for the select() call
    // in the Monitor's run() method (currently set to 100 
    // milliseconds) plus a delta of 50 milliseconds.  The reason
    // for the wait here is to make sure that the Monitor entries
    // are updated before closing the connection sockets.
    //
    // PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL4, "Wait 150 milliseconds.");
    //  pegasus_sleep(150);  not needed anymore due to the semaphore
    // in the monitor

    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
        _acceptors[i]->closeConnectionSocket();
    }

    PEG_METHOD_EXIT();
}

void CIMServer::shutdown()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::shutdown()");

    _dieNow = true;
    _cimserver->tickle_monitor();

    PEG_METHOD_EXIT();
}

void CIMServer::resume()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::resume()");

    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
        _acceptors[i]->reopenConnectionSocket();
    }

    PEG_METHOD_EXIT();
}

void CIMServer::setState(Uint32 state)
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::setState()");

    _serverState->setState(state);

    //
    // get the configured authentication and authorization flags
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean enableAuthentication = false;
    Boolean enableNamespaceAuthorization = false;

    if (String::equal(
        configManager->getCurrentValue("enableAuthentication"), "true"))
    {
        enableAuthentication = true;
    }

    if (String::equal(
        configManager->getCurrentValue("enableNamespaceAuthorization"), "true"))
    {
        enableNamespaceAuthorization = true;
    }

    if (state == CIMServerState::TERMINATING)
    {
        // tell decoder that CIMServer is terminating
        _cimOperationRequestDecoder->setServerTerminating(true);
        _cimExportRequestDecoder->setServerTerminating(true);

        // tell authorizer that CIMServer is terminating ONLY if
        // authentication and authorization are enabled
        //
        if ( enableAuthentication && enableNamespaceAuthorization )
        {
            _cimOperationRequestAuthorizer->setServerTerminating(true);
        }
    }
    else
    {
        // tell decoder that CIMServer is not terminating
        _cimOperationRequestDecoder->setServerTerminating(false);
        _cimExportRequestDecoder->setServerTerminating(false);

        // tell authorizer that CIMServer is terminating ONLY if
        // authentication and authorization are enabled
        //
        if ( enableAuthentication && enableNamespaceAuthorization )
        {
            _cimOperationRequestAuthorizer->setServerTerminating(false);
        }
    }
    PEG_METHOD_EXIT();
}

Uint32 CIMServer::getOutstandingRequestCount()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::getOutstandingRequestCount()");

    Uint32 requestCount = 0;

    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
        requestCount += _acceptors[i]->getOutstandingRequestCount();
    }

    PEG_METHOD_EXIT();
    return requestCount;
}


#ifdef PEGASUS_ENABLE_SLP
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _callSLPProvider(void *parm);


// This is a control function that starts a new thread which issues a
// cim operation to start the slp provider.
void CIMServer::startSLPProvider()
{

   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "CIMServer::startSLPProvider");

    
    // This is a onetime function.  If already issued, or config is not to use simply
    // return
    if (!_runSLP)
    {
        return;
    }

    // Get Config parameter to determine if we should start SLP.
    ConfigManager* configManager = ConfigManager::getInstance();
    _runSLP = String::equal(
         configManager->getCurrentValue("slp"), "true");

    // If false, do not start slp provider
    if (!_runSLP)
    {
        return;
    }
    //SLP startup is onetime function; reset the switch so this
    // function does not get called a second time.
    _runSLP = false;

    // Create a separate thread, detach and call function to execute the startup.
    Thread t( _callSLPProvider, 0, true );
    t.run();

    PEG_METHOD_EXIT();
    return;
}


// startSLPProvider is a function to get the slp provider kicked off
// during startup.  It is placed in the provider manager simply because 
// the provider manager is the only component of the system is
// driven by a timer after startup.  It should never be here and must be
// moved to somewhere more logical or really replaced. We simply needed
// something that was run shortly after system startup.
// This function is assumed to operate in a separate thread and 
// KS 15 February 2004.

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _callSLPProvider(void* parm )
{
    //
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::_callSLPProvider()");
    // Create CIMClient object
    //
    CIMClient client;
    //
    // open connection to CIMOM 
    //
    String hostStr = System::getHostName();

    try
    {
        //
        client.connectLocal();

        //
        // set client timeout to 2 seconds
        //
        client.setTimeout(40000);
        // construct CIMObjectPath
        //
        String referenceStr = "//";
        referenceStr.append(hostStr);
        referenceStr.append("/");  
        referenceStr.append(PEGASUS_NAMESPACENAME_INTERNAL.getString());
        referenceStr.append(":");
        referenceStr.append(PEGASUS_CLASSNAME_WBEMSLPTEMPLATE.getString());
        CIMObjectPath reference(referenceStr);

        //
        // issue the invokeMethod request on the register method
        //
        Array<CIMParamValue> inParams;
        Array<CIMParamValue> outParams;

        CIMValue retValue = client.invokeMethod(
            PEGASUS_NAMESPACENAME_INTERNAL,
            reference,
            CIMName("register"),
            inParams,
            outParams
            );
    }

    catch(CIMException& e)
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
            "SLP Registration Failed. CIMException. $0", e.getMessage());
    }

    catch(Exception& e)
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
            "SLP Registration Failed Startup: CIMServer exception. $0", e.getMessage());
    }

    client.disconnect();

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
        "SLP Registration Initiated");

    PEG_METHOD_EXIT();
    return( (PEGASUS_THREAD_RETURN)32 );
}
#endif

PEGASUS_NAMESPACE_END



