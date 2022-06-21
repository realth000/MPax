#ifndef PLAYCONTROLWIDGET_H
#define PLAYCONTROLWIDGET_H

#include <QtWidgets/QWidget>

#include "core/coreplayer.h"

namespace Ui {
class PlayControlWidget;
}

class PlayControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayControlWidget(QWidget *parent = nullptr);
    ~PlayControlWidget();

signals:
    // FIXME: New content should be created by something but CorePlayer,
    // and after that the content is passed to CorePlayer and PlaylistModel.
    // But remember, there may still should be a check of the existence of current content
    // if current content changed.
    void contentChanged(const PlayContent *content);

public slots:
    void setContentPath(const QString &contentPath);

private:
    Ui::PlayControlWidget *ui;
    CorePlayer *m_corePlayer;
    QUrl m_currentContentUrl;
    QMediaPlayer::State m_corePlayerState;

    // Volume control
    bool m_volMute;
    int m_vol;

    void InitConfig();
    void InitConnections();
    void UpdateCurrentUrl();
    QString MiliSecondToString(const qint64 &ms);

private slots:
    void updatePlay();
    void updatePlayerState(const QMediaPlayer::State &state);
    void stopPlay();
    void updateMute();
    void updateMuteWithValue(const bool &muted);
    void updateVolume(const int &vol);
    void updatePlayPosition(const qint64 &position);
    void updatePlayDuration(const qint64 &duration);
    void updatePlayContent(const PlayContent *content);
    void setPlayPosition();
};

#endif // PLAYCONTROLWIDGET_H
