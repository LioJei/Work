#include "Logger.h"

Logger::Logger(const std::string &filename, std::size_t maxFileSize)
        : maxFileSize(maxFileSize) {
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "无法打开日志文件: " << filename << std::endl;
        throw std::runtime_error("无法打开日志文件: " + filename); // 增加错误处理机制
    }
    if (pthread_mutex_init(&data_mutex, nullptr) != 0) { // 检查互斥锁初始化是否成功
        std::cerr << "互斥锁初始化失败" << std::endl;
        logFile.close(); // 确保在互斥锁初始化失败时关闭文件
        throw std::runtime_error("互斥锁初始化失败"); // 增加错误处理机制
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
    pthread_mutex_destroy(&data_mutex);
}

std::string Logger::getFileName(const std::string &fullPath) {
    // 找到最后一个斜杠的位置
    size_t lastSlash = fullPath.find_last_of("/\\");
    // 如果找到了斜杠，就返回斜杠后面的部分；否则返回完整路径
    return (lastSlash == std::string::npos) ? fullPath : fullPath.substr(lastSlash + 1);
}

void Logger::log(Level level,  const std::string &message, const std::string &file, int line) {
    std::string levelStr = getLevelString(level);
    std::string colorCode = getColorCode(level);
    // 获取当前时间
    std::string timestamp = getCurrentTime();
    std::string fileName = getFileName(file);
    pthread_mutex_lock(&data_mutex);
    // 输出到控制台
    std::cout << std::left << colorCode << "[" << std::setw(5) << levelStr << "]"
              << " " << std::setw(20) << timestamp
              << " " << std::setw(15) << fileName
              << " " << std::setw(3) << line
              << " " << message
              << "\033[0m" << std::endl;

    // 写入到文件
    if (logFile.is_open()) {
        logFile << std::left << "[" << std::setw(5) << levelStr << "]"
                << " " << std::setw(20) << timestamp
                << " " << std::setw(15) << fileName
                << " " << std::setw(3) << line
                << " " << message << std::endl;

        try {
            checkFileSize();
        } catch (const std::runtime_error &e) {
            std::cerr << "文件大小检查时出错: " << e.what() << std::endl;
            pthread_mutex_unlock(&data_mutex);
            throw; // 重新抛出异常，确保调用者也能接收到错误信息
        }
    }
    pthread_mutex_unlock(&data_mutex);
}

std::string Logger::getCurrentTime() {
    auto now = std::time(nullptr);
    auto localTime = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::getLevelString(Level level) {
    switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARN:
            return "WARN";
        case ERROR:
            return "ERROR";
        case FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

std::string Logger::getColorCode(Level level) {
    switch (level) {
        case DEBUG:
            return "\033[37m"; // 白色
        case INFO:
            return "\033[32m";  // 绿色
        case WARN:
            return "\033[33m";  // 黄色
        case ERROR:
            return "\033[31m"; // 红色
        case FATAL:
            return "\033[41m"; // 红色背景
        default:
            return "\033[0m";     // 默认
    }
}

void Logger::checkFileSize() {
    logFile.seekp(0, std::ios::end);
    if (logFile.tellp() > maxFileSize) {
        logFile.close();
        // 找到文件名
        std::string filename = FILE_NAME;
        // 重命名文件，如果文件已存在，则增加后缀
        std::string newFilename = filename + ".1";
        int fileIndex = 1;
        while (std::ifstream(newFilename)) {
            fileIndex++;
            newFilename = filename + "." + std::to_string(fileIndex);
        }
        if (rename(filename.c_str(), newFilename.c_str()) != 0) {
            std::cerr << "无法重命名日志文件: " << filename << std::endl;
            throw std::runtime_error("无法重命名日志文件: " + filename);
        }
        logFile.open(filename, std::ios::trunc); // 以覆盖模式重新打开原文件
        if (!logFile.is_open()) {
            std::cerr << "无法以覆盖模式重新打开日志文件: " << filename << std::endl;
            throw std::runtime_error("无法以覆盖模式重新打开日志文件: " + filename);
        }
    }
}