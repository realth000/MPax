﻿#ifndef MAINUI_H
#define MAINUI_H

#include <QtGui/QKeyEvent>
#include <QtWidgets/QMainWindow>

#include "audio/coreplayer.h"
#include "gui/playlistsearchdialog.h"
#include "model/listtabmodel.h"
#include "model/playlistmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainUI;
typedef QPair<int, PlayContent *> PlayContentPair;
}  // namespace Ui
QT_END_NAMESPACE
class MainUI : public QMainWindow {
  Q_OBJECT

 public:
  MainUI(QWidget *parent = nullptr);
  ~MainUI();
  void InitConnections();

 signals:
  void updateConfig();

 protected:
  void keyPressEvent(QKeyEvent *event) override;

 private:
  Ui::MainUI *ui;
  QList<Ui::PlayContentPair> *m_history;
  int m_historyPos;
  PlaylistSearchDialog *m_searchDialog;

  PlayContent *addAudioFile(const QString &filePath);
  void addHistory(const PlayContentPos &cp);

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
  void removeLastHistory();
  void handleDoubleClickPlay(const int &index, PlayContent *content);
  void openSearchWindow();
};
#endif  // MAINUI_H
