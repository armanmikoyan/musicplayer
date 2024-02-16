#ifndef SOCKET_REQUEST_HPP
#define SOCKET_REQUEST_HPP

#include <QTcpSocket>

#include <QObject>

class socket_request : public QObject
{
    Q_OBJECT
public:
    socket_request();
    ~socket_request();

public slots:

private:
    void connection_to_server();

public:
    void request(const QByteArray&);
    QByteArray read();

private:
    QTcpSocket* socket;
};

#endif // SOCKET_REQUEST_HPP
