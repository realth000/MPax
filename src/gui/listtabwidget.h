#ifndef LISTTABWIDGET_H
#define LISTTABWIDGET_H

#include <QtWidgets/QWidget>

#include "core/listtabmodel.h"

namespace Ui {
class ListTabWidget;
}

class ListTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ListTabWidget(QWidget *parent = nullptr);
    ~ListTabWidget();

signals:
    void currentPlaylistChanged(PlaylistModel *playlistModel);

public slots:
    void addPlaylist(PlaylistModel *playlistModel);
    void addContent(PlayContent *playContent);

private:
    Ui::ListTabWidget *ui;
    ListTabModel *m_listTabModel;

    void InitConnections();

private slots:
    void updateCurrentPlaylist(const QModelIndex &index);
};

#endif // LISTTABWIDGET_H
