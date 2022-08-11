#include "playlistsearchfiltermodel.h"

#include <QDebug>

#include "playlistmodel.h"

Model::PlaylistSearchFilterModel::PlaylistSearchFilterModel(QObject *parent)
    : PlaylistFilterModel(parent) {}

void Model::PlaylistSearchFilterModel::setFilterExp(
    const QRegExp &filter,
    const Model::PlaylistSearchFilterModel::FilterMode &mode) {
  // Check format;
  bool ok = false;
  QString errString;
  m_parser.init(filter.pattern(), &ok, &errString);
  if (!ok) {
    qDebug() << "failed to init search parser" << errString;
    return;
  }
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
  PlaylistModel *m = reinterpret_cast<PlaylistModel *>(sourceModel());
  if (m == nullptr) {
    return false;
  }
  PlayContentPos cp = m->content(sourceRow);
  if (cp.index < 0 || cp.content == nullptr) {
    return false;
  }
  //  m_parser.parse(cp.content);

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
