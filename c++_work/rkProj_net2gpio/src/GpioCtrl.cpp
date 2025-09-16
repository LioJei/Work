//
// Created by Administrator on 2025/4/10.
//

#include "GpioCtrl.h"

GpioCtrl::GpioCtrl(const char *pin) : m_pin(pin) {
    FILE *fp = fopen("/sys/class/gpio/export", "w");
    if (fp == NULL) {
        perror("Failed to open export");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", m_pin);
    fclose(fp);
}

GpioCtrl::~GpioCtrl() {
    FILE *fp = fopen("/sys/class/gpio/unexport", "w");
    if (fp == NULL) {
        perror("Failed to open unexport");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", m_pin);
    fclose(fp);
}

void GpioCtrl::setDirection(const char *direction) {
    char path[35];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", m_pin);
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        perror("Failed to open direction");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", direction);
    fclose(fp);
}

const char *GpioCtrl::readDirection() {
    static char direction[4]; // "in" 或 "out" + null terminator
    char path[35];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", m_pin);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Failed to open direction");
        exit(EXIT_FAILURE);
    }
    fscanf(fp, "%3s", direction); // 读取方向
    fclose(fp);

    return direction; // 返回方向
}

void GpioCtrl::setValue(const char *level) {
    char path[30];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", m_pin);
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        perror("Failed to open value");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", level);
    fclose(fp);
}

char GpioCtrl::readValue() {
    char path[30];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", m_pin);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Failed to open value");
        exit(EXIT_FAILURE);
    }
    char value;
    fscanf(fp, "%c", &value);
    fclose(fp);

    return value;
}