#include "playlistwidget.h"
#include "ui_playlistwidget.h"

#include <QtCore/QtDebug>

// test
#include "QStandardItemModel"

PlaylistWidget::PlaylistWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::PlaylistWidget),
    m_PlaylistModel(nullptr)
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
    qDebug() << "set model in PlayListWidget with PlayContent count =" << playlistModel->count() << playlistModel->count();
    m_PlaylistModel = playlistModel;
    ui->tableView->setModel(m_PlaylistModel);
    qDebug() << "set model in PlayListWidget with PlayContent count =" << m_PlaylistModel->count() << playlistModel->count();
}

