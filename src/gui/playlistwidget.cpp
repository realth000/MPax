#include "playlistwidget.h"
#include "ui_playlistwidget.h"

#include <QtCore/QtDebug>

// test
#include "QStandardItemModel"

PlaylistWidget::PlaylistWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::PlaylistWidget),
    m_playlistModel(nullptr)
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
    m_playlistModel = playlistModel;
    ui->tableView->setModel(m_playlistModel);
    qDebug() << "set model in PlayListWidget with PlayContent count =" << m_playlistModel->count() << playlistModel->count();
}

PlayContent *PlaylistWidget::preContent() const {
    return m_playlistModel->findPreContent();
}

PlayContent *PlaylistWidget::nextContent() const {
    return m_playlistModel->findNextContent();
}

void PlaylistWidget::setCurrentContent(PlayContent *content) {
    if (!m_playlistModel->contains(content)) {
        qDebug() << "the current content you want to set not exists in current playlist";
        return;
    }
    m_playlistModel->setCurrentPlayContent(m_playlistModel->indexOf(content));
}

