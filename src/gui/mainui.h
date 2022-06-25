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
  void playAudio(PlayContent *content);
  void savePlaylist();
  void saveAllPlaylist();
};
#endif  // MAINUI_H
