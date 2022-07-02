#ifndef MPAX_PLAYLISTSEARCHFILTERMODEL_H
#define MPAX_PLAYLISTSEARCHFILTERMODEL_H

#include <QtCore/QRegExp>

#include "model/playlistfiltermodel.h"

namespace Model {
class PlaylistSearchFilterModel : public PlaylistFilterModel {
  Q_OBJECT
 public:
  enum class FilterMode : int { Text = 0, Blob, Regexp };
  Q_ENUM(FilterMode)

  explicit PlaylistSearchFilterModel(QObject *parent = nullptr);

 public slots:
  void setFilterExp(const QRegExp &filter,
                    const Model::PlaylistSearchFilterModel::FilterMode &mode);

 protected:
  bool filterAcceptsRow(int sourceRow,
                        const QModelIndex &sourceParent) const override;

 private:
  QRegExp m_filterExp;
  FilterMode m_filterMode;
};
}  // namespace Model

#endif  // MPAX_PLAYLISTSEARCHFILTERMODEL_H
