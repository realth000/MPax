#include "playlistsearchdialog.h"

#include <QtCore/QDebug>

#include "audioinfodialog.h"
#include "config/appconfig.h"
#include "model/playlistmodel.h"
#include "ui_playlistsearchdialog.h"
#include "util/cssloader.h"

PlaylistSearchDialog::PlaylistSearchDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::PlaylistSearchDialog),
      m_model(new Model::PlaylistSearchFilterModel),
      m_tableViewWidthRadio(QList<qreal>{0.5, 0.2, 0.3}),
      m_rowCount(-1),
      m_rowPos(-1),
      m_tableViewContextMenu(initTableViewContextMenu()),
      m_header(PLModel::PlaylistModelHeader::getInstance()) {
  ui->setupUi(this);
  this->setWindowTitle(tr("Search in playlist"));
  ui->searchTableView->verticalHeader()->setHidden(true);
  ui->searchTableView->setSelectionBehavior(QTableView::SelectRows);
  //  ui->searchTableView->setColumnWidthRatio(m_tableViewWidthRadio);
  ui->searchTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->searchTableView->setSortingEnabled(true);
  //  ui->tableView->sortByColumn(0, Qt::AscendingOrder);
  ui->searchTableView->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->searchTableView->setFocusPolicy(Qt::NoFocus);
  ui->searchTableView->setAlternatingRowColors(true);
  this->setStyleSheet(Util::loadCssFromFile({":/css/base.css", ":/css/playlistwidget.css"}));
  //  m_model->setSourceModel(model);
  ui->caseCheckBox->setFocusPolicy(Qt::NoFocus);
  ui->searchLineEdit->setFocus();
  ui->searchTableView->setModel(m_model);
  ui->searchTableView->setSortingEnabled(false);

  auto headerVector = m_header->headerVector();
  for (auto &h : headerVector) {
    if (0 <= h.index && h.index < m_header->headerCount()) {
      ui->searchTableView->setColumnWidth(h.index, h.width);
    }
  }

  connect(ui->searchLineEdit, &QLineEdit::textEdited, this, &PlaylistSearchDialog::updateSearchFilter);
  connect(ui->caseCheckBox, &QCheckBox::stateChanged, this,
          [this]() { updateSearchFilter(ui->searchLineEdit->text()); });
  connect(ui->searchTableView, &QTableView::doubleClicked, this,
          QOverload<const QModelIndex &>::of(&PlaylistSearchDialog::updatePlayContent));
  connect(m_model, &Model::PlaylistSearchFilterModel::rowCountChanged, this,
          &PlaylistSearchDialog::updateRowCountAfterFilter);
  connect(ui->searchTableView, &QTableView::pressed, this,
          [this](const QModelIndex &index) { this->m_rowPos = index.row(); });
  connect(ui->searchTableView, &QTableView::customContextMenuRequested, this,
          &PlaylistSearchDialog::openTableViewContextMenu);
}

PlaylistSearchDialog::~PlaylistSearchDialog() {}

void PlaylistSearchDialog::setModel(QAbstractItemModel *model) {
  m_model->setSourceModel(model);
  auto headerVector = m_header->headerVector();
  for (auto &h : headerVector) {
    if (0 <= h.index && h.index < m_header->headerCount()) {
      ui->searchTableView->setColumnWidth(h.index, h.width);
    }
  }
}

void PlaylistSearchDialog::keyPressEvent(QKeyEvent *event) {
  if (event->modifiers() == Qt::NoModifier) {
    switch (event->key()) {
      case Qt::Key_Up:
        selectPre();
        event->accept();
        return;
      case Qt::Key_Down:
        selectNext();
        event->accept();
        return;
      case Qt::Key_Return:
        updatePlayContent(m_rowPos);
        return;
    }
  }
  if (event->modifiers() == Qt::KeypadModifier && event->key() == Qt::Key_Enter) {
    updatePlayContent(m_rowPos);
    return;
  }
  QDialog::keyPressEvent(event);
}

