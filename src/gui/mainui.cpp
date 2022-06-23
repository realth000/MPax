#include "mainui.h"
#include "./ui_mainui.h"

#include <QtCore/QtDebug>
#include <QtWidgets/QFileDialog>

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI)
{
    ui->setupUi(this);
    if (ui->playControlWidget == nullptr) {
        qDebug() << "empy";
    }
    this->setMinimumSize(800, 600);
    // Bind playlist view and model.

    InitConnections();
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::InitConnections()
{
    connect(ui->openAudioAction, &QAction::triggered, this, &MainUI::openAudio);
    connect(ui->playControlWidget, &PlayControlWidget::playPre, this, &MainUI::playPre);
    connect(ui->playControlWidget, &PlayControlWidget::playNext, this, &MainUI::playNext);
    connect(ui->playControlWidget, &PlayControlWidget::contentChanged, this, &MainUI::checkIncomingContent);
    connect(ui->playlistAddAction, &QAction::triggered, this, &MainUI::addPlaylist);
    connect(ui->listTabWidget, &ListTabWidget::currentPlaylistChanged, ui->playlistWidget, &PlaylistWidget::setModel);
}

void MainUI::openAudio()
{
    const QString filePath = QFileDialog::getOpenFileName(this, "Open audio", QCoreApplication::applicationFilePath(), "MP3(*.mp3)");
    if (filePath.isEmpty()) {
        return;
    }
    QFileInfo fileInfo(filePath);
    PlayContent *t = new PlayContent;
    t->contentPath = fileInfo.absoluteFilePath();
    t->contentName = fileInfo.fileName();
    ui->listTabWidget->addContent(t);
    ui->playControlWidget->setContentPath(fileInfo.absoluteFilePath());
}

void MainUI::addPlaylist()
{
    ui->listTabWidget->addPlaylist(new PlaylistModel(DEFAULT_PLAYLIST_NAME, QList<PlaylistHeaderItem>{PlaylistHeaderItem("ContentPath", true)}));
}

void MainUI::checkIncomingContent(PlayContent *content)
{
    // This slot is triggered by playControlWidget's contentChanged signal.
    // What we need to do here is to do something that should happen on UI when the current PlayContent changed.
}

void MainUI::playPre() {

}

void MainUI::playNext() {

}
