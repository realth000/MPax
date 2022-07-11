#include "playlistsearchdialog.h"

#include <QtCore/QDebug>

#include "ui_playlistsearchdialog.h"
#include "util/cssloader.h"

PlaylistSearchDialog::PlaylistSearchDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::PlaylistSearchDialog),
      m_model(new Model::PlaylistSearchFilterModel),
      m_tableViewWidthRadio(QList<qreal>{0.5, 0.2, 0.3}) {
  ui->setupUi(this);
  this->setWindowTitle(tr("Search in playlist"));
  ui->searchTableView->verticalHeader()->setHidden(true);
  ui->searchTableView->horizontalHeader()->setStretchLastSection(false);
  ui->searchTableView->setSelectionBehavior(QTableView::SelectRows);
  ui->searchTableView->setColumnWidthRatio(m_tableViewWidthRadio);
  ui->searchTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->searchTableView->setSortingEnabled(true);
  //  ui->tableView->sortByColumn(0, Qt::AscendingOrder);
  ui->searchTableView->setContextMenuPolicy(Qt::CustomContextMenu);
  this->setStyleSheet(util::loadCssFromFile(":/css/playlistwidget.css"));
  //  m_model->setSourceModel(model);
  ui->caseCheckBox->setFocusPolicy(Qt::NoFocus);
  ui->searchLineEdit->setFocus();
  ui->searchTableView->setModel(m_model);

  connect(ui->searchLineEdit, &QLineEdit::textEdited, this,
          &PlaylistSearchDialog::updateSearchFilter);
  connect(ui->caseCheckBox, &QCheckBox::stateChanged, this,
          [this]() { updateSearchFilter(ui->searchLineEdit->text()); });
  connect(ui->searchTableView, &QTableView::doubleClicked, this,
          &PlaylistSearchDialog::updatePlayContent);
}

PlaylistSearchDialog::~PlaylistSearchDialog() {}

void PlaylistSearchDialog::setModel(QAbstractItemModel *model) {
  m_model->setSourceModel(model);
}

Qt::CaseSensitivity PlaylistSearchDialog::caseSensitivity() {
  return ui->caseCheckBox->isChecked() ? Qt::CaseSensitive
                                       : Qt::CaseInsensitive;
}

void PlaylistSearchDialog::updateSearchFilter(const QString &filterString) {
  m_model->setFilterExp(QRegExp(filterString, caseSensitivity()),
                        Model::PlaylistSearchFilterModel::FilterMode::Text);
}

void PlaylistSearchDialog::updatePlayContent(const QModelIndex &index) {
  const int row = m_model->sourceIndex(index).row();
  emit playContentChanged(row);
}
