#include "playlistwidget.h"
#include "ui_playlistwidget.h"

#include <QtCore/QtDebug>

#include <QtCore/QRandomGenerator>

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
    InitConnections();
}

PlaylistWidget::~PlaylistWidget()
{
    delete ui;
}

void PlaylistWidget::setModel(PlaylistModel *playlistModel)
{
    m_playlistModel = playlistModel;
    ui->tableView->setModel(m_playlistModel);
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

void PlaylistWidget::InitConnections() {
    connect(ui->tableView, &QTableView::doubleClicked, this, &PlaylistWidget::updatePlayContent);
}

void PlaylistWidget::updatePlayContent(const QModelIndex &index) {
    const int row = index.row();
    m_playlistModel->setCurrentPlayContent(row);
    emit playContent(m_playlistModel->currentPlayContent());
}

PlayContent *PlaylistWidget::randomContent() const {
    return m_playlistModel->content(QRandomGenerator::securelySeeded().bounded(0, m_playlistModel->count()));
}

