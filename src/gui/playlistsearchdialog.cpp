#include "playlistsearchdialog.h"

#include <QtCore/QDebug>

#include "ui_playlistsearchdialog.h"
#include "util/cssloader.h"

PlaylistSearchDialog::PlaylistSearchDialog(QWidget *parent,
                                           QAbstractItemModel *model)
    : QDialog(parent),
      ui(new Ui::PlaylistSearchDialog),
      m_model(new Model::PlaylistSearchFilterModel) {
  ui->setupUi(this);
  this->setWindowTitle(tr("Search in playlist"));
  ui->searchTableView->verticalHeader()->setHidden(true);
  ui->searchTableView->horizontalHeader()->setStretchLastSection(true);
  ui->searchTableView->setSelectionBehavior(QTableView::SelectRows);
  ui->searchTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  //        ui->tableView->setItemDelegate(new NoFocusDelegate);
  // Set tableView row height.
  ui->searchTableView->verticalHeader()->setDefaultSectionSize(30);
  ui->searchTableView->setSortingEnabled(true);
  //  ui->tableView->sortByColumn(0, Qt::AscendingOrder);
  ui->searchTableView->setContextMenuPolicy(Qt::CustomContextMenu);
  this->setStyleSheet(util::loadCssFromFile(":/css/playlistwidget.css"));
  m_model->setSourceModel(model);
  ui->searchTableView->setModel(m_model);

  connect(ui->searchLineEdit, &QLineEdit::textEdited, this,
          &PlaylistSearchDialog::updateSearchFilter);
  connect(ui->caseCheckBox, &QCheckBox::stateChanged, this,
          [this]() { updateSearchFilter(ui->searchLineEdit->text()); });
  connect(ui->searchTableView, &QTableView::doubleClicked, this,
          &PlaylistSearchDialog::updatePlayContent);
}

PlaylistSearchDialog::~PlaylistSearchDialog() {}

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
