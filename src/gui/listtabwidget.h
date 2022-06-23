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

public slots:
    void setModel(ListTabModel *listTabModel);
    void addPlaylist(PlaylistModel *playlistModel);

private:
    Ui::ListTabWidget *ui;
    ListTabModel *m_currentListTabModel;
};

#endif // LISTTABWIDGET_H
