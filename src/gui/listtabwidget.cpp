#include "listtabwidget.h"

#include "ui_listtabwidget.h"
#include "util/cssloader.h"

ListTabWidget::ListTabWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ListTabWidget),
      m_listTabModel(new ListTabModel),
      m_listViewContextMenu(InitListViewContextMenu()) {
  ui->setupUi(this);
  ui->listView->setModel(m_listTabModel);
  ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
  InitConnections();
  this->setStyleSheet(
      util::loadCssFromFile({":/css/base.css", ":/css/listtabwidget.css"}));
}

ListTabWidget::~ListTabWidget() { delete ui; }

void ListTabWidget::addPlaylist(PlaylistModel *playlistModel) {
  m_listTabModel->addPlaylist(playlistModel);
}

void ListTabWidget::addContent(PlayContent *playContent) {
  m_listTabModel->addContent(playContent);
}

void ListTabWidget::InitConnections() {
  connect(m_listTabModel, &ListTabModel::currentPlaylistChanged, this,
          &ListTabWidget::currentPlaylistChanged);
  connect(ui->listView, &QListView::clicked, this,
          &ListTabWidget::updateCurrentPlaylist);
  connect(ui->listView, &QListView::customContextMenuRequested, this,
          &ListTabWidget::openListViewContextMenu);
  connect(m_listTabModel, &ListTabModel::reloadInfoStatusChanged, this,
          &ListTabWidget::reloadInfoStatusChanged);
}

void ListTabWidget::updateCurrentPlaylist(const QModelIndex &index) {
  m_listTabModel->setCurrentPlaylist(index.row());
  emit currentPlaylistIndexChanged(index.row());
}

void ListTabWidget::openListViewContextMenu() {
  m_listViewContextMenu->popup(QCursor::pos());
}

QMenu *ListTabWidget::InitListViewContextMenu() {
  QMenu *m = new QMenu(this);
  QAction *actionDelete = new QAction(tr("Delete"));
  connect(actionDelete, &QAction::triggered, this,
          &ListTabWidget::removePlaylist);
  m->addAction(actionDelete);
  return m;
}

void ListTabWidget::removePlaylist() {
  m_listTabModel->removePlaylist(ui->listView->currentIndex().row());
  emit currentPlaylistChanged(m_listTabModel->currentPlaylist());
}

void ListTabWidget::saveAllPlaylist(const QString &filePath) {
  m_listTabModel->saveAllPlaylist(filePath);
}

void ListTabWidget::setCurrentPlaylist(const int &index) {
  m_listTabModel->setCurrentPlaylist(index);
}

PlaylistModel *ListTabWidget::CurrentPlaylist() const {
  return m_listTabModel->currentPlaylist();
}

void ListTabWidget::addPlaylist(const QList<Playlist> &playlistList) {
  for (const auto &playlist : playlistList) {
    m_listTabModel->addPlaylist(new PlaylistModel(playlist));
  }
}

void ListTabWidget::saveDefaultPlaylist() {
  m_listTabModel->saveDefaultPlaylist();
}

int ListTabWidget::indexOf(PlaylistModel *playlistModel) const {
  return m_listTabModel->indexOf(playlistModel);
}

void ListTabWidget::saveCurrentPlaylist() {
  m_listTabModel->saveCurrentPlaylist();
}
