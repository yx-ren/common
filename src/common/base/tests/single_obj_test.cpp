#include <thread>
#include <vector>
#include <random>
#include <common/base/SingleObject.h>
#include <common/base/logger/Logger.h>

using CBASE_NAMESPACE::SingleObject;
using namespace CBASE_LOGGER_NAMESPACE;

class IRandomFactory : public SingleObject<IRandomFactory>
{
public:
    virtual ~IRandomFactory() {}

    uint32_t generate_random()
    {
        return random();
    }

private:
    std::default_random_engine random;
};

int main(int argc, char* argv[])
{
    LoggerParameter logger_param;
    logger_param.file_path = "./SingleObjecTest.log";
    Logger logger;
    logger.init(logger_param);

    auto thread_generate_random = [&] ()
    {
        auto random_factory = IRandomFactory::getInstance();
        LOG4CXX_INFO(logger.get_logger(),
                "single object address:[" << random_factory << "], "
                "generate random:" << random_factory->generate_random());
    };

    std::vector<std::shared_ptr<std::thread>> threads;
    int thread_numbers = 10;
    for (int i = 0; i != thread_numbers; i++)
        threads.push_back(std::make_shared<std::thread>(thread_generate_random));

    for (auto& thread : threads)
        thread->join();

    return 0;
}

