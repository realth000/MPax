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
  explicit PlaylistWidget(QWidget *parent = nullptr);
  ~PlaylistWidget();

 signals:
  void playContent(PlayContent *content);

 public slots:
  void setModel(PlaylistModel *playlistModel);
  PlayContent *nextContent() const;
  PlayContent *preContent() const;
  PlayContent *randomContent() const;
  void setCurrentContent(PlayContent *content);
  void updatePlayContent(const QModelIndex &index);

 private:
  Ui::PlaylistWidget *ui;
  PlaylistModel *m_playlistModel;

  void InitCss(const QString &cssFilePath);
  void InitConnections();
};

#endif  // PLAYLISTWIDGET_H
