#include "playlistsearchfiltermodel.h"

#include <QDebug>

Model::PlaylistSearchFilterModel::PlaylistSearchFilterModel(QObject *parent)
    : PlaylistFilterModel(parent) {}

void Model::PlaylistSearchFilterModel::setFilterExp(
    const QRegExp &filter,
    const Model::PlaylistSearchFilterModel::FilterMode &mode) {
  m_filterExp = filter;
  m_filterMode = mode;
  setFilterRegExp(filter);
  emit rowCountChanged(QSortFilterProxyModel::rowCount(QModelIndex()));
}

bool Model::PlaylistSearchFilterModel::filterAcceptsRow(
    int sourceRow, const QModelIndex &sourceParent) const {
  if (m_filterExp.isEmpty()) {
    return false;
  }
  QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
  const int columnCount = sourceModel()->columnCount(sourceParent);
  for (int i = 0; i < columnCount; i++) {
    if (sourceModel()
            ->data(index0.siblingAtColumn(i))
            .toString()
            .contains(m_filterExp)) {
      return true;
    }
  }

  return false;
}
