#ifndef LISTTABWIDGET_H
#define LISTTABWIDGET_H

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

 public slots:
  void addPlaylist(PlaylistModel *playlistModel);
  void addContent(PlayContent *playContent);
  void savePlaylist(const QString &filePath);
  void saveAllPlaylist(const QString &filePath);

 private:
  Ui::ListTabWidget *ui;
  ListTabModel *m_listTabModel;

  void InitConnections();
  void InitCss(const QString &cssFilePath);

 private slots:
  void updateCurrentPlaylist(const QModelIndex &index);
};

#endif  // LISTTABWIDGET_H
