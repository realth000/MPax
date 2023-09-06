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

QModelIndex PlaylistFilterModel::sourceIndex(
    const QModelIndex &proxyIndex) const {
  return mapToSource(proxyIndex);
}
QModelIndex PlaylistFilterModel::seekSourceRow(const QModelIndex &proxyIndex,
                                               const int &offset) const {
  if (sourceModel()->rowCount() == 0) {
    return mapToSource(index(0, 0));
  }
  const int nIndex = ((sourceModel()->rowCount() + offset + proxyIndex.row()) %
                          sourceModel()->rowCount() +
                      sourceModel()->rowCount()) %
                     sourceModel()->rowCount();
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
  return mapToSource(proxyIndex.siblingAtRow(nIndex));
#else
  return mapToSource(proxyIndex.sibling(nIndex, proxyIndex.column()));
#endif
}

void PlaylistFilterModel::setSourceModel(QAbstractItemModel *sourceModel) {
  QSortFilterProxyModel::setSourceModel(sourceModel);
}

QModelIndex PlaylistFilterModel::fromSourceIndex(
    const QModelIndex &sourceIndex) const {
  return mapFromSource(sourceIndex);
}

void PlaylistFilterModel::reloadPlaylistByOrder(int column,
                                                Qt::SortOrder order) {
  if (sourceModel() == nullptr) {
    return;
  }
  auto m = dynamic_cast<PlaylistModel *>(sourceModel());
  if (m == nullptr) {
    return;
  }
  m->reloadPlaylistWithOrder(column, order);
}
