#include "playlistsearchfiltermodel.h"

Model::PlaylistSearchFilterModel::PlaylistSearchFilterModel(QObject *parent)
    : PlaylistFilterModel(parent) {}

void Model::PlaylistSearchFilterModel::setFilterExp(
    const QRegExp &filter,
    const Model::PlaylistSearchFilterModel::FilterMode &mode) {
  m_filterExp = filter;
  m_filterMode = mode;
  setFilterRegExp(filter);
}

bool Model::PlaylistSearchFilterModel::filterAcceptsRow(
    int sourceRow, const QModelIndex &sourceParent) const {
  if (m_filterExp.isEmpty()) {
    return false;
  }
  QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
  if (sourceModel()->data(index0).toString().contains(m_filterExp)) {
    return true;
  }
  return false;
}
