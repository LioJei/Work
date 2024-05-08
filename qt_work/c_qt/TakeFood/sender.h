//
// Created by Administrator on 2024/5/8.
//

#ifndef TAKEFOOD_SENDER_H
#define TAKEFOOD_SENDER_H
#include <QString>
#include <QTcpSocket>

class Sender {
public:
    explicit Sender(QString  sender, QString  receiver, QString  passwd, QString  server, int port);
    ~Sender();
    bool setMsg(const QString& subject, const QString& body);

private:
    QString m_sender;
    QString m_receiver;
    QString m_passwd;
    QString m_server;
    int m_port{};
    QString m_subject;
    QString m_body;
};


#endif //TAKEFOOD_SENDER_H
