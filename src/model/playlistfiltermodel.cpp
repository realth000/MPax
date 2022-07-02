#include "playlistfiltermodel.h"

PlaylistFilterModel::PlaylistFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent) {}

bool PlaylistFilterModel::filterAcceptsRow(
    int sourceRow, const QModelIndex &sourceParent) const {
  return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

bool PlaylistFilterModel::lessThan(const QModelIndex &left,
                                   const QModelIndex &right) const {
  QVariant leftData = sourceModel()->data(left);
  QVariant rightData = sourceModel()->data(right);

  switch (leftData.userType()) {
    case QMetaType::QString: {
      return QString::localeAwareCompare(leftData.toString(),
                                         rightData.toString()) < 0;
    }
    case QMetaType::Int: {
      return leftData.toInt() < rightData.toInt();
    }
  }
  return QSortFilterProxyModel::lessThan(left, right);
}

QModelIndex PlaylistFilterModel::sourceIndex(
    const QModelIndex &proxyIndex) const {
  return mapToSource(proxyIndex);
}
QModelIndex PlaylistFilterModel::seekSourceRow(const QModelIndex &proxyIndex,
                                               const int &offset) const {
  const int nIndex = proxyIndex.row() + offset;
  if (nIndex >= sourceModel()->rowCount() || nIndex < 0) {
    return mapToSource(index(0, 0));
  }
  return mapToSource(proxyIndex.siblingAtRow(nIndex));
}

QModelIndex PlaylistFilterModel::fromSourceIndex(
    const QModelIndex &sourceIndex) const {
  return mapFromSource(sourceIndex);
}
