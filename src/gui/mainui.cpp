#include "mainui.h"

#include <QtCore/QtDebug>
#include <QtWidgets/QFileDialog>

#include "./ui_mainui.h"
#include "audio/audioscanner.h"
#include "config/appconfig.h"
#include "config/appplaylist.h"
#include "core/playlistjson.h"
#include "util/cssloader.h"

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI),
      m_history(new QList<Ui::PlayContentPair>),
      m_historyPos(0) {
  ui->setupUi(this);
  this->setWindowIcon(QIcon(":/pic/logo/MPax.svg"));
  this->setMinimumSize(800, 600);
  this->setWindowTitle(QStringLiteral("MPax"));
  this->setStyleSheet(util::loadCssFromFile(":/css/external/MaterialDark.css"));
  Config::AppConfig::getInstance()->loadConfig();
  QList<Playlist> playlistList =
      Config::AppPlaylist::loadPlaylist(CONFIG_PLAYLIST_FILE_PATH);

  InitConnections();
  emit updateConfig();

  // Load default playlist from ./mpax.list.conf.
  ui->listTabWidget->addPlaylist(playlistList);
  // Set current Playlist from config.
  const int playlist = Config::AppConfig::getInstance()
                           ->config(CONFIG_CUR_PLAYLIST)
                           .value.toInt();
  if (playlist < 0) {
    qDebug() << "current playlist pos <0";
    return;
  }
  ui->listTabWidget->setCurrentPlaylist(playlist);
  PlaylistModel *model = ui->listTabWidget->CurrentPlaylist();
  if (model == nullptr) {
    qDebug() << "current PlaylistModel is null";
    return;
  }
  ui->playlistWidget->setModel(model);
  const int playContent = Config::AppConfig::getInstance()
                              ->config(CONFIG_CUR_PLAYCONTENT)
                              .value.toInt();
  if (playContent < 0) {
    qDebug() << "current playContent pos < 0";
    return;
  }
  // Set current PlayContent from config.
  ui->playlistWidget->setCurrentContent(playContent);
  // Start play;
  PlayContentPos cp = ui->playlistWidget->currentPlayContent();
  if (cp.index < 0 || cp.content == nullptr) {
    return;
  }
  addHistory(cp);
  playAudio(cp.index, cp.content);
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
  connect(ui->playControlWidget, &PlayControlWidget::playInvalid, this,
          &MainUI::removeLastHistory);
  connect(ui->playlistAddAction, &QAction::triggered, this,
          &MainUI::addPlaylist);
  connect(ui->listTabWidget, &ListTabWidget::currentPlaylistChanged,
          ui->playlistWidget, &PlaylistWidget::setModel);
  connect(ui->listTabWidget, &ListTabWidget::currentPlaylistIndexChanged, this,
          &MainUI::saveCurrentPlaylistIndex);
  connect(ui->scanDirAction, &QAction::triggered, this, &MainUI::scanAudioDir);
  connect(ui->playlistWidget, &PlaylistWidget::playContentChanged, this,
          &MainUI::handleDoubleClickPlay);
  connect(ui->savePlaylistAction, &QAction::triggered, this,
          &MainUI::savePlaylist);
  connect(ui->saveAllPlaylistAction, &QAction::triggered, this,
          &MainUI::saveAllPlaylist);
  connect(this, &MainUI::updateConfig, ui->playlistWidget,
          &PlaylistWidget::updateConfig);
  connect(this, &MainUI::updateConfig, ui->playControlWidget,
          &PlayControlWidget::updateConfig);
  connect(this, &MainUI::updateConfig, this, &MainUI::loadPlaylist);
  connect(ui->saveSettingsAction, &QAction::triggered, this,
          &MainUI::saveConfig);
}

void MainUI::openAudio() {
  const QString filePath = QFileDialog::getOpenFileName(
      this, "Open audio", QCoreApplication::applicationFilePath(),
      "MP3(*.mp3)");
  if (filePath.isEmpty()) {
    return;
  }
  playAudio(ui->playlistWidget->count() - 1, addAudioFile(filePath));
  ui->listTabWidget->saveDefaultPlaylist();
}

