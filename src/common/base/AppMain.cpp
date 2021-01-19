#include <common/base/AppMain.h>

#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <set>
#include <functional>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/backtrace.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include <common/base/ScopedGuard.h>
#include <common/base/logger/Logger.h>
#include <common/base/logger/LoggerManager.h>

#ifdef WIN32
#include <cntservice/resource.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#include <locale>
#endif

using namespace CBASE_LOGGER_NAMESPACE;

CBASE_BEGIN

#ifndef WIN32 
static void defaultSignalHandler(int signo)
{
    CB_LOG_SYS_INFO("default handler: signal " << signo << " is triggered.");
}

static void childSignalHandler(int signo)
{
    if (signo != SIGCHLD)
        return;

    int status;
    pid_t pid;
    while ((pid = ::waitpid(-1, &status, WNOHANG)) > 0)
    {
        CB_LOG_SYS_DEBUG("process " << pid << " is exited.");
        if (AppMain::getInstance() != NULL) 
        {
            AppMain::getInstance()->childProcessExit(pid);
        }
    }

}
static bool installSignalHandler(int signo, void (*handler)(int signo));
static void criticalSignalHandler(int signo)
{
    CB_LOG_SYS_ERROR("critical handler: signal " << signo << " is triggered.");
    boost::backtrace bt;
    CB_LOG_SYS_ERROR(bt.trace());
    if (AppMain::getInstance() && AppMain::getInstance()->isCoreDumpEnabled())
    {
        installSignalHandler(signo, SIG_DFL);
    }
    else
    {
        ::exit(EXIT_FAILURE);
    }
}

static bool installSignalHandler(int signo, void (*handler)(int signo))
{
    struct sigaction act;
    ::memset(&act, 0, sizeof(act));
    act.sa_handler = handler;
    act.sa_flags = SA_RESTART;
    if (signo == SIGCHLD)
    {
        act.sa_flags |= SA_NOCLDSTOP;
    }
    else if (signo == SIGALRM)
    {
        act.sa_flags |= SA_NODEFER;
    }
    sigemptyset(&act.sa_mask);
    if (::sigaction(signo, &act, NULL) < 0)
    {
        CB_LOG_SYS_ERROR("failed to install signal handler for " << signo);
        return false;
    }

    return true;
}
#endif // NOT WIN32

#ifdef WIN32
#include <DbgHelp.h>
#if defined(_M_IX86)
static LONG WINAPI unhandledExceptionFilter(
    struct _EXCEPTION_POINTERS *exceptionInfo
    )
{
    PCONTEXT context = exceptionInfo->ContextRecord;
    CONTEXT iterContext = *context;
    STACKFRAME stackFrame;
    HANDLE process = ::GetCurrentProcess();
    HANDLE thread = ::GetCurrentThread();

    CB_LOG_SYS_ERROR("critical exception occurs.");
    memset(&stackFrame, 0, sizeof(stackFrame));
    stackFrame.AddrPC.Offset = context->Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;

    std::vector<void *> frames;
    frames.push_back((void *)stackFrame.AddrPC.Offset);
    while (true)
    {
        if (!StackWalk(IMAGE_FILE_MACHINE_I386, process, thread, &stackFrame, &iterContext, NULL, NULL, NULL, NULL))
        {
            break;
        }
        frames.push_back((void *)stackFrame.AddrPC.Offset);
    }

    std::string symbols = boost::stack_trace::get_symbols(frames.data(), frames.size());
    CB_LOG_SYS_ERROR(symbols);

    return !((AppMain::getInstance() && AppMain::getInstance()->isCoreDumpEnabled())) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;    
}