Qt::CaseSensitivity PlaylistSearchDialog::caseSensitivity() {
  return ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

void PlaylistSearchDialog::selectPre() {
  if (m_rowCount <= 0) {
    return;
  }
  if (m_rowPos > 0) {
    ui->searchTableView->selectRow(--m_rowPos);
  } else {
    ui->searchTableView->selectRow(m_rowCount - 1);
    m_rowPos = m_rowCount - 1;
  }
}

void PlaylistSearchDialog::selectNext() {
  if (m_rowCount <= 0) {
    return;
  }
  if (m_rowPos < m_rowCount - 1) {
    ui->searchTableView->selectRow(++m_rowPos);
  } else {
    ui->searchTableView->selectRow(0);
    m_rowPos = 0;
  }
}

void PlaylistSearchDialog::updateRowCountAfterFilter(const int newCount) {
  m_rowCount = newCount;
  if (m_rowCount > 0) {
    m_rowPos = 0;
    ui->searchTableView->selectRow(m_rowPos);
  }
}

void PlaylistSearchDialog::updateSearchFilter(const QString &filterString) {
  m_model->setFilterExp(QRegExp(filterString, caseSensitivity()), Model::PlaylistSearchFilterModel::FilterMode::Text);
}

void PlaylistSearchDialog::updatePlayContent(const QModelIndex &index) {
  const int row = m_model->sourceIndex(index).row();
  emit playContentChanged(row);
}

void PlaylistSearchDialog::updatePlayContent(const int &row) { updatePlayContent(m_model->index(row, 0)); }

void PlaylistSearchDialog::openTableViewContextMenu(const QPoint &pos) {
  m_tableViewSelectedRows = ui->searchTableView->selectionModel()->selectedRows();
  if (m_tableViewSelectedRows.isEmpty()) {
    return;
  }
  m_tableViewContextMenu->popup(QCursor().pos());
}

QMenu *PlaylistSearchDialog::initTableViewContextMenu() {
  QMenu *m = new QMenu(this);
  QAction *actionDelete = new QAction(tr("Delete"));
  connect(actionDelete, &QAction::triggered, this, &PlaylistSearchDialog::actionDelete);
  QAction *actionPlay = new QAction(tr("Play"));
  connect(actionPlay, &QAction::triggered, this, &PlaylistSearchDialog::actionPlay);
  QAction *actionOpen = new QAction(tr("Open in folder"));
  connect(actionOpen, &QAction::triggered, this, &PlaylistSearchDialog::actionOpenInFolder);
  QAction *actionProperty = new QAction(tr("Property"));
  connect(actionProperty, &QAction::triggered, this, &PlaylistSearchDialog::actionShowPropertyDialog);
  m->addAction(actionDelete);
  m->addSeparator();
  m->addAction(actionOpen);
  m->addAction(actionPlay);
  m->addAction(actionProperty);
  return m;
}

void PlaylistSearchDialog::actionDelete() {
  if (m_model->sourceModel() == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() <= 0) {
    return;
  }
  QList<int> indexes;
  for (auto t : m_tableViewSelectedRows) {
    const int tt = m_model->mapToSource(t).row();
    indexes.append(tt);
  }
  emit deleteTriggered(indexes);
}

void PlaylistSearchDialog::actionPlay() {
  if (m_model->sourceModel() == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() <= 0) {
    return;
  }
  updatePlayContent(m_tableViewSelectedRows[0]);
}

void PlaylistSearchDialog::actionOpenInFolder() {
  if (m_model->sourceModel() == nullptr) {
    return;
  }
  if (m_tableViewSelectedRows.count() <= 0) {
    return;
  }
  emit openFileInDirTriggered(m_model->mapToSource(m_tableViewSelectedRows[0]).row());
}

void PlaylistSearchDialog::actionShowPropertyDialog() {
  if (m_tableViewSelectedRows.count() <= 0) {
    return;
  }
  auto sourceModel = dynamic_cast<PlaylistModel *>(m_model->sourceModel());
  if (sourceModel == nullptr) {
    return;
  }
  const QModelIndex i = m_tableViewSelectedRows[0];
  PlayContentPos c = sourceModel->content(m_model->mapToSource(i).row());
  if (c.index < 0 || c.content == nullptr) {
    return;
  }
  AudioInfoDialog *dialog = new AudioInfoDialog(c.content, this);

  connect(dialog, &AudioInfoDialog::updatePlayContentRequested, this, [this, sourceModel](PlayContent *playContent) {
    sourceModel->updatePlayContent(playContent);
    if (sourceModel != nullptr && sourceModel->currentPlayContent().index >= 0 &&
        sourceModel->currentPlayContent().content != nullptr &&
        playContent->contentPath == sourceModel->currentPlayContent().content->contentPath) {
      emit this->playContentInfoChanged(playContent);
    }
  });
  dialog->exec();
}
