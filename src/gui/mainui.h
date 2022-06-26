#ifndef MAINUI_H
#define MAINUI_H

#include <QtWidgets/QMainWindow>

#include "audio/coreplayer.h"
#include "model/listtabmodel.h"
#include "model/playlistmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainUI;
}
QT_END_NAMESPACE
class MainUI : public QMainWindow {
  Q_OBJECT

 public:
  MainUI(QWidget *parent = nullptr);
  ~MainUI();
  void InitConnections();

 signals:
  void updateConfig();

 private:
  Ui::MainUI *ui;

  PlayContent *addAudioFile(const QString &filePath);

 private slots:
  void openAudio();
  void scanAudioDir();
  void addPlaylist();
  void playPre();
  void playNext();
  void playRandom();
  void playAudio(const int &index, PlayContent *content);
  void playAudio(const int &index);
  void savePlaylist();
  void saveAllPlaylist();
  void loadPlaylist();
  void saveConfig();
  void saveCurrentPlaylistIndex(const int &index);
};
#endif  // MAINUI_H