#elif defined(_M_X64)
static LONG WINAPI unhandledExceptionFilter(
    struct _EXCEPTION_POINTERS *exceptionInfo
    )
{
    PCONTEXT context = exceptionInfo->ContextRecord;
    CONTEXT iterContext = *context;
    STACKFRAME64 stackFrame;
    HANDLE process = ::GetCurrentProcess();
    HANDLE thread = ::GetCurrentThread();

    CB_LOG_SYS_ERROR("critical exception occurs.");
    memset(&stackFrame, 0, sizeof(stackFrame));
    stackFrame.AddrPC.Offset = context->Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;

    std::vector<void *> frames;
    frames.push_back((void *)stackFrame.AddrPC.Offset);
    while (true)
    {
        if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &stackFrame, &iterContext, NULL, NULL, NULL, NULL))
        {
            break;
        }
        frames.push_back((void *)stackFrame.AddrPC.Offset);
    }

    std::string symbols = boost::stack_trace::get_symbols(frames.data(), frames.size());
    CB_LOG_SYS_ERROR(symbols);

    return !((AppMain::getInstance() && AppMain::getInstance()->isCoreDumpEnabled())) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;    
}
#else
static LONG WINAPI unhandledExceptionFilter(
    struct _EXCEPTION_POINTERS *exceptionInfo
    )
{
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif
#endif // WIN32

AppMain * AppMain::gInstance = NULL;

#ifdef WIN32
#define SERVICE_OPERATE_TIMEOUT 5000
/* example :
CNTService(
    _T("AppMainService"),
    _T("AppMain's Demo NT Service"),
    SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE,
    _T("Description text for AppMain's Demo Service"))
*/
AppMain::AppMain(
    int argc,
    char *argv[],
    LPCTSTR serviceName,
    LPCTSTR displayName,
    DWORD controlsAccepted,
    LPCTSTR description)
    : CNTService(serviceName, displayName, controlsAccepted, description)
    , mArgCount(argc)
    , mArgs(argv)
    , mCoreDumpEnabled(false)
{
    Logger::initialize();
}

AppMain::AppMain(
    int argc,
    char *argv[],
    LPCTSTR lpszServiceName,
    LPCTSTR lpszDisplayName,
    DWORD dwControlsAccepted,
    LoggerParameter &logPara,
    LPCTSTR lpszDescription)
    : CNTService(lpszServiceName, lpszDisplayName, dwControlsAccepted, lpszDescription)
    , mArgCount(argc)
    , mArgs(argv)
    , mCoreDumpEnabled(false)
{
    Logger::init();
}

#else
AppMain::AppMain(int argc, char *argv[], bool ignoreChildSignal)
    : mArgCount(argc)
    , mArgs(argv)
    , mSignalQuit(false)
    , mIgnoreChildSignal(ignoreChildSignal)
    , mChildSignalBlocked(false)
    , mSignalBlockPending(false)
    , mCoreDumpEnabled(false)
{
    //appendPreHandler(boost::bind(&AppMain::processSignal, boost::lambda::_1));
    //appendPreHandler(&processSignal);
    Logger logger;
    logger.init();
}

AppMain::AppMain(int argc, char *argv[], LoggerParameter &logPara, bool ignoreChildSignal)
    : mArgCount(argc)
    , mArgs(argv)
    , mSignalQuit(false)
    , mIgnoreChildSignal(ignoreChildSignal)
    , mChildSignalBlocked(false)
    , mSignalBlockPending(false)
    , mCoreDumpEnabled(false)
{
    Logger logger;
    logger.init(logPara);
}
#endif

AppMain::~AppMain()
{

}

bool AppMain::logInit(LoggerParameter &logPara)
{
    Logger logger;
    logger.init(logPara);
    return true;
}

AppMain * AppMain::getInstance()
{
    return gInstance;
}

int AppMain::exec()
{
    gInstance = this;
    /* set only character to utf8, so log4cxx will display Chinese in UTF8 format, don't bother LC_ALL */
#ifdef __linux__
    std::setlocale(LC_CTYPE, "en_US.UTF-8");
#else if defined(__APPLE__) && defined(__MACH__)
    setlocale(LC_CTYPE, "en_US.UTF-8");
#endif // WIN32

    //std::string pidfile;
    //std::string cfgfile;
    try
    {
        //po::store(po::parse_command_line(mArgCount, mArgs, getOptionsDescription(pidfile, cfgfile)), mVariablesMap);
        po::options_description desc = getOptionsDescription();
        po::store(po::command_line_parser(mArgCount, mArgs).options(desc).allow_unregistered().run(), mVariablesMap);
        po::notify(mVariablesMap);

        if (mVariablesMap.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }
    }
    catch (po::error &e)
    {
        CB_ERROR("Error: " << e.what());
        return 1;
    }

    /* retVal:
    1  - operate successfully
    0  - do nothing
    -1 - operate failed */
    int retVal = 0;
    if ((retVal = winServiceOperation()))
        return retVal;


    if (mVariablesMap.count("daemon"))
    {
        CB_INFO("running in daemon mode.");
        if (!daemonize())
        {
            return -1;
        }
    }

    if (mVariablesMap.count("pidfile"))
    {
        mPidfile = mVariablesMap["pidfile"].as<std::string>();
        CB_INFO("Pid file: " << mPidfile);
        if (createPidfile() != 0)
        {
            CB_ERROR("Create pid file failed.");
            return -1;
        }
    }

    if (mVariablesMap.count("cfgfile"))
    {
        mCfgfile = mVariablesMap["cfgfile"].as<std::string>();
        CB_INFO("Config file: " << mCfgfile);
    }

    if (mVariablesMap.count("core"))
    {
        mCoreDumpEnabled = true;
        CB_INFO("core dump is enabled for critical signal.");
    }


    if (!setupSignalProcessing())
    {
        CB_ERROR("failed to set up signal processing.");
        return -1;
    }
    ON_SCOPE_EXIT(boost::bind(&AppMain::teardownSignalProcessing, this));

    for (auto &handler: mPreHandlers)
    {
        handler(this);
    }

#ifdef WIN32
    int rv;
    if(mVariablesMap.count("nodaemon"))
    {
        if (mVariablesMap.count("noconsole"))
            FreeConsole();
        rv = run();
    }
    else
        rv = Run() ? 0 : -1; //Run in cntservice
#else
    int rv = run();
#endif

    for (auto &handler: mPostHandlers)
    {
        handler(this);
    }

    return rv;
}

int AppMain::quit()
{
    if (removePidfile() != 0)
    {
        CB_ERROR("Remove pid file failed.");
        return -1;
    }
    return 0;
}

int AppMain::reloadConfiguration()
{
    return 0;
}

std::string AppMain::getUsage() const
{
    return (boost::format("Usage: %s [options]\n"
                         "options:\n"
                         "\t-h, --help\tprint usage\n"
                         "\t-d, --daemon\trun in daemon mode\n"
                         "\t-p, --pidfile\tspecify pid file\n"
                         "\t-c, --cfgfile\tspecify config file\n")
        % (mArgs[0])).str();
}

bool AppMain::appendPreHandler(boost::function< void (AppMain *)> handler)
{
    mPreHandlers.push_back(handler);
    return true;
}

bool AppMain::appendPostHandler(boost::function< void (AppMain *)> handler)
{
    mPostHandlers.push_back(handler);
    return true;
}

bool AppMain::setupSignalProcessing()
{
#ifndef WIN32
    struct rlimit rlim;
    memset(&rlim, 0, sizeof(rlim));
    int rv = getrlimit(RLIMIT_CORE, &rlim);
    if (rv == 0)
    {
        CB_LOG_SYS_INFO("core size is set to " << rlim.rlim_cur);
        if (rlim.rlim_cur > 0)
        {
            mCoreDumpEnabled = true;
        }
    }
    else
    {
        CB_LOG_SYS_ERROR("failed to get core size.");
    }
    CB_LOG_SYS_DEBUG("installing signal handler.");
    installSignalHandler(SIGPIPE, SIG_IGN);
    installSignalHandler(SIGUSR1, &defaultSignalHandler);
    installSignalHandler(SIGUSR2, SIG_IGN);
    installSignalHandler(SIGHUP, &defaultSignalHandler);
    installSignalHandler(SIGCHLD, &childSignalHandler);
    installSignalHandler(SIGINT, &defaultSignalHandler);
    installSignalHandler(SIGTERM, &defaultSignalHandler);
    installSignalHandler(SIGQUIT, &defaultSignalHandler);
    installSignalHandler(SIGSEGV, &criticalSignalHandler);
    installSignalHandler(SIGILL, &criticalSignalHandler);
    installSignalHandler(SIGFPE, &criticalSignalHandler);
    installSignalHandler(SIGABRT, &criticalSignalHandler);

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGCHLD);
    sigaddset(&sigset, SIGHUP);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGUSR1); // currently used for quiting sigwait.

    if (::pthread_sigmask(SIG_BLOCK, &sigset, NULL) < 0)
    {
        CB_LOG_SYS_ERROR("failed to set signal mask");
        return false;
    }
    mSignalQuit = false;
    mSignalThread.reset(new boost::thread(boost::bind(&AppMain::processSignal, this, sigset)));
