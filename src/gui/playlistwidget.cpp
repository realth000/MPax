#include "playlistwidget.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QtCore/QRandomGenerator>
#endif
#include <QtCore/QtDebug>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QScrollBar>

#include "audioinfodialog.h"
#include "config/appconfig.h"
#include "model/playlistmodelheader.h"
#include "ui_playlistwidget.h"
#include "util/cssloader.h"
#include "util/fileutil.h"

PlaylistWidget::PlaylistWidget(QWidget *parent,
                               const PLModel::PlaylistModelHeader *header)
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
  ui->tableView->setFocusPolicy(Qt::NoFocus);
  this->setStyleSheet(
      util::loadCssFromFile({":/css/base.css", ":/css/playlistwidget.css"}));
  InitConnections();
}

PlaylistWidget::~PlaylistWidget() { delete ui; }

void PlaylistWidget::setHeader(const PLModel::PlaylistModelHeader *header) {
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
  const QString sortHeader = Config::AppConfig::getInstance()
                                 ->config(CONFIG_PLAYLIST_SORT_HEADER)
                                 .value.toString();
  const Qt::SortOrder sortOrder =
      static_cast<Qt::SortOrder>(Config::AppConfig::getInstance()
                                     ->config(CONFIG_PLAYLIST_SORT_ORDER)
                                     .value.toInt());
  for (int i = 0; i < m_header->headerCount(); i++) {
    if (m_header->usedHeader(i) == sortHeader) {
      ui->tableView->horizontalHeader()->setSortIndicator(i, sortOrder);
    }
  }
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

void PlaylistWidget::setCurrentContent(const QString &playContent) {
  if (m_showingModel == nullptr) {
    qDebug() << "can not set current content index in a null playlist";
    return;
  }
  if (!m_showingModel->list().contains(playContent)) {
    qDebug() << "set current content not in current playlist " << playContent;
    return;
  }
  PlayContentPos pc = m_showingModel->content(playContent);
  if (pc.index < 0 || pc.content == nullptr) {
    qDebug() << "can not set empty content path to current content";
    return;
  }
  m_showingModel->setCurrentPlayContent(pc.index);
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
  QAction *actionProperty = new QAction(tr("Property"));
  connect(actionProperty, &QAction::triggered, this,
          &PlaylistWidget::actionShowPropertyDialog);
  m->addAction(actionDelete);
  m->addSeparator();
  m->addAction(actionOpen);
  m->addAction(actionPlay);
  m->addSeparator();
  m->addAction(actionProperty);
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
  util::openFileInDir(path);
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
  if (m_showingModel == nullptr || m_playingModel == m_showingModel) {
    return;
  }
  m_playingModel = m_showingModel;
  m_playingFilterModel->setSourceModel(m_playingModel);
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
  const PLModel::PlaylistModelHeader *header = new PLModel::PlaylistModelHeader(
      noHeader ? PLModel::PlaylistModelHeader::defaultHeaderList() : list);
  setHeader(header);
}

PlayContentPos PlaylistWidget::randomContent() const {
  if (m_playingModel == nullptr) {
    return PlayContentPos{-1, nullptr};
  }
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  return m_playingModel->content(
      QRandomGenerator::securelySeeded().bounded(0, m_playingModel->count()));
#else
  const QTime t = QTime::currentTime();
  qsrand(t.msec() + t.second() * 1000);
  return m_playingModel->content(qrand() % m_playingModel->count());
#endif
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

void PlaylistWidget::removeContents(const QList<int> &indexes) {
  m_showingModel->removeContent(indexes);
}

void PlaylistWidget::openFileInDir(const int &row) {
  util::openFileInDir(m_showingModel->content(row).content->contentPath);
}

void PlaylistWidget::scrollToContent(const QString &contentPath) {
  if (contentPath.isEmpty()) {
    return;
  }

  // If selected rows is more than one, do not scroll, otherwise may get a bad
  // experience.
  if (ui->tableView->selectionModel()->selectedRows().count() > 1) {
    return;
  }
  // Only scroll to current play contentPath when showing the current
  // playlist.
  if (m_showingModel != m_playingModel) {
    return;
  }
  const QModelIndex showIndex =
      m_showingFilterModel->mapFromSource(m_showingModel->find(contentPath));
  if (!showIndex.isValid()) {
    return;
  }
  ui->tableView->scrollTo(showIndex, QAbstractItemView::PositionAtCenter);
  ui->tableView->clearSelection();
  ui->tableView->selectRow(showIndex.row());
}

void PlaylistWidget::actionShowPropertyDialog() {
  if (m_tableViewSelectedRows.count() <= 0) {
    return;
  }
  const QModelIndex i = m_tableViewSelectedRows[0];
  PlayContentPos c =
      m_showingModel->content(m_showingFilterModel->mapToSource(i).row());
  if (c.index < 0 || c.content == nullptr) {
    return;
  }
  AudioInfoDialog *dialog = new AudioInfoDialog(c.content, this);
  dialog->exec();
}
