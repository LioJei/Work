//
// Created by Administrator on 2025/4/10.
//

#include "GpioCtrl.h"

#include <utility>

GpioCtrl::GpioCtrl(const char *pin, std::shared_ptr<Logger> logger) :
m_pin(pin),
m_logger(std::move(logger))
{
    if (gpioWrite("export", m_pin) != 0) {
        exit(EXIT_FAILURE);
    }
}

GpioCtrl::~GpioCtrl() {
    if (gpioWrite("unexport", m_pin) != 0) {

    }
}

void GpioCtrl::setDirection(const char *direction) {
    if (gpioWrite("direction", direction) != 0) {
        exit(EXIT_FAILURE);
    }
}

const char *GpioCtrl::readDirection() {
    static char direction[4]; // "in" 或 "out" + null terminator
    if (gpioRead("direction", direction) != 0) {
        return nullptr; // 或者您可以选择其他合适的处理方式
    }
    return direction;
}

void GpioCtrl::setValue(const char *level) {
    if (gpioWrite("value", level) != 0) {
        exit(EXIT_FAILURE);
    }
}

char GpioCtrl::readValue() {
    char value[2]; // 用于存储字符和null terminator
    if (gpioRead("value", value) != 0) {
        return '\0'; // 或者其他合适的错误标志
    }
    return value[0];
}

int GpioCtrl::gpioWrite(const char *control, const char *data) {
    char path[40] = "/sys/class/gpio/";
    if (strcmp(control, "export") == 0 || strcmp(control, "unexport") == 0) {
        strncat(path, control, sizeof(path) - strlen(path) - 1);
    } else {
        strncat(path, "gpio", sizeof(path) - strlen(path) - 1);
        strncat(path, m_pin, sizeof(path) - strlen(path) - 1);
        strncat(path, "/", sizeof(path) - strlen(path) - 1);
        strncat(path, control, sizeof(path) - strlen(path) - 1);
    }

    FILE *fp = fopen(path, "w");
    if (fp == nullptr) {
        LOG(m_logger, Logger::ERROR, "Failed to open %s", path + std::string(strerror(errno)));
        return -1;
    }

    if (fprintf(fp, "%s", data) < 0) {
        LOG(m_logger, Logger::ERROR, "Failed to write to %s", path + std::string(strerror(errno)));
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int GpioCtrl::gpioRead(const char *control, char *data) {
    char path[40] = "/sys/class/gpio/";
    strncat(path, "gpio", sizeof(path) - strlen(path) - 1);
    strncat(path, m_pin, sizeof(path) - strlen(path) - 1);
    strncat(path, "/", sizeof(path) - strlen(path) - 1);
    strncat(path, control, sizeof(path) - strlen(path) - 1);

    FILE *fp = fopen(path, "r");
    if (fp == nullptr) {
        LOG(m_logger, Logger::ERROR, "Failed to open %s", path + std::string(strerror(errno)));
        return -1;
    }

    if (fscanf(fp, "%3s", data) != 1) {
        LOG(m_logger, Logger::ERROR, "Failed to read from %s", path + std::string(strerror(errno)));
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}