#ifndef PLAYLISTSEARCHDIALOG_H
#define PLAYLISTSEARCHDIALOG_H

#include <QtCore/QRegExp>
#include <QtCore/QSortFilterProxyModel>
#include <QtGui/QKeyEvent>
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

 protected:
  virtual void keyPressEvent(QKeyEvent *event) override;

 private:
  Ui::PlaylistSearchDialog *ui;
  Model::PlaylistSearchFilterModel *m_model;
  QList<qreal> m_tableViewWidthRadio;
  int m_rowCount;
  int m_rowPos;

  Qt::CaseSensitivity caseSensitivity();
  void selectPre();
  void selectNext();

 private slots:
  void updateRowCountAfterFilter(const int newCount);
  void updateSearchFilter(const QString &filterString);
  void updatePlayContent(const QModelIndex &index);
  void updatePlayContent(const int &row);
};

#endif  // PLAYLISTSEARCHDIALOG_H
