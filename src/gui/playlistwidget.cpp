#include "playlistwidget.h"

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
      m_playlistModel(nullptr) {
  ui->setupUi(this);
  ui->tableView->verticalHeader()->setHidden(true);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->setSelectionBehavior(QTableView::SelectRows);
  ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  //        ui->tableView->setItemDelegate(new NoFocusDelegate);
  // Set tableView row height.
  ui->tableView->verticalHeader()->setDefaultSectionSize(30);
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
  m_playlistModel->setHeader(m_header);
  ui->tableView->setModel(m_playlistModel);
}

PlayContent *PlaylistWidget::preContent() const {
  if (m_playlistModel == nullptr) {
    return nullptr;
  }
  return m_playlistModel->findPreContent();
}

PlayContent *PlaylistWidget::nextContent() const {
  if (m_playlistModel == nullptr) {
    return nullptr;
  }
  return m_playlistModel->findNextContent();
}

void PlaylistWidget::setCurrentContent(PlayContent *content) {
  if (!m_playlistModel->contains(content)) {
    qDebug()
        << "the current content you want to set not exists in current playlist";
    return;
  }
  m_playlistModel->setCurrentPlayContent(m_playlistModel->indexOf(content));
}

void PlaylistWidget::setCurrentContent(const int &index) {
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
}

void PlaylistWidget::updatePlayContent(const QModelIndex &index) {
  const int row = index.row();
  m_playlistModel->setCurrentPlayContent(row);
  emit playContent(m_playlistModel->currentPlayContent());
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

PlayContent *PlaylistWidget::randomContent() const {
  return m_playlistModel->content(
      QRandomGenerator::securelySeeded().bounded(0, m_playlistModel->count()));
}

void PlaylistWidget::InitCss(const QString &cssFilePath) {
  this->setStyleSheet(util::loadCssFromFile(cssFilePath));
}

const PlayContent *PlaylistWidget::currentPlayContent() const {
  return m_playlistModel->currentPlayContent();
}
