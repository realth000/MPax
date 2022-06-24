#ifndef MAINUI_H
#define MAINUI_H

#include <QtWidgets/QMainWindow>

#include "core/coreplayer.h"
#include "core/listtabmodel.h"
#include "core/playlistmodel.h"

QT_BEGIN_NAMESPACE
    namespace Ui { class MainUI; }
QT_END_NAMESPACE
class MainUI : public QMainWindow
{
    Q_OBJECT

public:
    MainUI(QWidget *parent = nullptr);
    ~MainUI();
    void InitConnections();

private:
    Ui::MainUI *ui;

    PlayContent* addAudioFile(const QString &filePath);

private slots:
    void openAudio();
    void scanAudioDir();
    void addPlaylist();
    void checkIncomingContent(PlayContent *content);
    void playPre();
    void playNext();
    void playRandom();
    void playAudio(PlayContent *content);
};
#endif // MAINUI_H
