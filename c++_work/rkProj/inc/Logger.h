/**
* Copyright (c) 2025 cdly.Co.,Ltd. All rights reserved.
* @brief 日志类，主要保存程序运行期间的重要信息
* @author lijunjie
* @date 2025/4/10
*/
#ifndef LOGGER_H
#define LOGGER_H

#include "headfile.h"
#define LOG(logger, level, message) logger->log(level, message, __FILE__, __LINE__)

class Logger {
public:
    //log等级
    enum Level {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    /**
     * @brief: 初始化构造，声明保存日志信息的文件名，以及文件最大大小
     * @param[in]: filename(文件名)
     * @param[in]: maxFileSize(文件最大大小)
     * */
    Logger(const std::string &filename, std::size_t maxFileSize);

    /**
     * @brief: 析构函数，在析构里面关闭文件流
     * */
    ~Logger();

    /**
     * @brief: 日志记录接口
     * @param[in]: level(日志信息等级)
     * @param[in]: message(需要记录的信息)
     * */
    void log(Level level,  const std::string &message, const std::string &file, int line);

private:
    pthread_mutex_t data_mutex{};
    std::ofstream logFile;
    std::size_t maxFileSize;

    /**
     * @brief: 获取日志记录的当下时间，为内部接口，不对外开放
     * */
    static std::string getCurrentTime();

    /**
     * @brief: 转换日志记录的当下等级，为内部接口，不对外开放
     * @param[in]: level(日志等级)
     * */
    static std::string getLevelString(Level level);

    /**
     * @brief: 转换日志记录的等级颜色，为内部接口，不对外开放
     * @param[in]: level(日志等级)
     * */
    static std::string getColorCode(Level level);

    /**
     * @brief: 日志文件的大小控制，超过大小开始覆盖
     * */
    void checkFileSize();
};

#endif // LOGGER_H