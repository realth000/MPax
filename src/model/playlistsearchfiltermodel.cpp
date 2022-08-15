#include "playlistsearchfiltermodel.h"

#include <QDebug>

#include "playlistmodel.h"

Model::PlaylistSearchFilterModel::PlaylistSearchFilterModel(QObject *parent)
    : PlaylistFilterModel(parent), m_validSyntax(false) {}

void Model::PlaylistSearchFilterModel::setFilterExp(
    const QRegExp &filter,
    const Model::PlaylistSearchFilterModel::FilterMode &mode) {
  // Check format;
  bool ok = false;
  QString errString;
  m_parser.init(filter.pattern(), &ok, &errString);
  m_validSyntax = ok;
  if (!m_validSyntax) {
    qDebug() << "failed to init search parser" << errString
             << ", use plain search";
  } else {
    qDebug() << "init search parser";
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
  if (m_validSyntax) {
    // Use search statement
    PlaylistModel *m = reinterpret_cast<PlaylistModel *>(sourceModel());
    if (m == nullptr) {
      return false;
    }
    PlayContentPos cp = m->content(sourceRow);
    if (cp.index < 0 || cp.content == nullptr) {
      return false;
    }
    return m_parser.parse(cp.content, m_filterExp.caseSensitivity());
  } else {
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    const int columnCount = sourceModel()->columnCount(sourceParent);
    for (int i = 0; i < columnCount; i++) {
      if (sourceModel()
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
              ->data(index0.siblingAtColumn(i))
#else
              ->data(index0.sibling(index0.row(), i))
#endif
              .toString()
              .contains(m_filterExp)) {
        return true;
      }
    }
  }
  return false;
}
