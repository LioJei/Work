//
// Created by Administrator on 2024/5/8.
//

#include "sender.h"

#include <utility>

Sender::Sender(QString sender, QString receiver, QString passwd, QString server, int port)
        : m_sender(std::move(sender)),
          m_receiver(std::move(receiver)),
          m_passwd(std::move(passwd)),
          m_server(std::move(server)),
          m_port(port),
          m_subject(""),
          m_body("") {
}

bool Sender::setMsg(const QString& subject, const QString& body) {
    QString email = m_sender;
    QString smtpServer = m_server;
    int smtpPort = m_port; //端口号
    QString senderEmail = m_sender;
    QString senderPassword = m_passwd;

    // Connect to the SMTP server
    QTcpSocket socket;	//连接方式。这里选用TCP方式，也可以选择SSL加密方式
    socket.connectToHost(smtpServer, smtpPort);
    if (!socket.waitForConnected())
    {
        qDebug() << "Failed to connect to SMTP server.";
        return false;
    }
    else
        qDebug() << "Success to connect to SMTP server.";

    // Read the welcome message from the server
    if (!socket.waitForReadyRead())
    {
        qDebug() << "Failed to receive welcome message from SMTP server.";
        return false;
    }
    QByteArray response = socket.readAll();
    qDebug() << response;

    // Send the EHLO command
    socket.write("EHLO localhost\r\n");
    if (!socket.waitForBytesWritten())
    {
        qDebug() << "Failed to send EHLO command to SMTP server.";
        return false;
    }
    if (!socket.waitForReadyRead())
    {
        qDebug() << "Failed to receive EHLO command response from SMTP server.";
        return false;
    }
    response = socket.readAll();
    qDebug() << response;

    // Send the authentication login command and credentials
    socket.write("AUTH LOGIN\r\n");
    if (!socket.waitForBytesWritten())
    {
        qDebug() << "Failed to send AUTH LOGIN command to SMTP server.";
        return false;
    }
    if (!socket.waitForReadyRead())
    {
        qDebug() << "Failed to receive AUTH LOGIN command response from SMTP server.";
        return false;
    }
    response = socket.readAll();
    qDebug() << response;

    QByteArray encodedEmail = senderEmail.toUtf8().toBase64();
    QByteArray encodedPassword = senderPassword.toUtf8().toBase64();

    socket.write(encodedEmail + "\r\n");
    if (!socket.waitForBytesWritten())
    {
        qDebug() << "Failed to send encoded email to SMTP server.";
        return false;
    }
    if (!socket.waitForReadyRead())
    {
        qDebug() << "Failed to receive email authentication response from SMTP server.";
        return false;
    }
    response = socket.readAll();
    qDebug() << response;

    socket.write(encodedPassword + "\r\n");
    if (!socket.waitForBytesWritten())
    {
        qDebug() << "Failed to send encoded password to SMTP server.";
        return false;
    }
    if (!socket.waitForReadyRead())
    {
        qDebug() << "Failed to receive password authentication response from SMTP server.";
        return false;
    }
    response = socket.readAll();
    qDebug() << response;

    // Send the MAIL FROM command
    socket.write(QString("MAIL FROM:<%1>\r\n").arg(senderEmail).toUtf8());
    if (!socket.waitForBytesWritten())
    {
        qDebug() << "Failed to send MAIL FROM command to SMTP server.";
        return false;
    }
    if (!socket.waitForReadyRead())
    {
        qDebug() << "Failed to receive MAIL FROM command response from SMTP server.";
        return false;
    }
    response = socket.readAll();
    qDebug() << response;

    // Send the RCPT TO command
    socket.write(QString("RCPT TO:<%1>\r\n").arg(email).toUtf8());
    if (!socket.waitForBytesWritten())
    {
        qDebug() << "Failed to send RCPT TO command to SMTP server.";
        return false;
    }
    if (!socket.waitForReadyRead())
    {
        qDebug() << "Failed to receive RCPT TO command response from SMTP server.";
        return false;
    }
    response = socket.readAll();
    qDebug() << response;

    // Send the DATA command
    socket.write("DATA\r\n");
    if (!socket.waitForBytesWritten())
    {
        qDebug() << "Failed to send DATA command to SMTP server.";
        return false;
    }
    if (!socket.waitForReadyRead())
    {
        qDebug() << "Failed to receive DATA command response from SMTP server.";
        return false;
    }
    response = socket.readAll();
    qDebug() << response;

    // Send the message headers and body
    QString message = "To: " + email + "\r\n";
    message += "From: " + senderEmail + "\r\n";
    message += "Subject: " + subject + "\r\n";
    message += "Content-Type: text/plain; charset=\"utf-8\"\r\n\r\n";
    message += body + "\r\n";

    socket.write(message.toUtf8());
    socket.write(".\r\n");
    if (!socket.waitForBytesWritten())
    {
        qDebug() << "Failed to send message to SMTP server.";
        return false;
    }
    qDebug() << "Message sent successfully.";

    // Quit the session
    socket.write("QUIT\r\n");
    socket.waitForBytesWritten();

    // Close the connection
    socket.close();
    return true;
}

Sender::~Sender()= default;
