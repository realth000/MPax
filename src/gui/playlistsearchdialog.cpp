#include "playlistsearchdialog.h"

#include <QtCore/QDebug>

#include "ui_playlistsearchdialog.h"
#include "util/cssloader.h"

PlaylistSearchDialog::PlaylistSearchDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::PlaylistSearchDialog),
      m_model(new Model::PlaylistSearchFilterModel),
      m_tableViewWidthRadio(QList<qreal>{0.5, 0.2, 0.3}),
      m_rowCount(-1),
      m_rowPos(-1) {
  ui->setupUi(this);
  this->setWindowTitle(tr("Search in playlist"));
  ui->searchTableView->verticalHeader()->setHidden(true);
  ui->searchTableView->setSelectionBehavior(QTableView::SelectRows);
  ui->searchTableView->setColumnWidthRatio(m_tableViewWidthRadio);
  ui->searchTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->searchTableView->setSortingEnabled(true);
  //  ui->tableView->sortByColumn(0, Qt::AscendingOrder);
  ui->searchTableView->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->searchTableView->setFocusPolicy(Qt::NoFocus);
  this->setStyleSheet(
      util::loadCssFromFile({":/css/base.css", ":/css/playlistwidget.css"}));
  //  m_model->setSourceModel(model);
  ui->caseCheckBox->setFocusPolicy(Qt::NoFocus);
  ui->searchLineEdit->setFocus();
  ui->searchTableView->setModel(m_model);
  ui->searchTableView->setSortingEnabled(false);

  connect(ui->searchLineEdit, &QLineEdit::textEdited, this,
          &PlaylistSearchDialog::updateSearchFilter);
  connect(ui->caseCheckBox, &QCheckBox::stateChanged, this,
          [this]() { updateSearchFilter(ui->searchLineEdit->text()); });
  connect(ui->searchTableView, &QTableView::doubleClicked, this,
          QOverload<const QModelIndex &>::of(
              &PlaylistSearchDialog::updatePlayContent));
  connect(m_model, &Model::PlaylistSearchFilterModel::rowCountChanged, this,
          &PlaylistSearchDialog::updateRowCountAfterFilter);
  connect(ui->searchTableView, &QTableView::pressed, this,
          [this](const QModelIndex &index) { this->m_rowPos = index.row(); });
}

PlaylistSearchDialog::~PlaylistSearchDialog() {}

void PlaylistSearchDialog::setModel(QAbstractItemModel *model) {
  ui->searchTableView->setColumnWidthRatio(m_tableViewWidthRadio);
  m_model->setSourceModel(model);
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
  if (event->modifiers() == Qt::KeypadModifier &&
      event->key() == Qt::Key_Enter) {
    updatePlayContent(m_rowPos);
    return;
  }
  QDialog::keyPressEvent(event);
}

Qt::CaseSensitivity PlaylistSearchDialog::caseSensitivity() {
  return ui->caseCheckBox->isChecked() ? Qt::CaseSensitive
                                       : Qt::CaseInsensitive;
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
  m_model->setFilterExp(QRegExp(filterString, caseSensitivity()),
                        Model::PlaylistSearchFilterModel::FilterMode::Text);
}

void PlaylistSearchDialog::updatePlayContent(const QModelIndex &index) {
  const int row = m_model->sourceIndex(index).row();
  emit playContentChanged(row);
}

void PlaylistSearchDialog::updatePlayContent(const int &row) {
  updatePlayContent(m_model->index(row, 0));
}
