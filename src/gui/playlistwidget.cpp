#include "playlistwidget.h"
#include "ui_playlistwidget.h"

#include <QtCore/QtDebug>

PlaylistWidget::PlaylistWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistWidget)
{
    ui->setupUi(this);
    ui->tableView->verticalHeader()->setHidden(true);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
//        ui->tableView->setItemDelegate(new NoFocusDelegate);
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
