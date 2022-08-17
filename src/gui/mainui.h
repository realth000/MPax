#ifndef MAINUI_H
#define MAINUI_H

#include <QtGui/QKeyEvent>
#include <QtWidgets/QLabel>
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
  void scrollToContent(const QString &contentPath);

 protected:
  void keyPressEvent(QKeyEvent *event) override;

 private:
  Ui::MainUI *ui;
  QList<Ui::PlayContentPair> *m_history;
  int m_historyPos;
  PlaylistSearchDialog *m_searchDialog;
  QLabel *m_statusLabel;

  PlayContent *addAudioFile(const QString &filePath);
  void addAudioFileList(const QStringList &filePathList,
                        int *counter = nullptr);
  void appendHistory(const PlayContentPos &cp);
  void prependHistory(const PlayContentPos &cp);
  void InitStatusBar();

 private slots:
  void openAudio();
  void scanAudioDir();
  void addPlaylist();
  void playPre();
  void playNext();
  void playAudioInShowingList(const int &index, PlayContent *content);
  void playAudioInPlayingList(const int &index, PlayContent *content);
  void saveAllPlaylist();
  void saveConfig();
  void savePlayingListIndex(PlaylistModel *playlistModel);
  void removeLastHistory();
  void handleDoubleClickPlay(const int &index, PlayContent *content);
  void openSearchWindow();
  void showAboutInfo();
  void showAboutQtInfo();
  void updateReloadInfoStatus(const QString &playlistName, bool finished,
                              int count, qint64 time);
  void handleCurrentPlayContentChanged(const QUrl &contentUrl);
};
#endif  // MAINUI_H
