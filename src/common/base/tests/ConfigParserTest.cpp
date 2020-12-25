#include <iostream>
#include <boost/filesystem.hpp>
#include <common/base/common.h>
#include <common/base/ConfigParser.h>

using namespace CBASE_NAMESPACE;
namespace fs = boost::filesystem;

int main(int argc, const char* argv[])
{
    std::string xml_path = "./etc/test.xml";
    if (!fs::exists(xml_path))
    {
        std::cout << "xml file:" << xml_path << " not existed" << std::endl;
        return -1;
    }

    ConfigParser configParser(xml_path);
    std::string level = configParser.getValue<std::string>("config.logging.level");
    BOOST_ASSERT(level == "DEBUG");
    bool enabled = configParser.getValue<bool>("config.components.component[name=\"DownloadService\"].enabled");
    BOOST_ASSERT(!enabled);
    bool licensed = configParser.getValue<bool>("config.components.component[name=\"DownloadService\"].licensed");
    BOOST_ASSERT(licensed);

    std::string  value1 = configParser.getValue<std::string>("config.testChinese");
    BOOST_ASSERT(value1 == "中国");
    std::string  value2 = configParser.getValue<std::string>("config.测试结点");
    BOOST_ASSERT(value2 == "中央");
    std::string  value3 = configParser.getValue<std::string>("config.testChinese1[name=\"cnode\"]");
    BOOST_ASSERT(value3 == "电视台");

    return 0;
}
