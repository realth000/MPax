#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QtWidgets/QWidget>

#include "core/playlistmodel.h"

namespace Ui {
class PlaylistWidget;
}

class PlaylistWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistWidget(QWidget *parent = nullptr);
    ~PlaylistWidget();

public slots:
    void setModel(PlaylistModel *playlistModel);

private:
    Ui::PlaylistWidget *ui;
};

#endif // PLAYLISTWIDGET_H
