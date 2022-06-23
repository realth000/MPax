#ifndef MAINUI_H
#define MAINUI_H

#include <QtWidgets/QMainWindow>

#include "core/coreplayer.h"
#include "core/listtabmodel.h"
#include "core/playlistmodel.h"

#define DEFAULT_PLAYLIST_NAME "default"

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
    PlaylistModel *m_playlistModel;
    ListTabModel *m_listTabModel;

private slots:
    void openAudio();
    void addPlaylist();
    void checkIncommingContent(const PlayContent *content);
};
#endif // MAINUI_H
