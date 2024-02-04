#pragma once

#include <QDebug>

namespace Logger {

#define MY_LOG_DEBUG qDebug() <<"DEBUG"<<"--"<<__FILE__ << "--"<<__FUNCTION__ <<"--"<< __LINE__<<": "
#define MY_LOG_INFO qInfo() << "INFO"<<"--"<<__FILE__ << "--"<<__FUNCTION__ <<"--"<< __LINE__<<": "
#define MY_LOG_WARN qWarning() << "WARNING"<<"--"<<__FILE__ << "--"<<__FUNCTION__ <<"--"<< __LINE__<<": "
#define MY_LOG_CRIT qCritical() << "CRITICAL"<<"--"<<__FILE__ << "--"<<__FUNCTION__ <<"--"<< __LINE__<<": "

    void initLog(const QString &logPath = QStringLiteral("Log"), int logMaxCount = 1024, bool async = true);

} // namespace Logger
