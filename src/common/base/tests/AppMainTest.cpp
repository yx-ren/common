#include <iostream>
#include <common/base/common.h>
#include <common/base/AppMain.h>
#include <common/base/ScopedGuard.h>

using namespace std;
using namespace CBASE_NAMESPACE;
using namespace CBASE_LOGGER_NAMESPACE;

class MyApp : public AppMain
{
  public:
    MyApp(int argc, char *argv[])
        : AppMain(argc, argv)
    {
        
    }

    int run()
    {
        blockChildSignal();
        ON_SCOPE_EXIT([&] {
                unblockChildSignal();
            });
        FILE *fp = ::popen("ls -l /usr/lib", "r");
        char buffer[4096];
        size_t sz = fread(buffer, sizeof(buffer), 1, fp);
        std::cout << "read " << sz << " bytes." << std::endl;
        ::sleep(5);
        int rv = ::pclose(fp);

        std::cout << "rv = " << rv << std::endl;
        return rv;
    }
    
};

int main(int ac, const char* av[])
{
    CB_TRACE_FUNCTION();
    char *argv[] = {
        "MyApp",
        NULL,
    };
    MyApp app(1, argv);

    BOOST_ASSERT(app.exec() == 0);

    return 0;
}
