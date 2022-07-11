#ifndef PLAYLISTSEARCHDIALOG_H
#define PLAYLISTSEARCHDIALOG_H

#include <QtCore/QRegExp>
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QDialog>

#include "core/playcontent.h"
#include "model/playlistsearchfiltermodel.h"

namespace Ui {
class PlaylistSearchDialog;
}  // namespace Ui

class PlaylistSearchDialog : public QDialog {
  Q_OBJECT

 public:
  explicit PlaylistSearchDialog(QWidget *parent);
  ~PlaylistSearchDialog();

 signals:
  void playContentChanged(const int &row);

 public slots:
  void setModel(QAbstractItemModel *model);

 private:
  Ui::PlaylistSearchDialog *ui;
  Model::PlaylistSearchFilterModel *m_model;
  QList<qreal> m_tableViewWidthRadio;

  Qt::CaseSensitivity caseSensitivity();

 private slots:
  void updateSearchFilter(const QString &filterString);
  void updatePlayContent(const QModelIndex &index);
};

#endif  // PLAYLISTSEARCHDIALOG_H
