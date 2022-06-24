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
    enum class PlayMode {
        ListRepeat = 0,
        SingleRepeat,
        Random
    };
    Q_ENUM(PlayMode)
    explicit PlayControlWidget(QWidget *parent = nullptr);
    ~PlayControlWidget();
    PlayMode playMode() const;
signals:
    // FIXME: New content should be created by something but CorePlayer,
    // and after that the content is passed to CorePlayer and PlaylistModel.
    // But remember, there may still should be a check of the existence of current content
    // if current content changed.
    void contentChanged(PlayContent *content);
    void playPre();
    void playNext();
    void playRandom();

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

    PlayMode m_playMode;

    void InitConfig();
    void InitIconFont();
    void InitConnections();
    QString MiliSecondToString(const qint64 &ms);

private slots:
    void updatePlay();
    void updatePlayerState(const QMediaPlayer::State &state);
    void stopPlay();
    void updateMute();
    void updateMuteWithValue(const bool &muted);
    void updateVolume(const int &vol);
    void updatePlayPosition(const qint64 &position);
    void setPlayPosition();
    void setPlayDuration(const qint64 &duration);
    void updateMuteButtonIcon();
    void updatePlayMode();
    void updatePlayModeButtonIcon();
    void handleMediaStatusChanged(QMediaPlayer::MediaStatus status);
};

#endif // PLAYCONTROLWIDGET_H