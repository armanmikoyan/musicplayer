#include "music_player.hpp"

music_player::music_player(Ui::MainWindow *ui, QObject *parent)
    : QObject{parent}
    , ui{ui}
    , socket{new QTcpSocket}
{
    setup_player();
    socket_connections();
    initial_requests_queue();
}

// network manipulations

void music_player::socket_connections() const
{
    connect(socket, &QTcpSocket::connected, this, &music_player::initialRequestStart);
    connect(socket, &QTcpSocket::readyRead, this, &music_player::initial_requests_handler);
    connect(socket, &QTcpSocket::disconnected, this, &music_player::Ondisconected);
    socket->connectToHost("127.0.0.1", 8080);
}

void music_player::initialRequestStart()
{
    next_request();
    qDebug() << "Connected to the server-------\n";
}

void music_player::Ondisconected()
{
    qDebug() << "Disconnected to the server\n";
}

void music_player::initial_requests_handler()
{
    QString current_request = request_queue.front();
    request_queue.pop();
    if (current_request == "get_meta_music")
    {
        QString names = socket->readAll();
        qDebug() <<"Response: " << names << "\n";
        response_music_names_ui(names);
    }
    // handle andother meta requests...

     // swiching to the stream
    if (request_queue.empty())
    {
        socket->disconnectFromHost();
        disconnect(socket, &QTcpSocket::connected, this, &music_player::initialRequestStart);
        disconnect(socket, &QTcpSocket::readyRead, this, &music_player::initial_requests_handler);
        stream_connections();
    }
    else
    {
        next_request();
    }
}

void music_player::initial_requests_queue()
{
    request_queue.push("get_meta_music");
}

void music_player::next_request()
{
    socket->write(request_queue.front().toUtf8());
}

void music_player::response_music_names_ui(const QString& names)
{
    music_files = names.split("\n", Qt::SkipEmptyParts);
    for (auto& file : music_files)
    {
        ui->music_list->addItem(file);
    }

    ui->length->setText(QString::number(music_files.size()));
    ui->curr_id->setText("1");
}
// ------------------ end intial requests



// stream manipulations

void music_player::stream_connections() const
{
    connect(socket, &QTcpSocket::connected, this, &music_player::streamRequest);
    connect(this, &music_player::request, this, &music_player::streamRequest);
    connect(socket, &QTcpSocket::readyRead, this, &music_player::OnReadyStream);
    connect(ui->music_list, &QListWidget::itemClicked, this, &music_player::onMusicClicked);
}

void music_player::OnReadyStream()
{
    stream_music();
}

void music_player::setup_player()
{
    audioOutput = new QAudioOutput;
    media_player = new QMediaPlayer;
    media_player->setAudioOutput(audioOutput);
}

void music_player::stream_music()
{

    QByteArray data = socket->readAll();
    if (!buffer || media_player->mediaStatus() != QMediaPlayer::BufferedMedia &&
                   media_player->mediaStatus() != QMediaPlayer::LoadingMedia)
    {
        reset_player();
    }

    buffer->buffer().append(data);

    if (!buffer->isOpen())
    {
        buffer->open(QIODevice::ReadOnly);
    }

    const qint64 min_size = 1024 * 10;
    if (buffer->size() >= min_size)
    {
        media_player->setPosition(0);
        media_player->play();
    }
}

void music_player::streamRequest()
{
    socket->write(current_music_name.toUtf8());
    qDebug() << "Request sent... " << current_music_name << "\n";
}

void music_player::onMusicClicked(QListWidgetItem* item)
{
    current_music_name = item->text();
    for (int i = 0; i < music_files.size(); ++i)
    {
        if (music_files[i] == current_music_name)
        {
            current_music_index = i;
        }
    }

    ui->current_music->setText(current_music_name);
    ui->curr_id->setText(QString::number(current_music_index + 1));

    if (socket->state() != QTcpSocket::ConnectedState)
    {
        socket->connectToHost("127.0.0.1", 8080);
    }
    else
    {
        reset_player();
        emit request();
    }
    qDebug() << current_music_name;
}

void music_player::reset_player()
{
    if (buffer)
    {
        delete buffer;
    }

    buffer = new QBuffer(this);
    media_player->setSourceDevice(buffer);
}





