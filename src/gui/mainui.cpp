#include "mainui.h"

#include <QtCore/QtDebug>
#include <QtWidgets/QFileDialog>

#include "./ui_mainui.h"
#include "audio/audioscanner.h"
#include "config/appconfig.h"
#include "core/playlistjson.h"

MainUI::MainUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainUI) {
  ui->setupUi(this);
  this->setMinimumSize(800, 600);
  this->setWindowTitle(QStringLiteral("MPax"));
  InitConnections();
  Config::AppConfig::getInstance()->loadConfig();
  Config::AppConfig::getInstance()->printConfig();
  emit updateConfig();
}

MainUI::~MainUI() { delete ui; }

void MainUI::InitConnections() {
  connect(ui->openAudioAction, &QAction::triggered, this, &MainUI::openAudio);
  connect(ui->playControlWidget, &PlayControlWidget::playPre, this,
          &MainUI::playPre);
  connect(ui->playControlWidget, &PlayControlWidget::playNext, this,
          &MainUI::playNext);
  connect(ui->playControlWidget, &PlayControlWidget::playRandom, this,
          &MainUI::playRandom);
  connect(ui->playlistAddAction, &QAction::triggered, this,
          &MainUI::addPlaylist);
  connect(ui->listTabWidget, &ListTabWidget::currentPlaylistChanged,
          ui->playlistWidget, &PlaylistWidget::setModel);
  connect(ui->scanDirAction, &QAction::triggered, this, &MainUI::scanAudioDir);
  connect(ui->playlistWidget, &PlaylistWidget::playContent, this,
          QOverload<PlayContent *>::of(&MainUI::playAudio));
  connect(ui->savePlaylistAction, &QAction::triggered, this,
          &MainUI::savePlaylist);
  connect(ui->saveAllPlaylistAction, &QAction::triggered, this,
          &MainUI::saveAllPlaylist);
  connect(this, &MainUI::updateConfig, ui->playlistWidget,
          &PlaylistWidget::updateConfig);
  connect(this, &MainUI::updateConfig, ui->playControlWidget,
          &PlayControlWidget::updateConfig);
  connect(this, &MainUI::updateConfig, this, &MainUI::loadPlaylist);
}

void MainUI::openAudio() {
  const QString filePath = QFileDialog::getOpenFileName(
      this, "Open audio", QCoreApplication::applicationFilePath(),
      "MP3(*.mp3)");
  if (filePath.isEmpty()) {
    return;
  }
  playAudio(addAudioFile(filePath));
}

void MainUI::addPlaylist() {
  ui->listTabWidget->addPlaylist(new PlaylistModel(
      DEFAULT_PLAYLIST_NAME, PlaylistModelHeader::defaultHeaderList()));
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
  if (ui->playControlWidget->playMode() ==
      PlayControlWidget::PlayMode::Random) {
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
  const QString dirPath =
      QFileDialog::getExistingDirectory(this, "Scan directory");
  if (dirPath.isEmpty()) {
    return;
  }
  for (const auto &audioFile :
       AudioScanner::scanAudioInDir(dirPath, QStringList{"mp3"})) {
    addAudioFile(audioFile);
  }
}

PlayContent *MainUI::addAudioFile(const QString &filePath) {
  PlayContent *t = new PlayContent(filePath);
  ui->listTabWidget->addContent(t);
  return t;
}

void MainUI::playAudio(PlayContent *content) {
  ui->playControlWidget->updatePlayInfo(content);
  ui->playControlWidget->setContentPath(content->contentPath);
  ui->playlistWidget->setCurrentContent(content);
}

void MainUI::playAudio(const int &index) {
  ui->playlistWidget->setCurrentContent(index);
  const PlayContent *content = ui->playlistWidget->currentPlayContent();
  if (content == nullptr) {
    return;
  }
  ui->playControlWidget->setContent(content);
}

void MainUI::savePlaylist() {
  const QString filePath = QFileDialog::getSaveFileName(this, "Save playlist");
  if (filePath.isEmpty()) {
    return;
  }
  ui->listTabWidget->savePlaylist(filePath);
}

void MainUI::saveAllPlaylist() {
  const QString filePath = QFileDialog::getSaveFileName(this, "Save playlist");
  if (filePath.isEmpty()) {
    return;
  }
  ui->listTabWidget->saveAllPlaylist(filePath);
}

void MainUI::loadPlaylist() {
  const QStringList allPlaylistPath = Config::AppConfig::getInstance()
                                          ->config(CONFIG_ALL_PLAYLIST)
                                          .value.toStringList();
  const int currentPlaylist = Config::AppConfig::getInstance()
                                  ->config(CONFIG_CUR_PLAYLIST)
                                  .value.toInt();
  const int currentPlayContent = Config::AppConfig::getInstance()
                                     ->config(CONFIG_CUR_PLAYCONTENT)
                                     .value.toInt();
  QStringList::const_iterator it = allPlaylistPath.constBegin();
  while (it != allPlaylistPath.constEnd()) {
    QList<Playlist> playlistList = PlaylistJson::fromJsonString((*it));
    ui->listTabWidget->addPlaylist(playlistList);
    it++;
  }
  ui->listTabWidget->setCurrentPlaylist(currentPlaylist);
  playAudio(currentPlayContent);
}
