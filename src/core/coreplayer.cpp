#include "coreplayer.h"

#include <QtCore/QtDebug>

CorePlayer::CorePlayer(QObject *parent)
    : QObject{parent},
      m_player(new QMediaPlayer),
      m_playlist(new QMediaPlaylist),
      m_currentContent(nullptr)
{
    InitConnections();
    m_player->setPlaylist(m_playlist);
}

void CorePlayer::InitConnections()
{
    connect(m_player, &QMediaPlayer::stateChanged, this, &CorePlayer::updatePlayState);
    connect(m_player, &QMediaPlayer::positionChanged, this, &CorePlayer::playPositionChanged);
}

void CorePlayer::UpdateCurrentContent(const QUrl &contentUrl)
{
    m_currentContentUrl = contentUrl;
    m_playlist->clear();
    m_playlist->addMedia(QMediaContent(m_currentContentUrl));
}

void CorePlayer::updatePlayState(const QMediaPlayer::State &state)
{
    emit playStateChanged(state);
    switch (state) {
    case QMediaPlayer::PlayingState:
        break;
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
    default:
        break;
    }
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
    qDebug() << "play" << contentUrl;
    UpdateCurrentContent(contentUrl);
    m_player->play();
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

void CorePlayer::setPlayPosition(const qint64 &position)
{
    m_player->setPosition(position);
}
