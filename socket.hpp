#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "QObject"
#include <QTcpSocket>

class socket : public QObject
{
    Q_OBJECT

public:
    socket();
    ~socket();

public slots:
    void onConnected();
public:
    void connectToServer();

public:
    void request_for_music();
    QString response();

private:
    QTcpSocket* tcp_socket;
};

#endif // SOCKET_HPP
