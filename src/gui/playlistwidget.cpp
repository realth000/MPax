#include "playlistwidget.h"

#ifdef Q_OS_LINUX
#include <QtCore/QProcess>
#else
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#endif

#include <QtCore/QRandomGenerator>
#include <QtCore/QtDebug>
#include <QtGui/QStandardItemModel>

#include "config/appconfig.h"
#include "ui_playlistwidget.h"
#include "util/cssloader.h"

PlaylistWidget::PlaylistWidget(QWidget *parent,
                               const PlaylistModelHeader *header)
    : QWidget(parent),
      ui(new Ui::PlaylistWidget),
      m_header(header),
      m_playlistModel(nullptr),
      m_playlistFilterModel(new PlaylistFilterModel),
      m_tableViewContextMenu(InitTableViewContextMenu()) {
  ui->setupUi(this);
  ui->tableView->verticalHeader()->setHidden(true);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->setSelectionBehavior(QTableView::SelectRows);
  //  ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  //        ui->tableView->setItemDelegate(new NoFocusDelegate);
  // Set tableView row height.
  ui->tableView->verticalHeader()->setDefaultSectionSize(30);
  ui->tableView->setSortingEnabled(true);
  //  ui->tableView->sortByColumn(0, Qt::AscendingOrder);
  ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  InitCss(":/css/playlistwidget.css");
  InitConnections();
}

PlaylistWidget::~PlaylistWidget() { delete ui; }

void PlaylistWidget::setHeader(const PlaylistModelHeader *header) {
  if (m_header != nullptr) {
    delete m_header;
    m_header = nullptr;
  }
  m_header = header;
}

void PlaylistWidget::setModel(PlaylistModel *playlistModel) {
  m_playlistModel = playlistModel;
  if (m_playlistModel != nullptr) {
    m_playlistModel->setHeader(m_header);
  }
  //  ui->tableView->setModel(m_playlistModel);
  m_playlistFilterModel->setSourceModel(m_playlistModel);
  ui->tableView->setModel(m_playlistFilterModel);
}

PlayContentPos PlaylistWidget::preContent() const {
  if (m_playlistModel == nullptr) {
    return PlayContentPos{-1, nullptr};
  }
  const QModelIndex filterIndex =
      m_playlistModel->index(m_playlistModel->currentPlayContent().index, 0);
  return m_playlistModel->content(
      m_playlistFilterModel
          ->seekSourceRow(m_playlistFilterModel->fromSourceIndex(filterIndex),
                          -1)
          .row());
}

PlayContentPos PlaylistWidget::nextContent() const {
  if (m_playlistModel == nullptr) {
    return PlayContentPos{-1, nullptr};
  }
  PlayContentPos pc = m_playlistModel->currentPlayContent();

  const QModelIndex filterIndex =
      m_playlistModel->index(m_playlistModel->currentPlayContent().index, 0);
  return m_playlistModel->content(
      m_playlistFilterModel
          ->seekSourceRow(m_playlistFilterModel->fromSourceIndex(filterIndex),
                          1)
          .row());
}

void PlaylistWidget::setCurrentContent(PlayContent *content) {
  if (m_playlistModel == nullptr) {
    qDebug() << "can not set current content to a null playlist";
    return;
  }
  if (!m_playlistModel->contains(content)) {
    qDebug()
        << "the current content you want to set not exists in current playlist";
    return;
  }
  m_playlistModel->setCurrentPlayContent(m_playlistModel->indexOf(content));
}

void PlaylistWidget::setCurrentContent(const int &index) {
  if (m_playlistModel == nullptr) {
    qDebug() << "can not set current content index in a null playlist";
    return;
  }
  if (m_playlistModel->count() <= index) {
    qDebug() << "set current content out of index" << m_playlistModel->count()
             << "to" << index;
    return;
  }
  m_playlistModel->setCurrentPlayContent(index);
}

void PlaylistWidget::InitConnections() {
  connect(ui->tableView, &QTableView::doubleClicked, this,
          &PlaylistWidget::updatePlayContent);
  connect(ui->tableView, &QTableView::customContextMenuRequested, this,
          &PlaylistWidget::openTableViewContextMenu);
}

