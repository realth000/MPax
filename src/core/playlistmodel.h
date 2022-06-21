#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QtCore/QAbstractItemModel>

#include "core/playcontent.h"

typedef QPair<QString, bool> PlaylistHeaderItem;

class PlayConentInfo {

};

class PlaylistModelHeader {
public:
    explicit PlaylistModelHeader(const QList<PlaylistHeaderItem> &headerList);
    int headerCount() const;
    int usedHeaderCount() const;
    bool setHeaderUsed(const PlaylistHeaderItem &headerItem);
    QString header(const int &index) const;
    QString usedHeader(const int &index) const;
    static QList<PlaylistHeaderItem> defaultHeaderList();

private:
    QList<QPair<QString, bool>> m_header;
};

class PlaylistModel : public QAbstractItemModel
{
public:
    explicit PlaylistModel(QList<PlaylistHeaderItem> = QList<PlaylistHeaderItem>{}, QObject *parent = nullptr);
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
/* Something contains data, data is a list of music information
 * Music information contains meta datas
 */
    QList<PlayContent> m_content;
    PlaylistModelHeader m_header;
};

#endif // PLAYLISTMODEL_H
