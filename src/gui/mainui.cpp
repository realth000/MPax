#include "mainui.h"
#include "./ui_mainui.h"

#include <QtCore/QtDebug>
#include <QtWidgets/QFileDialog>

#include "core/audioscanner.h"

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI)
{
    ui->setupUi(this);
    if (ui->playControlWidget == nullptr) {
        qDebug() << "empy";
    }
    this->setMinimumSize(800, 600);
    this->setWindowTitle(QStringLiteral("MPax"));
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
    connect(ui->playControlWidget, &PlayControlWidget::playRandom, this, &MainUI::playRandom);
    connect(ui->playControlWidget, &PlayControlWidget::contentChanged, this, &MainUI::checkIncomingContent);
    connect(ui->playlistAddAction, &QAction::triggered, this, &MainUI::addPlaylist);
    connect(ui->listTabWidget, &ListTabWidget::currentPlaylistChanged, ui->playlistWidget, &PlaylistWidget::setModel);
    connect(ui->scanDirAction, &QAction::triggered, this, &MainUI::scanAudioDir);
    connect(ui->playlistWidget, &PlaylistWidget::playContent, this, &MainUI::playAudio);
}

void MainUI::openAudio()
{
    const QString filePath = QFileDialog::getOpenFileName(this, "Open audio", QCoreApplication::applicationFilePath(), "MP3(*.mp3)");
    if (filePath.isEmpty()) {
        return;
    }
    playAudio(addAudioFile(filePath));
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
    PlayContent *content = ui->playlistWidget->preContent();
    if (content == nullptr) {
        qDebug() << "can not find previous one";
        return;
    }
    playAudio(content);
}

void MainUI::playNext() {
    PlayContent *content;
    if (ui->playControlWidget->playMode() == PlayControlWidget::PlayMode::Random) {
        content = ui->playlistWidget->randomContent();
    } else {
        content = ui->playlistWidget->nextContent();
    }
    if (content == nullptr) {
        qDebug() << "can not find next one";
        return;
    }
    playAudio(content);
}

void MainUI::playRandom() {
    PlayContent *content = ui->playlistWidget->randomContent();
    if (content == nullptr) {
        qDebug() << "can not find random one";
        return;
    }
    playAudio(content);
}

void MainUI::scanAudioDir() {
    const QString dirPath = QFileDialog::getExistingDirectory(this, "Scan directory");
    if (dirPath.isEmpty()) {
        return;
    }
    for (const auto &audioFile : AudioScanner::scanAudioInDir(dirPath, QStringList{"mp3"})) {
        addAudioFile(audioFile);
    }
}

PlayContent * MainUI::addAudioFile(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    PlayContent *t = new PlayContent;
    t->contentPath = fileInfo.absoluteFilePath();
    t->contentName = fileInfo.fileName();
    ui->listTabWidget->addContent(t);
    return t;
}

void MainUI::playAudio(PlayContent *content) {
    ui->playControlWidget->setContentPath(content->contentPath);
    ui->playlistWidget->setCurrentContent(content);
}