QMenu *PlaylistWidget::InitTableViewContextMenu() {
  QMenu *m = new QMenu(this);
  QAction *actionDelete = new QAction(tr("Delete"));
  connect(actionDelete, &QAction::triggered, this,
          &PlaylistWidget::actionDelete);
  QAction *actionPlay = new QAction(tr("Play"));
  connect(actionPlay, &QAction::triggered, this, &PlaylistWidget::actionPlay);
  QAction *actionOpen = new QAction(tr("Open in folder"));
  connect(actionOpen, &QAction::triggered, this,
          &PlaylistWidget::actionOpenInFolder);
  m->addAction(actionDelete);
  m->addSeparator();
  m->addAction(actionOpen);
  m->addAction(actionPlay);
  return m;
}

void PlaylistWidget::actionDelete() {
  if (m_playlistModel == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() <= 0) {
    return;
  }
  QList<int> indexes;
  for (auto t : m_tableViewSelectedRows) {
    const int tt = m_playlistFilterModel->mapToSource(t).row();
    indexes.append(tt);
  }
  m_playlistModel->removeContent(indexes);
  emit playlistChanged();
}

void PlaylistWidget::actionOpenInFolder() {
  if (m_playlistModel == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() <= 0) {
    return;
  }
  const QModelIndex i = m_tableViewSelectedRows[0];
  const QString path =
      m_playlistModel->content(m_playlistFilterModel->mapToSource(i).row())
          .content->contentPath;
#ifdef Q_OS_LINUX
  QProcess p;
  p.startDetached("nautilus", {path});
#else
  QDesktopServices::openUrl(
      QUrl("file://" + QFileInfo(path).absoluteDir().absolutePath()));
#endif
}

void PlaylistWidget::actionPlay() {
  if (m_playlistModel == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() < 0) {
    return;
  }
  PlayContentPos pc = m_playlistModel->content(
      m_playlistFilterModel->mapToSource(m_tableViewSelectedRows[0]).row());
  emit playContentChanged(pc.index, pc.content);
}

void PlaylistWidget::updatePlayContent(const QModelIndex &index) {
  if (m_playlistModel == nullptr) {
    qDebug() << "can not update play content index in a null playlist";
    return;
  }
  const int row = m_playlistFilterModel->sourceIndex(index).row();
  m_playlistModel->setCurrentPlayContent(row);
  emit playContentChanged(row, m_playlistModel->currentPlayContent().content);
}

void PlaylistWidget::updateConfig() {
  const QMap<QString, QVariant> playlistHeader =
      Config::AppConfig::getInstance()
          ->config(CONFIG_PLAYLIST_HEADER)
          .value.toMap();
  QList<PlaylistHeaderItem> list;
  QMap<QString, QVariant>::const_iterator it = playlistHeader.constBegin();
  bool noHeader = true;
  while (it != playlistHeader.constEnd()) {
    list.append(PlaylistHeaderItem(it.key(), it.value().toInt()));
    if (it.value().toInt() > 0) {
      noHeader = false;
    }
    it++;
  }
  const PlaylistModelHeader *header = new PlaylistModelHeader(
      noHeader ? PlaylistModelHeader::defaultHeaderList() : list);
  setHeader(header);
}

PlayContentPos PlaylistWidget::randomContent() const {
  if (m_playlistModel == nullptr) {
    return PlayContentPos{-1, nullptr};
  }
  return m_playlistModel->content(
      QRandomGenerator::securelySeeded().bounded(0, m_playlistModel->count()));
}

void PlaylistWidget::InitCss(const QString &cssFilePath) {
  this->setStyleSheet(util::loadCssFromFile(cssFilePath));
}

PlayContentPos PlaylistWidget::currentPlayContent() const {
  if (m_playlistModel == nullptr) {
    return PlayContentPos{};
  }
  return m_playlistModel->currentPlayContent();
}

int PlaylistWidget::count() const {
  if (m_playlistModel == nullptr) {
    return 0;
  }
  return m_playlistModel->count();
}

void PlaylistWidget::openTableViewContextMenu(const QPoint &pos) {
  m_tableViewSelectedRows = ui->tableView->selectionModel()->selectedRows();
  m_tableViewContextMenu->popup(QCursor().pos());
}
