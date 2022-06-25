#ifndef PLAYCONTROLWIDGET_H
#define PLAYCONTROLWIDGET_H

#include <QtWidgets/QWidget>

#include "audio/coreplayer.h"
#include "core/playcontent.h"

namespace Ui {
class PlayControlWidget;
}

class PlayControlWidget : public QWidget {
  Q_OBJECT

 public:
  enum class PlayMode { ListRepeat = 0, SingleRepeat, Random };
  Q_ENUM(PlayMode)
  explicit PlayControlWidget(QWidget *parent = nullptr);
  ~PlayControlWidget();
  PlayMode playMode() const;
 signals:
  void playPre();
  void playNext();
  void playRandom();

 public slots:
  void setContentPath(const QString &contentPath);
  void updatePlayInfo(PlayContent *content);
  void updateConfig();

 private:
  Ui::PlayControlWidget *ui;
  CorePlayer *m_corePlayer;
  QUrl m_currentContentUrl;
  QMediaPlayer::State m_corePlayerState;

  // Volume control
  bool m_volMute;
  int m_vol;

  PlayMode m_playMode;

  void InitConfig();
  void InitCss(const QString &cssFilePath);
  void InitIconFont();
  void InitConnections();
  QString MiliSecondToString(const qint64 &ms);

 private slots:
  void updatePlay();
  void updatePlayerState(const QMediaPlayer::State &state);
  void stopPlay();
  void updateMute();
  void updateMuteWithValue(const bool &muted);
  void updateVolume(const int &vol);
  void updatePlayPosition(const qint64 &position);
  void setPlayPosition();
  void setPlayDuration(const qint64 &duration);
  void updateMuteButtonIcon();
  void updatePlayMode();
  void updatePlayMode(const int &playMode);
  void updatePlayModeButtonIcon();
  void handleMediaStatusChanged(QMediaPlayer::MediaStatus status);
};

#endif  // PLAYCONTROLWIDGET_H
