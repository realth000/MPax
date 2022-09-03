#ifndef PLAYLISTSEARCHDIALOG_H
#define PLAYLISTSEARCHDIALOG_H

#include <QtCore/QRegExp>
#include <QtCore/QSortFilterProxyModel>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMenu>

#include "core/playcontent.h"
#include "model/playlistmodelheader.h"
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
  void deleteTriggered(const QList<int> &indexes);
  void openFileInDirTriggered(const int &row);

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
  QModelIndexList m_tableViewSelectedRows;
  QMenu *m_tableViewContextMenu;
  const PLModel::PlaylistModelHeader *m_header;

  Qt::CaseSensitivity caseSensitivity();
  void selectPre();
  void selectNext();
  QMenu *initTableViewContextMenu();

 private slots:
  void updateRowCountAfterFilter(const int newCount);
  void updateSearchFilter(const QString &filterString);
  void updatePlayContent(const QModelIndex &index);
  void updatePlayContent(const int &row);
  void openTableViewContextMenu(const QPoint &pos);
  void actionDelete();
  void actionPlay();
  void actionOpenInFolder();
};

#endif  // PLAYLISTSEARCHDIALOG_H
