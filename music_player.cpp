#include "music_player.hpp"


music_player::music_player(Ui::MainWindow *ui, QObject *parent)
    : QObject{parent}
    , ui{ui}
    , play_button_state{false}
    , playIcon("/Users/armanmikoyan/Pictures/play_btn.png")
    , stopIcon("/Users/armanmikoyan/Pictures/stop_btn.png")
    , nextIcon("/Users/armanmikoyan/Pictures/next_btn.png")
    , prevtIcon("/Users/armanmikoyan/Pictures/prev_btn.png")
{
    setup_music_playlist();
    ui_render();
    connections();
}

void music_player::ui_render()
{
    ui->play_btn->setIcon(playIcon);
    ui->next_btn->setIcon(nextIcon);
    ui->prev_btn->setIcon(prevtIcon);

    ui->play_btn->setIconSize(QSize(30, 30));
    ui->next_btn->setIconSize(QSize(30, 30));
    ui->prev_btn->setIconSize(QSize(30, 30));
}

void music_player::setup_music_playlist()
{
    socket = new QTcpSocket;
    socket->connectToHost("127.0.0.1", 8080);
    connect(socket, &QTcpSocket::connected, this, &music_player::get_music_names);
    connect(socket, &QTcpSocket::readyRead, this, &music_player::draw_music_names);
    media_player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput();
}

void music_player::get_music_names()
{
    qDebug() << "connected to server\n";
    socket->write("meta_music");
}

void music_player::draw_music_names()
{
    QString names = socket->readAll();
    music_files = names.split("\n", Qt::SkipEmptyParts);
    for (auto& file : music_files)
    {
        ui->music_list->addItem(file);
    }
    socket->close();
    ui->length->setText(QString::number(music_files.size()));
    ui->curr_id->setText("1");
}

void music_player::connections() const
{
    connect(ui->play_btn, &QPushButton::clicked, this, &music_player::play_btn_clicked);
    connect(ui->next_btn, &QPushButton::clicked, this, &music_player::next_btn_clicked);
    connect(ui->prev_btn, &QPushButton::clicked, this, &music_player::prev_btn_clicked);
    connect(ui->vertical_slider, &QSlider::actionTriggered, this, &music_player::vertical_slider_triggered);
    connect(ui->music_list, &QListWidget::itemClicked, this, &music_player::music_clicked);
}

void music_player::music_clicked(QListWidgetItem* item)
{
    ui->play_btn->setIcon(stopIcon);

    current_music_name = item->text();
    for (int i = 0; i < music_files.size(); ++i)
    {
        if (music_files[i] == current_music_name)
        {
            current_music_index = i;
        }
    }
    change_current_music(current_music_name);

}

void music_player::get_music()
{
    socket->write(current_music_name.toUtf8());
}

void music_player::stream_music()
{
    QByteArray data = socket->readAll();
    if (!buffer ||
            media_player->mediaStatus() != QMediaPlayer::BufferingMedia &&
            media_player->mediaStatus() != QMediaPlayer::BufferedMedia &&
            media_player->mediaStatus() != QMediaPlayer::LoadingMedia)
    {
        reset_buffer_and_media_player();
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

void music_player::reset_buffer_and_media_player()
{
    if (buffer)
    {
        delete buffer;
    }
    buffer = new QBuffer(this);
    media_player->setAudioOutput(audioOutput);
    media_player->setSourceDevice(buffer);
    ui->current_music->setText(current_music_name);
}

void music_player::change_current_music(const QString &new_music_name)
{
    QString MESSAGE = "Playing " + new_music_name;
    ui->statusbar->showMessage(MESSAGE, 5000);

    if (media_player->mediaStatus() == QMediaPlayer::BufferedMedia ||
        media_player->mediaStatus() == QMediaPlayer::BufferingMedia ||
        media_player->mediaStatus() == QMediaPlayer::LoadingMedia)
    {
        media_player->stop();
        buffer->close();
        buffer->setData(QByteArray());
    }

    reset_buffer_and_media_player();

    socket = new QTcpSocket;
    socket->connectToHost("127.0.0.1", 8080);

    connect(socket, &QTcpSocket::connected, this, &music_player::get_music);
    connect(socket, &QTcpSocket::readyRead, this, &music_player::stream_music);

    buffer->seek(0);
    media_player->setPosition(0);
}


void music_player::play_btn_clicked(bool clicked)
{
    if(media_player->playbackState() == QMediaPlayer::PausedState)
    {
        media_player->play();
        ui->play_btn->setIcon(stopIcon);
    }
    else if(media_player->playbackState() == QMediaPlayer::PlayingState)
    {
        media_player->pause();
        ui->play_btn->setIcon(playIcon);
    }
    else
    {
        ui->statusbar->showMessage("Choose music", 2000);
    }
}

void music_player::next_btn_clicked()
{
    ui->play_btn->setIcon(stopIcon);
    ++current_music_index;
    if (current_music_index >= music_files.size())
    {
        current_music_index = 0;
    }
    ui->curr_id->setText(QString::number(current_music_index + 1));
    current_music_name = music_files[current_music_index];

    change_current_music(current_music_name);
}


void music_player::prev_btn_clicked()
{
    ui->play_btn->setIcon(stopIcon);
    --current_music_index;
    if (current_music_index < 0)
    {
        current_music_index = music_files.size() - 1;
    }
    ui->curr_id->setText(QString::number(current_music_index + 1));
    current_music_name = music_files[current_music_index];
    change_current_music(current_music_name);

}

void music_player::vertical_slider_triggered(int action)
{
    int value = ui->vertical_slider->value();
    float volume = static_cast<float>(value) /  100.0;
    audioOutput->setVolume(volume);
}


