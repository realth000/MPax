#include "playlistwidget.h"
#include "ui_playlistwidget.h"

#include <QtCore/QtDebug>

PlaylistWidget::PlaylistWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistWidget)
{
    ui->setupUi(this);
}

PlaylistWidget::~PlaylistWidget()
{
    delete ui;
}

void PlaylistWidget::setModel(PlaylistModel *playlistModel)
{
    if (playlistModel == nullptr) {
        qDebug() << "can not bind nullptr model with view in playlist";
        return;
    }
    ui->tableView->setModel(playlistModel);
}
