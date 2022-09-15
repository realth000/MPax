#include "playlistwidget.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QtCore/QRandomGenerator>
#endif
#include <QtCore/QUrl>
#include <QtCore/QtDebug>
#include <QtGui/QClipboard>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QScrollBar>

#include "audioinfodialog.h"
#include "config/appconfig.h"
#include "model/playlistmodelheader.h"
#include "ui_playlistwidget.h"
#include "util/cssloader.h"
#include "util/fileutil.h"

PlaylistWidget::PlaylistWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PlaylistWidget),
      m_header(PLModel::PlaylistModelHeader::getInstance()),
      m_showingModel(nullptr),
      m_showingFilterModel(new PlaylistFilterModel),
      m_playingModel(nullptr),
      m_playingFilterModel(new PlaylistFilterModel),
      m_tableViewContextMenu(InitTableViewContextMenu()),
      m_tableHeaderContextMenu(initTableHeaderContextMenu()),
      m_tableViewWidthRadio(QList<qreal>{0.5, 0.2, 0.3}) {
  ui->setupUi(this);
  ui->tableView->verticalHeader()->setHidden(true);
  ui->tableView->setSelectionBehavior(QTableView::SelectRows);
  //  ui->tableView->setColumnWidthRatio(m_tableViewWidthRadio);
  ui->tableView->setSortingEnabled(true);
  ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->tableView->setFocusPolicy(Qt::NoFocus);
  ui->tableView->setAlternatingRowColors(true);
  ui->tableView->setDragEnabled(true);
  ui->tableView->setAcceptDrops(true);
  //  ui->tableView->setDragDropMode(QAbstractItemView::DragDrop);
  ui->tableView->setDragDropMode(QAbstractItemView::InternalMove);
  ui->tableView->setDragDropOverwriteMode(false);
  ui->tableView->setDropIndicatorShown(true);
  // Sort indicators only shown for a short when sort changed by user,
  // indicating the order and the sort has applied.
  // Turn off to default.
  ui->tableView->horizontalHeader()->setSortIndicatorShown(false);
  ui->tableView->horizontalHeader()->setSectionsMovable(true);
  ui->tableView->horizontalHeader()->setContextMenuPolicy(
      Qt::CustomContextMenu);
  this->setStyleSheet(
      Util::loadCssFromFile({":/css/base.css", ":/css/playlistwidget.css"}));
  InitConnections();
  // When this timer timeout, hide sort indicator. The sort indicator only used
  // to indicate the just changed sort for a short time.
  m_indicatorVisibleTimer.setSingleShot(true);

  auto headerVector = m_header->headerVector();
  for (auto &h : headerVector) {
    if (0 <= h.index && h.index < m_header->usedHeaderCount()) {
      ui->tableView->setColumnWidth(h.index, h.width);
    }
  }
}

PlaylistWidget::~PlaylistWidget() { delete ui; }

void PlaylistWidget::setModel(PlaylistModel *playlistModel) {
  m_showingModel = playlistModel;
  //  ui->tableView->setModel(m_playlistModel);
  m_showingFilterModel->setSourceModel(m_showingModel);
  ui->tableView->setModel(m_showingFilterModel);
  auto headerVector = m_header->headerVector();
  for (auto &h : headerVector) {
    if (0 <= h.index && h.index < m_header->usedHeaderCount()) {
      ui->tableView->setColumnWidth(h.index, h.width);
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
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionMoved,
          PLModel::PlaylistModelHeader::getInstance(),
          &PLModel::PlaylistModelHeader::updateSort);
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionResized,
          m_header, &PLModel::PlaylistModelHeader::updateWidth);
  connect(ui->tableView->horizontalHeader(),
          &QHeaderView::customContextMenuRequested, this,
          &PlaylistWidget::openTableHeaderContextMenu);
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
          m_showingFilterModel, &PlaylistFilterModel::reloadPlaylistByOrder);
  connect(&m_indicatorVisibleTimer, &QTimer::timeout, this, [this]() {
    ui->tableView->horizontalHeader()->setSortIndicatorShown(false);
  });
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
          this, [this]() {
            ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
            m_indicatorVisibleTimer.start(5000);
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
  QAction *actionCopyToClipBoard = new QAction(tr("Copy to clipboard"));
  connect(actionCopyToClipBoard, &QAction::triggered, this,
          &PlaylistWidget::actionCopyToClipBoard);
  m->addAction(actionDelete);
  m->addSeparator();
  m->addAction(actionOpen);
  m->addAction(actionPlay);
  m->addAction(actionCopyToClipBoard);
  m->addSeparator();
  m->addAction(actionProperty);
  return m;
}

