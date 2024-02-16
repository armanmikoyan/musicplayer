#include "socket_request.hpp"

socket_request::socket_request()
{
    socket = new QTcpSocket();
    connection_to_server();
}

socket_request::~socket_request()
{
    socket->disconnectFromHost();
    delete socket;
}


void socket_request::connection_to_server()
{
    socket->connectToHost("127.0.0.1", 8080);

    if (!socket->waitForConnected())
    {
        qDebug() << "connection failed\n";
    }
    else
    {
        qDebug() << "connection succeded\n";
    }
}

void socket_request::request(const QByteArray& req)
{
    socket->write(req);
    socket->waitForBytesWritten();
}

QByteArray socket_request::read()
{
    QByteArray resp;
    while (socket->bytesAvailable() > 0)
    {
        resp += socket->readAll();
    }

    return resp;
}



