#include "coreplayer.h"

#include <QtCore/QtDebug>

CorePlayer::CorePlayer(QObject *parent)
    : QObject{parent},
      m_player(new QMediaPlayer),
      m_playlist(new QMediaPlaylist)
{
    InitConnections();
}

void CorePlayer::InitConnections()
{
    connect(m_player, &QMediaPlayer::stateChanged, this, &CorePlayer::playStateChanged);
}

QMediaPlayer::State CorePlayer::PlayState()
{
    return m_player->state();
}

void CorePlayer::play(const QUrl &contentUrl)
{
    if (contentUrl.isEmpty()) {
        qDebug() << "continue" << m_currentContent.fileName();
        m_player->play();
        return;
    }
    m_currentContent = contentUrl;
    qDebug() << "start" << m_currentContent.fileName();
    m_playlist->addMedia(contentUrl);
    m_playlist->setCurrentIndex(0);
    m_player->setPlaylist(m_playlist);
    m_player->play();
    m_player->play();
}

void CorePlayer::pause()
{
    qDebug() << "pause" << m_currentContent.fileName();
    m_player->pause();
}

void CorePlayer::stop()
{
    qDebug() << "stop" << m_currentContent.fileName();
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

