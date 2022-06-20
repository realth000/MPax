#include "coreplayer.h"

CorePlayer::CorePlayer(QObject *parent)
    : QObject{parent},
      m_player(new QMediaPlayer),
      m_playlist(new QMediaPlaylist)
{

}

void CorePlayer::Play(const QUrl &mediaContentUrl)
{
    m_playlist->addMedia(mediaContentUrl);
    m_playlist->setCurrentIndex(0);
    m_player->setPlaylist(m_playlist);
    m_player->play();
}
