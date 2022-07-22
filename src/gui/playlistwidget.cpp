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
#include <QtWidgets/QScrollBar>

#include "config/appconfig.h"
#include "ui_playlistwidget.h"
#include "util/cssloader.h"

PlaylistWidget::PlaylistWidget(QWidget *parent,
                               const PlaylistModelHeader *header)
    : QWidget(parent),
      ui(new Ui::PlaylistWidget),
      m_header(header),
      m_showingModel(nullptr),
      m_showingFilterModel(new PlaylistFilterModel),
      m_playingModel(nullptr),
      m_playingFilterModel(new PlaylistFilterModel),
      m_tableViewContextMenu(InitTableViewContextMenu()),
      m_tableViewWidthRadio(QList<qreal>{0.5, 0.2, 0.3}) {
  ui->setupUi(this);
  ui->tableView->verticalHeader()->setHidden(true);
  ui->tableView->setSelectionBehavior(QTableView::SelectRows);
  ui->tableView->setColumnWidthRatio(m_tableViewWidthRadio);
  ui->tableView->setSortingEnabled(true);
  ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  this->setStyleSheet(
      util::loadCssFromFile({":/css/base.css", ":/css/playlistwidget.css"}));
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
  m_showingModel = playlistModel;
  if (m_showingModel != nullptr) {
    m_showingModel->setHeader(m_header);
  }
  //  ui->tableView->setModel(m_playlistModel);
  m_showingFilterModel->setSourceModel(m_showingModel);
  ui->tableView->setModel(m_showingFilterModel);
}

PlayContentPos PlaylistWidget::preContent() const {
  if (m_playingModel == nullptr) {
    return PlayContentPos{-1, nullptr};
  }
  const QModelIndex filterIndex =
      m_playingModel->index(m_playingModel->currentPlayContent().index, 0);
  return m_playingModel->content(
      m_playingFilterModel
          ->seekSourceRow(m_playingFilterModel->fromSourceIndex(filterIndex),
                          -1)
          .row());
}

PlayContentPos PlaylistWidget::nextContent() const {
  if (m_playingModel == nullptr) {
    return PlayContentPos{-1, nullptr};
  }
  const QModelIndex filterIndex =
      m_playingModel->index(m_playingModel->currentPlayContent().index, 0);
  return m_playingModel->content(
      m_playingFilterModel
          ->seekSourceRow(m_playingFilterModel->fromSourceIndex(filterIndex), 1)
          .row());
}

void PlaylistWidget::setShowingListCurrentContent(PlayContent *content) {
  if (m_showingModel == nullptr) {
    qDebug() << "can not set current content to a null playlist";
    return;
  }
  if (!m_showingModel->contains(content)) {
    qDebug()
        << "the current content you want to set not exists in current playlist";
    return;
  }
  m_showingModel->setCurrentPlayContent(m_showingModel->indexOf(content));
}

void PlaylistWidget::setPlayingListCurrentContent(PlayContent *content) {
  if (m_playingModel == nullptr) {
    qDebug() << "can not set current content to a null playlist";
    return;
  }
  if (!m_playingModel->contains(content)) {
    qDebug()
        << "the current content you want to set not exists in current playlist";
    return;
  }
  m_playingModel->setCurrentPlayContent(m_playingModel->indexOf(content));
}

void PlaylistWidget::setCurrentContent(const int &index) {
  if (m_showingModel == nullptr) {
    qDebug() << "can not set current content index in a null playlist";
    return;
  }
  if (m_showingModel->count() <= index) {
    qDebug() << "set current content out of index" << m_showingModel->count()
             << "to" << index;
    return;
  }
  m_showingModel->setCurrentPlayContent(index);
}

void PlaylistWidget::InitConnections() {
  connect(ui->tableView, &QTableView::doubleClicked, this,
          &PlaylistWidget::updatePlayContent);
  connect(ui->tableView, &QTableView::customContextMenuRequested, this,
          &PlaylistWidget::openTableViewContextMenu);
  // Save current playlist if needed.
  //  connect(ui->tableView->horizontalHeader(),
  //  &QHeaderView::sortIndicatorChanged,
  //          this, &PlaylistWidget::playlistOrderChanged);
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
          this, [this](int logicalIndex, Qt::SortOrder order) {
            Config::AppConfig::getInstance()->setConfig(
                CONFIG_PLAYLIST_SORT_HEADER, m_header->header(logicalIndex));
            Config::AppConfig::getInstance()->setConfig(
                CONFIG_PLAYLIST_SORT_ORDER, order);
          });
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
  if (m_showingModel == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() <= 0) {
    return;
  }
  QList<int> indexes;
  for (auto t : m_tableViewSelectedRows) {
    const int tt = m_showingFilterModel->mapToSource(t).row();
    indexes.append(tt);
  }
  m_showingModel->removeContent(indexes);
  emit playlistChanged();
}

void PlaylistWidget::actionOpenInFolder() {
  if (m_showingModel == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() <= 0) {
    return;
  }
  const QModelIndex i = m_tableViewSelectedRows[0];
  const QString path =
      m_showingModel->content(m_showingFilterModel->mapToSource(i).row())
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
  if (m_showingModel == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() < 0) {
    return;
  }
  PlayContentPos pc = m_showingModel->content(
      m_showingFilterModel->mapToSource(m_tableViewSelectedRows[0]).row());
  // Click event on current showing model, update m_playingModel.
  updatePlayingModel();
  emit playContentChanged(pc.index, pc.content);
}

void PlaylistWidget::updatePlayingModel() {
  if (m_showingModel == nullptr) {
    return;
  }
  m_playingModel = m_showingModel;
  m_playingFilterModel = m_showingFilterModel;
  emit playingListChanged(m_playingModel);
}

void PlaylistWidget::updatePlayContent(const QModelIndex &index) {
  if (m_showingModel == nullptr) {
    qDebug() << "can not update play content index in a null playlist";
    return;
  }
  const int row = m_showingFilterModel->sourceIndex(index).row();
  m_showingModel->setCurrentPlayContent(row);
  // Double-click on table,update m_playingModel.
  updatePlayingModel();
  emit playContentChanged(row, m_showingModel->currentPlayContent().content);
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
  if (m_playingModel == nullptr) {
    return PlayContentPos{-1, nullptr};
  }
  return m_playingModel->content(
      QRandomGenerator::securelySeeded().bounded(0, m_showingModel->count()));
}

PlayContentPos PlaylistWidget::currentPlayContent() const {
  if (m_playingModel == nullptr) {
    return PlayContentPos{};
  }
  return m_playingModel->currentPlayContent();
}

int PlaylistWidget::countShowing() const {
  if (m_showingModel == nullptr) {
    return 0;
  }
  return m_showingModel->count();
}

void PlaylistWidget::openTableViewContextMenu(const QPoint &pos) {
  m_tableViewSelectedRows = ui->tableView->selectionModel()->selectedRows();
  m_tableViewContextMenu->popup(QCursor().pos());
}