#endif // NOT WIN32 
#ifdef WIN32
    SetUnhandledExceptionFilter(&unhandledExceptionFilter);
#endif

    return true;
}

void AppMain::teardownSignalProcessing()
{
#ifndef WIN32 
    if (mSignalThread)
    {
        mSignalQuit = true;
        ::kill(::getpid(), SIGQUIT);
        mSignalThread->join();
        mSignalThread.reset();
    }
#endif // NOT WIN32
}

#ifndef WIN32 
void AppMain::processSignal(sigset_t sigset)
{
    int signo;
    ON_SCOPE_EXIT([] ()
                  {
                      CB_LOG_SYS_DEBUG("signal processing thread is exited.") ;
                  });
    bool isChildSet = (sigismember(&sigset, SIGCHLD) != 0);
    
    while (!mSignalQuit)
    {
        sigset_t currsigset = sigset;
        if (isChildSet && mChildSignalBlocked)
        {
            sigdelset(&currsigset, SIGCHLD);
        }
        
        if (sigwait(&currsigset, &signo) < 0)
        {
            checkSignalBlockPending();
            CB_LOG_SYS_ERROR("failed to wait signal");
            mSignalQuit = true;
            break;
        }
        checkSignalBlockPending();
        CB_LOG_SYS_DEBUG("receive a signal: " << signo << ".");
        switch (signo)
        {
        case SIGCHLD:
            if (!mIgnoreChildSignal)
                childSignalHandler(signo);
            break;
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
            if (mSignalQuit)
                break;
            CB_INFO("signal " << signo << " is received, quitting...");
            this->quit();
            mSignalQuit = true;
            break;
        case SIGHUP:
            if (!reloadConfiguration())
            {
                CB_ERROR("failed to reload configuration.");
                mSignalQuit = true;
            }
            break;
        default:
            CB_INFO("signal " << signo << " is triggered.");
            break;
        }
    }
}

