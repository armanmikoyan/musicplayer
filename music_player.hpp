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
    void initialRequestsHandler();
    void Ondisconected();

// network manipulations (helpers);
private:
    void socketConnections() const;
    void nextRequest();
    void initialRequestsQueue();

// response handlers
private:
    void responseMusicNamesUi(const QString&);

private:
// stream manipulations
    void streamConnections() const;
    void setupPlayer();
    void resetPlayer();
    void streamMusic();

// stream slots
private slots:
    void onMusicClicked(QListWidgetItem*);
    void streamRequest();

// ui manipulations
private:
    void uiConnections();

private slots:
    void onPlayBtnClicked();
    void onNextBtnClicked();
    void onPrevBtnClicked();
    void onPlay();

signals:
    void request();
    void play();




private:
    Ui::MainWindow *ui{};
    QMediaPlayer *media_player{};
    QAudioOutput *audioOutput{};
    QStringList music_files{};
    static inline int current_music_index{};
    QTcpSocket *socket{};
    QString current_music_name{};
    QBuffer *buffer{};
    std::queue<QString> request_queue{};

};

#endif // MUSIC_PLAYER_HPP
