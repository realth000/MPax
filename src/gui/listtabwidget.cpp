#include "listtabwidget.h"
#include "ui_listtabwidget.h"

ListTabWidget::ListTabWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::ListTabWidget),
    m_ListTabModel(new ListTabModel)
{
    ui->setupUi(this);
    ui->listView->setModel(m_ListTabModel);
    InitConnections();
}

ListTabWidget::~ListTabWidget()
{
    delete ui;
}

void ListTabWidget::addPlaylist(PlaylistModel *playlistModel) {
    m_ListTabModel->addPlaylist(playlistModel);
}

void ListTabWidget::addContent(PlayContent *playContent) {
    m_ListTabModel->addContent(playContent);
}

void ListTabWidget::InitConnections() {
    connect(m_ListTabModel, &ListTabModel::currentPlaylistChanged, this, &ListTabWidget::currentPlaylistChanged);
}
