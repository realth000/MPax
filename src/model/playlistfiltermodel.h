#ifndef MPAX_PLAYLISTFILTERMODEL_H
#define MPAX_PLAYLISTFILTERMODEL_H

#include <QtCore/QSortFilterProxyModel>

class PlaylistFilterModel : public QSortFilterProxyModel {
  Q_OBJECT
 public:
  explicit PlaylistFilterModel(QObject *parent = nullptr);
  QModelIndex sourceIndex(const QModelIndex &proxyIndex) const;
  QModelIndex fromSourceIndex(const QModelIndex &sourceIndex) const;
  QModelIndex seekSourceRow(const QModelIndex &proxyIndex,
                            const int &offset) const;
  virtual void setSourceModel(QAbstractItemModel *sourceModel) override;

 signals:
  void sortFromSql(int column, Qt::SortOrder order);

 protected:
  bool filterAcceptsRow(int sourceRow,
                        const QModelIndex &sourceParent) const override;
  bool lessThan(const QModelIndex &left,
                const QModelIndex &right) const override;
  virtual void sort(int column,
                    Qt::SortOrder order = Qt::AscendingOrder) override;
};

#endif  // MPAX_PLAYLISTFILTERMODEL_H
