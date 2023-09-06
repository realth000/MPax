#include "listtabwidget.h"

#include <QtCore/QDir>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

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
  m->setMinimumWidth(150);
  QAction *actionDelete = new QAction(tr("Delete"));
  connect(actionDelete, &QAction::triggered, this,
          &ListTabWidget::removePlaylist);
  QAction *actionRename = new QAction(tr("Rename"));
  connect(actionRename, &QAction::triggered, this,
          &ListTabWidget::openRenameDialog);
  m->addAction(actionRename);
  m->addSeparator();
  m->addAction(actionDelete);
  return m;
}

void ListTabWidget::removePlaylist() {
  m_listTabModel->removePlaylist(ui->listView->currentIndex().row());
  emit currentPlaylistChanged(m_listTabModel->currentPlaylist());
}

void ListTabWidget::openRenameDialog() {
  const int index = ui->listView->currentIndex().row();
  RenameWidget *renameWidget =
      new RenameWidget(m_listTabModel->index(index)->playlistName());
  connect(renameWidget, &RenameWidget::renamed, this,
          [this, index](const QString &name) { renamePlaylist(index, name); });
  renameWidget->exec();
  delete renameWidget;
  renameWidget = nullptr;
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

void ListTabWidget::renamePlaylist(int index, const QString &name) {
  m_listTabModel->renamePlaylist(index, name);
  saveDefaultPlaylist();
}

PlaylistModel *ListTabModel::index(int index) const {
  if (m_playlistList.length() <= index) {
    return nullptr;
  }
  return m_playlistList[index];
}

RenameWidget::RenameWidget(const QString &currentName, QDialog *parent)
    : QDialog(parent) {
  this->setWindowTitle(tr("Rename Playlist"));
  QLineEdit *lineEdit = new QLineEdit(this);
  lineEdit->setText(currentName);
  lineEdit->selectAll();
  QPushButton *pushButton = new QPushButton(this);
  pushButton->setText(tr("Yes"));
  QHBoxLayout *layout = new QHBoxLayout(this);
  connect(pushButton, &QPushButton::clicked, this, [this, lineEdit]() {
    if (!lineEdit->text().isEmpty()) {
      emit renamed(lineEdit->text());
    }
    this->close();
  });
  layout->addWidget(lineEdit);
  layout->addWidget(pushButton);
  this->setLayout(layout);
}
