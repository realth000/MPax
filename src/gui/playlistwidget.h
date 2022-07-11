#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

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
  explicit PlaylistWidget(
      QWidget *parent = nullptr,
      const PlaylistModelHeader *header =
          new PlaylistModelHeader(PlaylistModelHeader::defaultHeaderList()));
  ~PlaylistWidget();

 signals:
  void playContentChanged(const int &row, PlayContent *content);
  void playlistChanged();

 public slots:
  void setHeader(const PlaylistModelHeader *header);
  void setModel(PlaylistModel *playlistModel);
  PlayContentPos nextContent() const;
  PlayContentPos preContent() const;
  PlayContentPos randomContent() const;
  PlayContentPos currentPlayContent() const;
  void setCurrentContent(PlayContent *content);
  void setCurrentContent(const int &index);
  void updatePlayContent(const QModelIndex &index);
  void updateConfig();
  int count() const;

 private:
  Ui::PlaylistWidget *ui;
  const PlaylistModelHeader *m_header;
  PlaylistModel *m_playlistModel;
  PlaylistFilterModel *m_playlistFilterModel;
  QMenu *m_tableViewContextMenu;
  QModelIndexList m_tableViewSelectedRows;
  QList<qreal> m_tableViewWidthRadio;

  void InitCss(const QString &cssFilePath);
  void InitConnections();
  QMenu *InitTableViewContextMenu();
  void actionDelete();
  void actionOpenInFolder();
  void actionPlay();

 private slots:
  void openTableViewContextMenu(const QPoint &pos);
};

#endif  // PLAYLISTWIDGET_H
