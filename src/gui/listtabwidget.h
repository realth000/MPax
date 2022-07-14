#ifndef LISTTABWIDGET_H
#define LISTTABWIDGET_H

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

 public slots:
  void addPlaylist(PlaylistModel *playlistModel);
  void addPlaylist(const QList<Playlist> &playlistList);
  void addContent(PlayContent *playContent);
  void saveAllPlaylist(const QString &filePath);
  void setCurrentPlaylist(const int &index);
  void saveDefaultPlaylist();
  PlaylistModel *CurrentPlaylist() const;
  int indexOf(PlaylistModel *playlistModel) const;

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
};

#endif  // LISTTABWIDGET_H
