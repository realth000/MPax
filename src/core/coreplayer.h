#ifndef COREPLAYER_H
#define COREPLAYER_H

#include <QtCore/QObject>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>

class CorePlayer : public QObject
{
    Q_OBJECT
public:
    explicit CorePlayer(QObject *parent = nullptr);

    QMediaPlayer::State PlayState();
signals:
    void playStateChanged(QMediaPlayer::State);

public slots:
    void play(const QUrl &contentUrl = QUrl());
    void pause();
    void stop();
    void setVolMute(const bool &muted);
    void setVol(const int &vol);

private:
    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;
    QUrl m_currentContent;

    void InitConnections();
};

#endif // COREPLAYER_H
