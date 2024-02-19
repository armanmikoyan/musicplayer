#ifndef MUSIC_PLAYER_HPP
#define MUSIC_PLAYER_HPP

#include <QMainWindow>
#include <QObject>
#include "ui_MainWindow.h"
#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QFileInfo>
#include <QAudioOutput>
#include <QPushButton>
#include <QListWidget>
#include <QTcpSocket>
#include <QCoreApplication>
#include <Qbuffer>
#include <queue>


class music_player : public QObject
{
    Q_OBJECT

public:
    explicit music_player(Ui::MainWindow*, QObject *parent = nullptr);


// network manipulations (slots);
private slots:
    void initialRequestStart();
    void OnReadyStream();
    void initial_requests_handler();
    void Ondisconected();

// network manipulations (helpers);
private:
    void socket_connections() const;
    void next_request();
    void initial_requests_queue();

// response handlers
private:
    void response_music_names_ui(const QString&);

private:
// stream manipulations
    void stream_connections() const;
    void setup_player();
    void reset_player();
    void stream_music();

// stream slots
private slots:
    void onMusicClicked(QListWidgetItem*);
    void streamRequest();

signals:
    void request();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *media_player;
    QAudioOutput *audioOutput;
    QStringList music_files;
    static inline int current_music_index = 0;
    QTcpSocket *socket;
    QString current_music_name;
    QBuffer *buffer;
    std::queue<QString> request_queue;

};

#endif // MUSIC_PLAYER_HPP
