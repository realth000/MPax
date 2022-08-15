#include "playlistfiltermodel.h"

#include <QtCore/QDebug>

#include "model/playlistmodel.h"

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

void PlaylistFilterModel::sort(int column, Qt::SortOrder order) {
  /**
   * Consider the sorting policy in sql as the true order.
   * For example, in ascending order:
   * 1. Alphabets [a-z]
   * 2. Characters in other languages.
   */
  emit sortFromSql(column, order);
#if 0
  QSortFilterProxyModel::sort(column, order);
#endif
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
  return mapToSource(proxyIndex.siblingAtRow(nIndex));
#else
  return mapToSource(proxyIndex.sibling(nIndex, proxyIndex.column()));
#endif
}

void PlaylistFilterModel::setSourceModel(QAbstractItemModel *sourceModel) {
  PlaylistModel *sourcePlaylistModel =
      reinterpret_cast<PlaylistModel *>(sourceModel);
  if (sourcePlaylistModel != nullptr) {
    if (sourceModel != nullptr) {
      disconnect(this, &PlaylistFilterModel::sortFromSql, sourceModel, nullptr);
    }
    connect(this, &PlaylistFilterModel::sortFromSql, sourcePlaylistModel,
            &PlaylistModel::reloadPlaylistWithOrder);
  }
  QSortFilterProxyModel::setSourceModel(sourceModel);
}

QModelIndex PlaylistFilterModel::fromSourceIndex(
    const QModelIndex &sourceIndex) const {
  return mapFromSource(sourceIndex);
}
