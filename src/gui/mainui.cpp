#include "mainui.h"
#include "./ui_mainui.h"

#include <QtCore/QtDebug>
#include <QtWidgets/QFileDialog>

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI),
      m_playlistModel(new PlaylistModel("default", PlaylistModelHeader::defaultHeaderList())),
      m_listTabModel(new ListTabModel)
{
    ui->setupUi(this);
    if (ui->playControlWidget == nullptr) {
        qDebug() << "empy";
    }
    this->setMinimumSize(800, 600);
    // Bind playlist view and model.

    ui->playlistWidget->setModel(m_playlistModel);
    m_listTabModel->addPlaylist(m_playlistModel);
    ui->listTabWidget->setModel(m_listTabModel);
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
    connect(ui->playControlWidget, &PlayControlWidget::contentChanged, this, &MainUI::checkIncommingContent);
    connect(ui->playlistAddAction, &QAction::triggered, this, &MainUI::addPlaylist);
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
    m_playlistModel->addContent(t);
    ui->playControlWidget->setContentPath(fileInfo.absoluteFilePath());
}

void MainUI::addPlaylist()
{
    ui->listTabWidget->addPlaylist(new PlaylistModel(DEFAULT_PLAYLIST_NAME));
}

void MainUI::checkIncommingContent(PlayContent *content)
{
    // TODO: Some check if content already in current playlist.
    m_playlistModel->addContent(content);
}

void MainUI::playPre() {

}

void MainUI::playNext() {

}
