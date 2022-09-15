#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QtCore/QTimer>
#include <QtWidgets/QMenu>
#include <QtWidgets/QWidget>

#include "model/playlistfiltermodel.h"
#include "model/playlistmodel.h"

namespace Ui {
class PlaylistWidget;
}

class PlaylistWidget : public QWidget {
  Q_OBJECT

 public:
  explicit PlaylistWidget(QWidget *parent = nullptr);
  ~PlaylistWidget();

 signals:
  void playContentChanged(const int &row, PlayContent *content);
  void playContentInfoChanged(PlayContent *content);
  void playlistChanged();
  void playingListChanged(PlaylistModel *playlistModel);
  void playlistOrderChanged();

 public slots:
  void setModel(PlaylistModel *playlistModel);
  PlayContentPos nextContent() const;
  PlayContentPos preContent() const;
  PlayContentPos randomContent() const;
  PlayContentPos currentPlayContent() const;
  void setShowingListCurrentContent(PlayContent *content);
  void setPlayingListCurrentContent(PlayContent *content);
  void setCurrentContent(const int &index);
  void setCurrentContent(const QString &contentPath);
  void updatePlayContent(const QModelIndex &index);
  void updateConfig();
  int countShowing() const;
  void updatePlayingModel();
  void removeContents(const QList<int> &indexes);
  void openFileInDir(const int &row);
  void scrollToContent(const QString &contentPath);

 private:
  Ui::PlaylistWidget *ui;
  PLModel::PlaylistModelHeader *m_header;
  PlaylistModel *m_showingModel;
  PlaylistFilterModel *m_showingFilterModel;
  PlaylistModel *m_playingModel;
  PlaylistFilterModel *m_playingFilterModel;
  QMenu *m_tableViewContextMenu;
  QMenu *m_tableHeaderContextMenu;
  QModelIndexList m_tableViewSelectedRows;
  QList<qreal> m_tableViewWidthRadio;
  QTimer m_indicatorVisibleTimer;

  void InitConnections();
  QMenu *InitTableViewContextMenu();
  QMenu *initTableHeaderContextMenu();
  QMenu *initSetTableColumnContextMenu();
  void actionDelete();
  void actionOpenInFolder();
  void actionPlay();
  void actionCopyToClipBoard();
  void actionShowPropertyDialog();

 private slots:
  void openTableViewContextMenu(const QPoint &pos);
  void openTableHeaderContextMenu(const QPoint &pos);
  void updateColumns(bool checked);
};

#endif  // PLAYLISTWIDGET_H
