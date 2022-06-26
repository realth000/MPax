#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QtWidgets/QWidget>

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

  void InitCss(const QString &cssFilePath);
  void InitConnections();
};

#endif  // PLAYLISTWIDGET_H
