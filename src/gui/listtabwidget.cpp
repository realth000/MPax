#include "listtabwidget.h"

#include <QtCore/QDir>

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
  ui->listView->setFocusPolicy(Qt::NoFocus);
  ui->listView->setDragEnabled(true);
  ui->listView->setAcceptDrops(true);
  ui->listView->setDragDropMode(QAbstractItemView::DragDrop);
  ui->listView->setDragDropOverwriteMode(false);
  ui->listView->setDropIndicatorShown(true);
  InitConnections();
  this->setStyleSheet(
      Util::loadCssFromFile({":/css/base.css", ":/css/listtabwidget.css"}));
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
  connect(m_listTabModel, &ListTabModel::currentPlayContentUpdated, this,
          &ListTabWidget::currentPlayContentUpdated);
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

void ListTabWidget::importPlaylist(const QStringList &fileList) {
  qDebug() << "!!!!" << fileList;
  for (auto &f : fileList) {
    QFile ff(f);
    if (!ff.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug() << "can not open playlist file " << ff.fileName();
      continue;
    }
    const QStringList audioList =
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QString(ff.readAll()).split("\n", Qt::SkipEmptyParts);
#else
        QString(ff.readAll()).split("\n", QString::SkipEmptyParts);
#endif
    ff.close();
    const QString playlistName = QFileInfo(f).baseName();
    PlaylistModel *playlistModel = new PlaylistModel(playlistName);
    for (auto &a : audioList) {
      if (!QFileInfo::exists(a)) {
        continue;
      }
      playlistModel->addContent(new PlayContent(a));
    }
    m_listTabModel->addPlaylist(playlistModel);
  }
  m_listTabModel->saveAllPlaylist();
}

void ListTabWidget::saveAllPlaylist(const QString &dirPath) {
  m_listTabModel->saveAllPlaylist(dirPath);
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