QMenu *PlaylistWidget::initTableHeaderContextMenu() {
  QMenu *m = new QMenu(this);
  m->setMinimumWidth(150);
  QMenu *menuSetColumn = initSetTableColumnContextMenu();
  m->addMenu(menuSetColumn);
  return m;
}

QMenu *PlaylistWidget::initSetTableColumnContextMenu() {
  QMenu *m = new QMenu(this);
  m->setTitle(tr("Set columns"));
  auto s = MODEL_ALL_HEADER;
  for (auto i = s.constBegin(); i != s.constEnd(); i++) {
    QAction *a = new QAction(tr(i.value().toStdString().c_str()));
    a->setCheckable(true);
    connect(a, &QAction::triggered, this, &PlaylistWidget::updateColumns);
    auto header = m_header->usedHeader(i.value());
    if (header.name != "" && header.used) {
      a->setChecked(true);
    }
    m->addAction(a);
  }
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
  Util::openFileInDir(path);
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

void PlaylistWidget::actionCopyToClipBoard() {
  if (m_showingModel == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() < 0) {
    return;
  }
  QList<QUrl> urlList;
  for (auto &r : m_tableViewSelectedRows) {
    PlayContentPos pc =
        m_showingModel->content(m_showingFilterModel->mapToSource(r).row());
    if (pc.index < 0 || pc.content == nullptr) {
      continue;
    }
    QUrl u;
    u.setScheme("file");
    u.setPath(pc.content->contentPath);
    urlList.append(u);
  }
  QClipboard *c = QApplication::clipboard();
  QMimeData *d = new QMimeData();
  d->setUrls(urlList);
  c->setMimeData(d);
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
  if (m_tableViewSelectedRows.isEmpty()) {
    return;
  }
  m_tableViewContextMenu->popup(QCursor::pos());
}

void PlaylistWidget::openTableHeaderContextMenu(const QPoint &pos) {
  m_tableHeaderContextMenu->popup(QCursor::pos());
}

void PlaylistWidget::updateColumns(bool checked) {
  auto *action = dynamic_cast<QAction *>(sender());
  if (action == nullptr) {
    return;
  }
  // Do NOT allow to remove the last table column
  if (m_header->usedHeaderCount() <= 1 && !checked) {
    action->setChecked(true);
    return;
  }
  qDebug() << action->text() << "checked =" << checked;
  // Do NOT use PlaylistModelHeader::setUsedHeader to change used header here
  // directly Use PlaylistModel::setUsedHeader, which has beginResetModel() and
  // endResetModel(), to ensure a refresh on view after changed used headers.
  m_showingModel->setUsedHeader(action->text(), checked);
}

void PlaylistWidget::removeContents(const QList<int> &indexes) {
  m_showingModel->removeContent(indexes);
}

void PlaylistWidget::openFileInDir(const int &row) {
  Util::openFileInDir(m_showingModel->content(row).content->contentPath);
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
  connect(dialog, &AudioInfoDialog::updatePlayContentRequested, this,
          [this](PlayContent *playContent) {
            this->m_showingModel->updatePlayContent(playContent);
            if (m_playingModel != nullptr &&
                m_playingModel->currentPlayContent().index >= 0 &&
                m_playingModel->currentPlayContent().content != nullptr &&
                playContent->contentPath ==
                    m_playingModel->currentPlayContent().content->contentPath) {
              emit this->playContentInfoChanged(playContent);
            }
          });
  dialog->exec();
}
