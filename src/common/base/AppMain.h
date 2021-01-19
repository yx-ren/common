#ifndef __COMMON_BASE_APP_MAIN_H__
#define __COMMON_BASE_APP_MAIN_H__

#include <functional>
#include <list>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <boost/function.hpp>
#include <boost/program_options.hpp>

#include <common/base/common.h>
#include <common/base/IBaseObject.h>
#include <common/base/cbase_export.h>
#include <common/base/logger/Logger.h>
#include <common/base/logger/LoggerManager.h>

#ifdef WIN32
#define CNTSERVICE_EXT_CLASS __declspec(dllimport)
#include <cntservice/ntserv.h>
#endif

using namespace CBASE_LOGGER_NAMESPACE;
namespace po = boost::program_options;

CBASE_BEGIN
#ifdef WIN32
class CBASE_EXPORT AppMain : public IBaseObject, public CNTService
#else
class CBASE_EXPORT AppMain: public IBaseObject
#endif
{
public:
#ifdef WIN32
    AppMain(
        int argc, char *argv[],
        LPCTSTR lpszServiceName,
        LPCTSTR lpszDisplayName,
        DWORD dwControlsAccepted,
        LPCTSTR lpszDescription = NULL);
    /* Will call Logger::initialize(logPara) to support new DLPServer's log format. */
    AppMain(
        int argc, char *argv[],
        LPCTSTR lpszServiceName,
        LPCTSTR lpszDisplayName,
        DWORD dwControlsAccepted,
        LoggerParameter &logPara,
        LPCTSTR lpszDescription = NULL);

#else
    AppMain(int argc, char *argv[], bool ignoreChildSignal = false);
    /* Will call Logger::initialize(logPara) to support new DLPServer's log format. */
    AppMain(int argc, char *argv[], LoggerParameter &logPara, bool ignoreChildSignal = false);
#endif
    virtual ~AppMain();

    /* will call Logger::initialize(logPara) to support new DLPServer's log format.
    *  But now this interface has been abandoned, cause AppMain() has already initialize Logger.
    * */
    bool logInit(LoggerParameter &logPara);
    static AppMain * getInstance();

    virtual int exec();
    virtual int quit();
    virtual int reloadConfiguration();
    virtual std::string getUsage() const;

    bool appendPreHandler(boost::function< void (AppMain *)> handler);
    bool appendPostHandler(boost::function< void (AppMain *)> handler);

    std::string toString() const;

#ifndef WIN32
    void processSignal(sigset_t sigset);
    void ignoreChildSignal(bool ignore = true);
    bool isChildSignalIgnored() const
    {
        return mIgnoreChildSignal;
    }
    void blockChildSignal();
    void unblockChildSignal();
    void checkSignalBlockPending();
    virtual int childProcessExit(pid_t pid);
#endif // NOT WIN32
    bool isCoreDumpEnabled()
    {
        return mCoreDumpEnabled;
    }

    bool setupSignalProcessing();
    void teardownSignalProcessing();
    bool daemonize();
    //virtual po::options_description getOptionsDescription(std::string &pidfile, std::string &cfg_file) const;
    virtual po::options_description getOptionsDescription() const;
    virtual int run();

#ifdef WIN32
    /* implement your windows service fnuction here*/
    virtual void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv){ run(); }
    //virtual void OnStop();
    //virtual void OnPause();
    //virtual void OnContinue();
    //......
private:
    bool install(DWORD &errorBits);
    bool uninstall(DWORD &errorBits);
#endif
private:
    int winServiceOperation();
    int createPidfile();
    int removePidfile();

protected:
    int mArgCount;
    char **mArgs;
    std::string mPidfile;
    std::string mCfgfile;
    po::variables_map mVariablesMap;
    std::list<boost::function< void (AppMain *)> > mPreHandlers;
    std::list<boost::function< void (AppMain *)> > mPostHandlers;
#ifndef WIN32 
    boost::shared_ptr<boost::thread> mSignalThread;
    std::atomic_bool mSignalQuit;
    std::atomic_bool mIgnoreChildSignal;
    std::atomic_bool mChildSignalBlocked;
    bool mSignalBlockPending;
    std::mutex       mSignalBlockLock;
    std::condition_variable mSignalBlockCond;
#endif // NOT WIN32 
    bool mCoreDumpEnabled;


    static AppMain * gInstance;
};

CBASE_END

#endif // SKYGUARD_BASE_APP_MAIN_H_H
