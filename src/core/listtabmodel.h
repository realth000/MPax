#ifndef LISTTABMODEL_H
#define LISTTABMODEL_H

#include <QtCore/QList>
#include <QtCore/QStringListModel>

#include "core/playlistmodel.h"

class ListTabModel : public QStringListModel
{
public:
    ListTabModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addPlaylist(PlaylistModel *playlistModel);

private:
    QList<PlaylistModel*> m_playlistList;
};

#endif // LISTTABMODEL_H
