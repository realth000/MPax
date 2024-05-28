#include "coreplayer.h"

#include <QtCore/QtDebug>

CorePlayer::CorePlayer(QObject *parent) : QObject{parent}, m_player(new QMediaPlayer), m_playlist(new QMediaPlaylist) {
  InitConnections();
  m_player->setPlaylist(m_playlist);
}

CorePlayer::~CorePlayer() {
  delete m_player;
  delete m_playlist;
}

void CorePlayer::InitConnections() {
  connect(m_player, &QMediaPlayer::stateChanged, this, &CorePlayer::updatePlayState);
  connect(m_player, &QMediaPlayer::positionChanged, this, &CorePlayer::playPositionChanged);
  connect(m_player, &QMediaPlayer::durationChanged, this, &CorePlayer::playDurationChanged);
  connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &CorePlayer::playMediaStatusChanged);
}

void CorePlayer::UpdateCurrentContent(const QUrl &contentUrl) {
  m_currentContentUrl = contentUrl;
  m_playlist->clear();
  m_playlist->addMedia(m_currentContentUrl);
}

void CorePlayer::updatePlayState(const QMediaPlayer::State &state) {
  emit playStateChanged(state);
  switch (state) {
    case QMediaPlayer::PlayingState:
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
    default:
      break;
  }
}

QMediaPlayer::State CorePlayer::playState() const { return m_player->state(); }

QMediaPlayer::MediaStatus CorePlayer::mediaStatus() const { return m_player->mediaStatus(); }

void CorePlayer::play(const QUrl &contentUrl) {
  if (contentUrl.isEmpty()) {
    m_player->play();
    return;
  }
  UpdateCurrentContent(contentUrl);
  m_player->play();
}

void CorePlayer::pause() { m_player->pause(); }

void CorePlayer::stop() { m_player->stop(); }

void CorePlayer::setVolMute(const bool &muted) { m_player->setMuted(muted); }

void CorePlayer::setVol(const int &vol) { m_player->setVolume(vol); }

void CorePlayer::setPlayPosition(const qint64 &position) { m_player->setPosition(position); }
