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


class music_player : public QObject
{
    Q_OBJECT

public:
    explicit music_player(Ui::MainWindow*, QObject *parent = nullptr);

private slots:
    void play_btn_clicked(bool = true);

    void next_btn_clicked();

    void prev_btn_clicked();

    void vertical_slider_triggered(int);

    void music_clicked(QListWidgetItem* item);
    void get_music();
    void stream_music();

private:
    void connections() const;
    void setup_music_playlist();
    void ui_render();
    void get_music_names();
    void draw_music_names();
    void change_current_music(const QString &);
    void reset_buffer_and_media_player();


private:
    Ui::MainWindow* ui;

    QMediaPlayer *media_player;
    QAudioOutput* audioOutput;
    QStringList music_files;
    static inline int current_music_index = 0;
    bool play_button_state;
    QTcpSocket* socket;
    QString current_music_name;
    QBuffer* buffer = nullptr;

private:
    QIcon playIcon;
    QIcon stopIcon;
    QIcon nextIcon;
    QIcon prevtIcon;
};

#endif // MUSIC_PLAYER_HPP