void AppMain::ignoreChildSignal(bool ignore)
{
    mIgnoreChildSignal = ignore;
}

void AppMain::blockChildSignal()
{
    std::unique_lock<std::mutex> ul(mSignalBlockLock);
    if (mChildSignalBlocked)
        return;
    mChildSignalBlocked = true;
    mSignalBlockPending = true;
    ::kill(::getpid(), SIGUSR1);
    while (mSignalBlockPending)
    {
        mSignalBlockCond.wait(ul);
    }
}

void AppMain::unblockChildSignal()
{
    std::unique_lock<std::mutex> ul(mSignalBlockLock);
    if (!mChildSignalBlocked) 
        return;
    mChildSignalBlocked = false;
    mSignalBlockPending = true;
    ::kill(::getpid(), SIGUSR1);
    while (mSignalBlockPending)
    {
        mSignalBlockCond.wait(ul);
    }
}

void AppMain::checkSignalBlockPending()
{
    std::unique_lock<std::mutex> ul(mSignalBlockLock);
    if (mSignalBlockPending)
    {
        mSignalBlockPending = false;
        mSignalBlockCond.notify_all();
    }
}

int AppMain::childProcessExit(pid_t pid)
{
    return 0;
}

#endif // NOT WIN32

bool AppMain::daemonize()
{
#ifndef WIN32 
    if (::daemon(0, 0) < 0)
    {
        CB_LOG_SYS_ERROR("failed to run the process in daemon mode.");
        return false;
    }
#endif // NOT WIN32 

    return true;
}

