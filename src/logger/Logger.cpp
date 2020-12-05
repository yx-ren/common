SGLD_BEGIN
void Logger::init(const LoggerParameter& param)
{
    Properties props;
    std::string access_log_value = param.level;
    std::string access_log_pattern = "%d [%t] <wfe>: %-5p %.32c - %m%n";
    std::string access_log_file = param.file_path;

    // init file
    if (param.file_flag)
    {
        access_log_value += ", file";
        props.put("log4j.appender.file", "org.apache.log4j.RollingFileAppender");
        props.put("log4j.appender.file.layout", "org.apache.log4j.Patternlayout");
        props.put("log4j.appender.file.layout.ConversionPattern", access_log_pattern.c_str());
        props.put("log4j.appender.file.File", access_log_file.c_str());
        props.put("log4j.appender.file.MaxFileSize", "10MB");
        props.put("log4j.appender.file.MaxBackupIndex", "10");
    }

    // init console
    if (param.console_flag)
    {
        access_log_value += ", console";
        props.put("log4j.appender.console", "org.apache.log4j.ConsoleAppender");
        props.put("log4j.appender.console.layout", "org.apache.log4j.PatternLayout");
        props.put("log4j.appender.console.layout.ConversionPattern", access_log_pattern.c_str());
    }

    props.put("log4j.logger." + param.logger_tag, access_log_value.c_str());
    PropertyConfigurator::configure(props);

    mLogger = log4cxx::Logger::getLogger(param.logger_tag);
}

log4cxx::LoggerPtr Logger::get_logger() const
{
    return mLogger;
}
SGLD_BEGIN
