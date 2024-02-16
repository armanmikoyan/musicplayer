#include "socket.hpp"

socket::socket()
    : tcp_socket(new QTcpSocket(this))
{
    connect(tcp_socket, &QTcpSocket::connected, this, &socket::onConnected);
    connect(tcp_socket, &QTcpSocket::readyRead, this, &socket::response);
    connectToServer();
}

socket::~socket()
{
    tcp_socket->disconnectFromHost();
    tcp_socket->waitForDisconnected();
    delete tcp_socket;
}

QString socket::response()
{
    QString data = tcp_socket->readAll();
    qDebug() << data << "\n";
    return data;
}

void socket::request_for_music()
{
    QString req = "meta_music";
    tcp_socket->write(req.toUtf8());
}

void socket::connectToServer()
{
    tcp_socket->connectToHost("127.0.0.1", 8080);
}

void socket::onConnected()
{
    qDebug() << "Connected to server";
}