//po::options_description AppMain::getOptionsDescription(std::string &pidfile, std::string &cfgfile) const
po::options_description AppMain::getOptionsDescription() const
{
    po::options_description desc("AppMain options");
    desc.add_options()
        ("help,h", "print help message")
        ("daemon,d", "run in daemon mode")
        ("pidfile,p", po::value<std::string>(), "specify pid file")
        //("pidfile,p", po::value<std::string>(&pidfile), "specify pid file")
        //("cfgfile,c", po::value<std::string>(&cfgfile), "specify config file")
        ("cfgfile,c", po::value<std::string>(), "specify config file")
        ("core", "enable core dump when critical signal recevied.")
#ifdef WIN32
        ("install", po::value<std::vector<std::string> >()->multitoken()->zero_tokens(), "install the service")
        ("uninstall", "uninstall the service")
        ("start", "start the service")
        ("stop", "stop the service")
        ("pause", "pause the service")
        ("continue", "continue the service")
        ("nodaemon", "running in no daemon mode")
        ("noconsole", "running in no console mode")
#endif
        ;
    return desc;
}

std::string AppMain::toString() const
{
    return "AppMain";
}

int AppMain::run()
{
    return 0;
}

int AppMain::winServiceOperation()
{
#ifdef WIN32
#define SERVICE_OPERATE(operateName, operateFunc, description, FAIL_ID) \
if (mVariablesMap.count(operateName)) {\
    if (operateFunc) {\
        CB_INFO(description << " successfully!");\
        return 1;\
    } else {\
        CNTServiceString sErrorMsg(\
            Win32ErrorToString(MapResourceID(FAIL_ID), errorBits, FALSE));\
        CB_ERROR("failed to " << description << sErrorMsg); \
        return -1;\
    }\
}

    DWORD errorBits = 0x00;

    if (mVariablesMap.count("install"))
    {
        const std::vector<std::string> &installParams =
				mVariablesMap["install"].as<std::vector<std::string> >();

        const size_t installParamsSize = installParams.size(); 
        if (installParamsSize != 2 && installParamsSize != 0)
        {
            CB_ERROR("install params num is error. " << installParamsSize);
            return -1;
        }

        if (installParamsSize == 2)
        {
            m_sUserName = installParams[0];
            m_sPassword = installParams[1];
        }

        if (install(errorBits))
        {
            CB_INFO("install service successfully!");
            return 1;

        }
        else
        {
            CNTServiceString sErrorMsg(
                Win32ErrorToString(MapResourceID(0), errorBits, FALSE));
            CB_ERROR("failed to install service. " << sErrorMsg);
            return -1;
        }
    }

    //SERVICE_OPERATE("install", install(errorBits), "install the service", 0)
    SERVICE_OPERATE("uninstall", uninstall(errorBits), "uninstall the service", 0)
    SERVICE_OPERATE(
        "start",
        SetServiceToStatus(ActionStartService, errorBits, SERVICE_OPERATE_TIMEOUT),
        "start the service",
        IDS_NTSERV_FAIL_START_SERVICE
    )
    SERVICE_OPERATE(
        "stop",
        SetServiceToStatus(ActionStopService, errorBits, SERVICE_OPERATE_TIMEOUT),
        "stop the service",
        IDS_NTSERV_FAIL_STOP_SERVICE
    )
    SERVICE_OPERATE(
        "pause",
        SetServiceToStatus(ActionPauseService, errorBits, SERVICE_OPERATE_TIMEOUT),
        "pause the service",
        IDS_NTSERV_FAIL_PAUSE_SERVICE
    )
    SERVICE_OPERATE(
        "continue",
        SetServiceToStatus(ActionContinueService, errorBits, SERVICE_OPERATE_TIMEOUT),
        "continue the service",
        IDS_NTSERV_FAIL_CONTINUE_SERVICE
    )
#endif
	return 0;
}
#ifdef WIN32
bool AppMain::install(DWORD &errorBits)
{
    //First get the full path of this exe
    TCHAR appPath[_MAX_PATH];
    memset(appPath, 0x00, sizeof(TCHAR) * _MAX_PATH);
    if (!GetModuleFileName(NULL, appPath, _MAX_PATH)) {
        CB_ERROR("failed get service path!");
        return false;
    }
    m_sBinaryPathName = appPath;

    //Always ensure the sevice path is quoted if there are spaces in it
    if (std::find(m_sBinaryPathName.begin(), m_sBinaryPathName.end(), _T(' ')) !=
        m_sBinaryPathName.end())
        m_sBinaryPathName = _T('\"') + m_sBinaryPathName + _T('\"');

    //Add in the pertinent options we have already parsed from the command line
    if (m_sServiceName.length()){
        TCHAR szName[2048];
	_stprintf_s(szName, sizeof(szName) / sizeof(TCHAR), _T(" \"/SN:%s\""), m_sServiceName.c_str());
        m_sBinaryPathName += szName;
    }

    if (m_sDisplayName.length()){
        TCHAR szDisplayName[2048];
        _stprintf_s(szDisplayName, sizeof(szDisplayName) / sizeof(TCHAR), _T(" \"/SDN:%s\""), m_sDisplayName.c_str());
        m_sBinaryPathName += szDisplayName;
    }

    if (m_sDescription.length()){
        TCHAR szDescription[2048];
        _stprintf_s(szDescription, sizeof(szDescription) / sizeof(TCHAR), _T(" \"/SD:%s\""), m_sDescription.c_str());
        m_sBinaryPathName += szDescription;
    }
    
    //Should the service be started automatically
    m_dwStartType = SERVICE_AUTO_START;


    //Display the error message if the install failed
    CNTServiceString sErrorMsg;
    if (!Install(sErrorMsg, errorBits)){
        CB_ERROR("install service error : " << sErrorMsg);
        return false;
    }

    return true;
}

bool AppMain::uninstall(DWORD &errorBits)
{
    CNTServiceString errorMsg;
    if (!Uninstall(errorMsg, errorBits, SERVICE_OPERATE_TIMEOUT)){
        CB_ERROR("uninstall service error : " << errorMsg);
        return false;
    }

    return true;
}
#endif

int AppMain::createPidfile()
{
    if (mPidfile.empty())
    {
        return 0;
    }
    FILE *fp = fopen(mPidfile.c_str(), "wt");
    if (NULL == fp)
    {
        CB_ERROR("fopen pidfile error: " << strerror(errno));
        return 1;
    }
    if (fprintf(fp, "%d", getpid()) < 0)
    {
        CB_ERROR("fprintf() to pidfile error.");
        fclose(fp);
        return 1;
    }
    fclose(fp);
    return 0;
}

int AppMain::removePidfile()
{
    if (mPidfile.empty())
    {
        return 0;
    }

    boost::filesystem::remove_all(mPidfile);

    return 0;
}

CBASE_END
