#include "listtabwidget.h"
#include "ui_listtabwidget.h"

ListTabWidget::ListTabWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::ListTabWidget),
    m_listTabModel(new ListTabModel)
{
    ui->setupUi(this);
    ui->listView->setModel(m_listTabModel);
    InitConnections();
}

ListTabWidget::~ListTabWidget()
{
    delete ui;
}

void ListTabWidget::addPlaylist(PlaylistModel *playlistModel) {
    m_listTabModel->addPlaylist(playlistModel);
}

void ListTabWidget::addContent(PlayContent *playContent) {
    m_listTabModel->addContent(playContent);
}

void ListTabWidget::InitConnections() {
    connect(m_listTabModel, &ListTabModel::currentPlaylistChanged, this, &ListTabWidget::currentPlaylistChanged);
    connect(ui->listView, &QListView::clicked, this, &ListTabWidget::updateCurrentPlaylist);
}

void ListTabWidget::updateCurrentPlaylist(const QModelIndex &index) {
    m_listTabModel->setCurrentPlaylist(index.row());
}
