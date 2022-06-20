#ifndef COREPLAYER_H
#define COREPLAYER_H

#include <QtCore/QDate>
#include <QtCore/QObject>
#include <QtGui/QImage>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>

struct PlayContent{
    QString contentPath;
    QString contentName;
    QString authorName;
    QString title;
    qint64 duration;
    qint64 size;
    int trackNumber;
    int audioBitRate;
    QString audioCodec;
    int sampleRate;
    QString albumArtist;
    QString albumTitle;
    QImage albumCover;
    int albumYear;
    QDate albumDate;
    int albumTrackCount;
};

class CorePlayer : public QObject
{
    Q_OBJECT
public:
    explicit CorePlayer(QObject *parent = nullptr);

    QMediaPlayer::State PlayState();
signals:
    void playContentChanged(const PlayContent *content);
    void playStateChanged(QMediaPlayer::State);
    void playPositionChanged(const qint64 &position);
    void playDurationChanged(const qint64 &duration);

public slots:
    void play(const QUrl &contentUrl = QUrl());
    void pause();
    void stop();
    void setVolMute(const bool &muted);
    void setVol(const int &vol);

private:
    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;
    QUrl m_currentContentUrl;
    PlayContent *m_currentContent;

    void InitConnections();
    void UpdateCurrentContent(const QUrl &contentUrl);
    PlayContent* GetCurrentPlayContent();

private slots:
    void updatePlayState(const QMediaPlayer::State &state);
    void updateMetaData(bool available);
};

#endif // COREPLAYER_H
