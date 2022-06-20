#include "coreplayer.h"

#include <QtCore/QtDebug>

CorePlayer::CorePlayer(QObject *parent)
    : QObject{parent},
      m_player(new QMediaPlayer),
      m_playlist(new QMediaPlaylist),
      m_currentContent(nullptr)
{
    InitConnections();
}

void CorePlayer::InitConnections()
{
    connect(m_player, &QMediaPlayer::stateChanged, this, &CorePlayer::updatePlayState);
    connect(m_player, &QMediaPlayer::positionChanged, this, &CorePlayer::playPositionChanged);
    connect(m_player, &QMediaPlayer::metaDataAvailableChanged, this, &CorePlayer::updateMetaData);
    connect(m_player, &QMediaPlayer::durationChanged, this, &CorePlayer::playDurationChanged);
}

void CorePlayer::UpdateCurrentContent(const QUrl &contentUrl)
{
    m_currentContentUrl = contentUrl;
    m_playlist->addMedia(m_currentContentUrl);
    m_playlist->setCurrentIndex(0);
    m_player->setPlaylist(m_playlist);
}

PlayContent* CorePlayer::GetCurrentPlayContent()
{
    qDebug() << "all available data" << m_player->availableMetaData();
    PlayContent *c = new PlayContent();
    c->contentPath =  m_playlist->currentMedia().request().url().path();
    c->contentName = m_playlist->currentMedia().request().url().fileName();
    c->authorName = m_player->metaData(QStringLiteral("Author")).toString();
    c->title = m_player->metaData(QStringLiteral("Title")).toString();
    // If duration here is zero, rely on signal durationChanged().
    c->duration = m_player->metaData(QStringLiteral("Duration")).toLongLong();
    c->size = m_player->metaData(QStringLiteral("Size")).toLongLong();
    c->trackNumber = m_player->metaData(QStringLiteral("TrackNumber")).toInt();
    c->audioBitRate = m_player->metaData(QStringLiteral("AudioBitRate")).toInt();
    c->audioCodec = m_player->metaData(QStringLiteral("AudioCodec")).toString();
    c->sampleRate = m_player->metaData(QStringLiteral("SampleRate")).toInt();
    c->albumArtist = m_player->metaData(QStringLiteral("AlbumArtist")).toString();
    c->albumTitle = m_player->metaData(QStringLiteral("AlbumTitle")).toString();
    c->albumTrackCount= m_player->metaData(QStringLiteral("TrackCount")).toInt();
    c->albumCover = m_player->metaData(QStringLiteral("Author")).value<QImage>();
    c->albumYear = m_player->metaData(QStringLiteral("Year")).toInt();
    c->albumDate = m_player->metaData(QStringLiteral("Date")).toDate();
    return c;
}

void CorePlayer::updatePlayState(const QMediaPlayer::State &state)
{
    emit playStateChanged(state);
    switch (state) {
    case QMediaPlayer::PlayingState:
        updateMetaData(true);
        break;
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
    default:
        break;
    }
}

void CorePlayer::updateMetaData(bool available)
{
    if (!available) {
        qDebug() << "unavailable media content" << m_currentContentUrl.fileName();
        return;
    }
    qDebug() << "update media content";
    if (m_currentContent != nullptr) {
        delete m_currentContent;
        m_currentContent = nullptr;
    }
    m_currentContent = GetCurrentPlayContent();
    emit playContentChanged(m_currentContent);
}

QMediaPlayer::State CorePlayer::PlayState()
{
    return m_player->state();
}

void CorePlayer::play(const QUrl &contentUrl)
{
    if (contentUrl.isEmpty()) {
        qDebug() << "continue" << m_currentContentUrl.fileName();
        m_player->play();
        return;
    }
    UpdateCurrentContent(contentUrl);
//    qDebug() << "start" << m_currentContentUrl.fileName();
    m_player->play();
//    updateMetaData(true);
//    qDebug() << "aaaaa" << m_player->duration();
}

void CorePlayer::pause()
{
    qDebug() << "pause" << m_currentContentUrl.fileName();
    m_player->pause();
}

void CorePlayer::stop()
{
    qDebug() << "stop" << m_currentContentUrl.fileName();
    m_player->stop();
}

void CorePlayer::setVolMute(const bool &muted)
{
    qDebug() << "set muted" << muted;
    m_player->setMuted(muted);
}

void CorePlayer::setVol(const int &vol)
{
    qDebug() << "set volume to" << vol;
    m_player->setVolume(vol);
}