void MainUI::addPlaylist() {
  ui->listTabWidget->addPlaylist(new PlaylistModel(
      DEFAULT_PLAYLIST_NAME, PlaylistModelHeader::defaultHeaderList()));
  // This will save empty default playlist.
  // This one is not ui->listTabWidget->addPlaylist, will not cause large IO.
  ui->listTabWidget->saveDefaultPlaylist();
}

void MainUI::playPre() {
  PlayContentPos cp;
  if (ui->playControlWidget->playMode() ==
          PlayControlWidget::PlayMode::Random &&
      m_history->count() > 0 && m_historyPos > 0) {
    m_historyPos--;
    cp.index = (*m_history)[m_historyPos].first;
    cp.content = (*m_history)[m_historyPos].second;
  } else {
    cp = ui->playlistWidget->preContent();
  }
  if (cp.content == nullptr) {
    qDebug() << "can not find previous one";
    return;
  }
  playAudio(cp.index, cp.content);
}

void MainUI::playNext() {
  PlayContentPos cp = PlayContentPos{-1, nullptr};
  if (ui->playControlWidget->playMode() ==
      PlayControlWidget::PlayMode::Random) {
    cp = ui->playlistWidget->randomContent();
    addHistory(cp);
  } else if (m_history->count() > 0 && m_historyPos + 1 < m_history->count()) {
    m_historyPos++;
    cp.index = (*m_history)[m_historyPos].first;
    cp.content = (*m_history)[m_historyPos].second;
  } else {
    cp = ui->playlistWidget->nextContent();
    addHistory(cp);
  }
  if (cp.content == nullptr) {
    qDebug() << "can not find next one";
    return;
  }
  playAudio(cp.index, cp.content);
}

void MainUI::playRandom() {
  PlayContentPos cp = ui->playlistWidget->randomContent();
  if (cp.content == nullptr) {
    qDebug() << "can not find random one";
    return;
  }
  playAudio(cp.index, cp.content);
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
  ui->listTabWidget->saveDefaultPlaylist();
}

PlayContent *MainUI::addAudioFile(const QString &filePath) {
  PlayContent *t = new PlayContent(filePath);
  ui->listTabWidget->addContent(t);
  return t;
}

void MainUI::playAudio(const int &index, PlayContent *content) {
  ui->playControlWidget->updatePlayInfo(content);
  ui->playControlWidget->setContentPath(content->contentPath);
  ui->playlistWidget->setCurrentContent(content);
  Config::AppConfig::getInstance()->setConfig(CONFIG_CUR_PLAYCONTENT, index);
  Config::AppConfig::getInstance()->saveConfigDefer();
}

void MainUI::playAudio(const int &index) {
  ui->playlistWidget->setCurrentContent(index);
  const PlayContent *content = ui->playlistWidget->currentPlayContent().content;
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
  if (allPlaylistPath.isEmpty()) {
    return;
  }
  const int currentPlaylist = Config::AppConfig::getInstance()
                                  ->config(CONFIG_CUR_PLAYLIST)
                                  .value.toInt();
  const int currentPlayContent = Config::AppConfig::getInstance()
                                     ->config(CONFIG_CUR_PLAYCONTENT)
                                     .value.toInt();
  QStringList::const_iterator it = allPlaylistPath.constBegin();
  while (it != allPlaylistPath.constEnd()) {
    QList<Playlist> playlistList = Config::AppPlaylist::loadPlaylist((*it));
    ui->listTabWidget->addPlaylist(playlistList);
    it++;
  }
  ui->listTabWidget->setCurrentPlaylist(currentPlaylist);
  playAudio(currentPlayContent);
}

void MainUI::saveConfig() {
  Config::AppConfig::getInstance()->saveConfigSoon();
}

void MainUI::saveCurrentPlaylistIndex(const int &index) {
  Config::AppConfig::getInstance()->setConfig(CONFIG_CUR_PLAYLIST, index);
  Config::AppConfig::getInstance()->saveConfigDefer();
}

void MainUI::removeLastHistory() {
  if (m_historyPos == m_history->count()) {
    m_historyPos--;
  }
  m_history->removeLast();
}

void MainUI::handleDoubleClickPlay(const int &index, PlayContent *content) {
  addHistory(PlayContentPos{index, content});
  playAudio(index, content);
}

void MainUI::addHistory(const PlayContentPos &cp) {
  m_history->append(Ui::PlayContentPair(cp.index, cp.content));
  m_historyPos = m_history->count() - 1;
}
