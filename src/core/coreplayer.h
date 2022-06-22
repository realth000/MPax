#ifndef COREPLAYER_H
#define COREPLAYER_H

#include <QtCore/QDate>
#include <QtCore/QObject>
#include <QtGui/QImage>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>

#include "core/playcontent.h"

class CorePlayer : public QObject
{
    Q_OBJECT
public:
    explicit CorePlayer(QObject *parent = nullptr);

    QMediaPlayer::State PlayState();
signals:
    void playStateChanged(QMediaPlayer::State);
    void playPositionChanged(const qint64 &position);

public slots:
    void play(const QUrl &contentUrl = QUrl());
    void pause();
    void stop();
    void setVolMute(const bool &muted);
    void setVol(const int &vol);
    void setPlayPosition(const qint64 &position);

private:
    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;
    QUrl m_currentContentUrl;
    PlayContent *m_currentContent;

    void InitConnections();
    void UpdateCurrentContent(const QUrl &contentUrl);

private slots:
    void updatePlayState(const QMediaPlayer::State &state);
};

#endif // COREPLAYER_H
