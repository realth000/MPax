#ifndef MAINUI_H
#define MAINUI_H

#include <QtWidgets/QMainWindow>

#include "core/coreplayer.h"

QT_BEGIN_NAMESPACE
    namespace Ui { class MainUI; }
QT_END_NAMESPACE
class MainUI : public QMainWindow
{
    Q_OBJECT

public:
    MainUI(QWidget *parent = nullptr);
    ~MainUI();

private:
    Ui::MainUI *ui;
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
#endif // MAINUI_H
