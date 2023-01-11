#ifndef LISTTABWIDGET_H
#define LISTTABWIDGET_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets/QWidget>

#include "model/listtabmodel.h"

namespace Ui {
class ListTabWidget;
}

class ListTabWidget : public QWidget {
 Q_OBJECT

 public:
  explicit ListTabWidget(QWidget *parent = nullptr);
  ~ListTabWidget();

 signals:
  void currentPlaylistChanged(PlaylistModel *playlistModel);
  void currentPlaylistIndexChanged(const int &index);
  void reloadInfoStatusChanged(QString playlistName, bool finished, int count,
                               qint64 time);
  void currentPlayContentUpdated(PlayContentPos pos);

 public slots:
  void addPlaylist(PlaylistModel *playlistModel);
  void addPlaylist(const QList<Playlist> &playlistList);
  void addContent(PlayContent *playContent);
  void importPlaylist(const QStringList &fileList);
  void saveAllPlaylist(const QString &dirPath);
  void setCurrentPlaylist(const int &index);
  void saveDefaultPlaylist();
  PlaylistModel *CurrentPlaylist() const;
  int indexOf(PlaylistModel *playlistModel) const;
  void saveCurrentPlaylist();
  void renamePlaylist(int index, const QString &name);

 private:
  Ui::ListTabWidget *ui;
  ListTabModel *m_listTabModel;
  QMenu *m_listViewContextMenu;

  void InitConnections();
  QMenu *InitListViewContextMenu();

 private slots:
  void updateCurrentPlaylist(const QModelIndex &index);
  void openListViewContextMenu();
  void removePlaylist();
  void openRenameDialog();
};

class RenameWidget : public QDialog {
 Q_OBJECT

 public:
  explicit RenameWidget(const QString &currentName, QDialog *parent = nullptr);

 signals:
  void renamed(const QString &name);
};

#endif  // LISTTABWIDGET_H
