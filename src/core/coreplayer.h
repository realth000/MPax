#ifndef COREPLAYER_H
#define COREPLAYER_H

#include <QtCore/QObject>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>

class CorePlayer : public QObject
{
    Q_OBJECT
public:
    explicit CorePlayer(QObject *parent = nullptr);

signals:


public slots:
    void Play(const QUrl &mediaContentUrl);

private:
    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;
};

#endif // COREPLAYER_H
