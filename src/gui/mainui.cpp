#include "mainui.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QtCore/QFutureWatcher>
#include <QtCore/QtDebug>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "./ui_mainui.h"
#include "audio/audioscanner.h"
#include "config/appconfig.h"
#include "config/appplaylist.h"
#include "core/playlistjson.h"
#include "core/playlistsql.h"
#include "gui/aboutdialog.h"
#include "gui/progressdialog.h"
#include "util/cssloader.h"

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI),
      m_history(new QList<Ui::PlayContentPair>),
      m_historyPos(0),
      m_searchDialog(new PlaylistSearchDialog(this)),
      m_statusLabel(new QLabel) {
  ui->setupUi(this);
  this->setWindowIcon(QIcon(":/pic/logo/MPax.svg"));
  this->setMinimumSize(800, 600);
  this->setWindowTitle(QStringLiteral("MPax"));
  this->setStyleSheet(util::loadCssFromFile(
      {":/css/external/MaterialDark.css", ":/css/base.css"}));
  InitStatusBar();
  Config::AppConfig::getInstance()->loadConfig();
  //  QList<Playlist> playlistList =
  //      Config::AppPlaylist::loadPlaylist(CONFIG_PLAYLIST_FILE_PATH);
  QList<Playlist> playlistList = PlaylistSql::getInstance()->loadPlaylist();
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
  ui->playlistWidget->updatePlayingModel();
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
  playAudioInShowingList(cp.index, cp.content);
}

MainUI::~MainUI() {
  delete ui;
  delete m_history;
  delete m_searchDialog;
}

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
  connect(ui->playlistWidget, &PlaylistWidget::playingListChanged, this,
          &MainUI::savePlayingListIndex);
  connect(ui->scanDirAction, &QAction::triggered, this, &MainUI::scanAudioDir);
  connect(ui->playlistWidget, &PlaylistWidget::playContentChanged, this,
          &MainUI::handleDoubleClickPlay);
  connect(ui->playlistWidget, &PlaylistWidget::playlistChanged,
          ui->listTabWidget, &ListTabWidget::saveDefaultPlaylist);
  connect(ui->saveAllPlaylistAction, &QAction::triggered, this,
          &MainUI::saveAllPlaylist);
  connect(this, &MainUI::updateConfig, ui->playlistWidget,
          &PlaylistWidget::updateConfig);
  connect(this, &MainUI::updateConfig, ui->playControlWidget,
          &PlayControlWidget::updateConfig);
  connect(ui->saveSettingsAction, &QAction::triggered, this,
          &MainUI::saveConfig);
  connect(ui->searchPlaylistAction, &QAction::triggered, this,
          &MainUI::openSearchWindow);
  connect(m_searchDialog, &PlaylistSearchDialog::playContentChanged, this,
          [this](const int &index) {
            ui->playlistWidget->setCurrentContent(index);
            PlayContent *content =
                ui->playlistWidget->currentPlayContent().content;
            if (content == nullptr) {
              return;
            }
            ui->playControlWidget->updatePlayInfo(content);
            ui->playControlWidget->setContentPath(content->contentPath);
            ui->playlistWidget->setShowingListCurrentContent(content);
            addHistory(PlayContentPos{index, content});
            playAudioInShowingList(index, content);
          });
  connect(ui->listTabWidget, &ListTabWidget::currentPlaylistChanged,
          m_searchDialog, &PlaylistSearchDialog::setModel);
  connect(ui->aboutAction, &QAction::triggered, this, &MainUI::showAboutInfo);
  connect(ui->aboutQtAction, &QAction::triggered, this,
          &MainUI::showAboutQtInfo);
  connect(ui->listTabWidget, &ListTabWidget::reloadInfoStatusChanged, this,
          &MainUI::updateReloadInfoStatus);
  connect(ui->playlistWidget, &PlaylistWidget::playlistOrderChanged,
          ui->listTabWidget, &ListTabWidget::saveCurrentPlaylist);
}

void MainUI::keyPressEvent(QKeyEvent *event) {
  if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F) {
    if (!m_searchDialog->isVisible()) {
      openSearchWindow();
      event->accept();
    }
  }
}

void MainUI::openAudio() {
  const QString filePath = QFileDialog::getOpenFileName(
      this, tr("Open audio"), QCoreApplication::applicationFilePath(),
      tr("Audio files") + "(*.mp3 *.flac *.wav);;");
  if (filePath.isEmpty()) {
    return;
  }
  playAudioInShowingList(ui->playlistWidget->countShowing() - 1,
                         addAudioFile(filePath));
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
  } else if (m_history->count() > 0 && m_historyPos - 1 < m_history->count() &&
             m_historyPos - 1 >= 0) {
    m_historyPos--;
    cp.index = (*m_history)[m_historyPos].first;
    cp.content = (*m_history)[m_historyPos].second;
  } else {
    cp = ui->playlistWidget->preContent();
    addHistory(cp);
  }
  if (cp.content == nullptr) {
    qDebug() << "can not find previous one";
    return;
  }
  playAudioInPlayingList(cp.index, cp.content);
}

void MainUI::playNext() {
  PlayContentPos cp = PlayContentPos{-1, nullptr};
  if (m_history->count() > 0 && m_historyPos + 1 < m_history->count()) {
    m_historyPos++;
    cp.index = (*m_history)[m_historyPos].first;
    cp.content = (*m_history)[m_historyPos].second;
  } else {
    if (ui->playControlWidget->playMode() ==
        PlayControlWidget::PlayMode::Random) {
      cp = ui->playlistWidget->randomContent();
    } else {
      cp = ui->playlistWidget->nextContent();
    }
    addHistory(cp);
  }
  if (cp.content == nullptr) {
    qDebug() << "can not find next one";
    return;
  }
  playAudioInPlayingList(cp.index, cp.content);
}

void MainUI::playRandom() {
  PlayContentPos cp = ui->playlistWidget->randomContent();
  if (cp.content == nullptr) {
    qDebug() << "can not find random one";
    return;
  }
  playAudioInPlayingList(cp.index, cp.content);
}

void MainUI::scanAudioDir() {
  const QString dirPath =
      QFileDialog::getExistingDirectory(this, tr("Scan directory"));
  if (dirPath.isEmpty()) {
    return;
  }
  AudioScanner *scanner = new AudioScanner;
  ProgressDialog *dialog = new ProgressDialog("Scanning directory");
  dialog->setInfinite(true);
  // Scan files.
  connect(
      scanner, &AudioScanner::scanStatusChanged, this,
      [this, dialog, scanner](bool finished, const qint64 &count) {
        if (dialog->infinite()) {
          dialog->updateProgress(count);
        }
        if (!finished) {
          return;
        }
        // Load audio info.
        dialog->setMax(scanner->audioFileList().length());
        dialog->setInfinite(false);
        dialog->setWorkName(tr("Loading audio info"));
        QFutureWatcher<void> watcher;
        QTimer timer;
        QEventLoop waitLoop;
        int loadFinishCount;
        connect(&timer, &QTimer::timeout, this, [dialog, &loadFinishCount]() {
          dialog->updateProgress(loadFinishCount);
        });
        connect(&watcher, &QFutureWatcher<void>::finished, this,
                [scanner, dialog, &timer, &waitLoop]() {
                  dialog->deleteLater();
                  scanner->deleteLater();
                  timer.stop();
                  waitLoop.quit();
                });
        timer.start(100);
        QStringList scannedPathList = scanner->audioFileList();
        if (ui->listTabWidget->CurrentPlaylist() != nullptr) {
          const PlayContentList *currentContentList =
              ui->listTabWidget->CurrentPlaylist()->list().content();
          QStringList contentPathList;
          for (const auto content : *currentContentList) {
            contentPathList.append(content->contentPath);
          }
          for (const auto &contentPath : contentPathList) {
            if (scannedPathList.contains(contentPath)) {
              scannedPathList.removeAll(contentPath);
            }
          }
        }
        watcher.setFuture(QtConcurrent::run(this, &MainUI::addAudioFileList,
                                            scannedPathList, &loadFinishCount));
        waitLoop.exec();
        ui->listTabWidget->saveDefaultPlaylist();
      });
  const QStringList formatList = QStringList{"mp3", "flac", "wav"};
  scanner->scanDir(dirPath, formatList);
  dialog->exec();
  //  QMetaObject::invokeMethod(scanner, "scanDir", Qt::QueuedConnection,
  //                            Q_ARG(QString, dirPath),
  //                            Q_ARG(QStringList, formatList));
}

PlayContent *MainUI::addAudioFile(const QString &filePath) {
  PlayContent *t = new PlayContent(filePath);
  ui->listTabWidget->addContent(t);
  return t;
}

void MainUI::addAudioFileList(const QStringList &filePathList, int *counter) {
  if (counter == nullptr) {
    for (const auto &filePath : filePathList) {
      addAudioFile(filePath);
    }
  } else {
    *counter = 0;
    for (const auto &filePath : filePathList) {
      (*counter)++;
      addAudioFile(filePath);
    }
  }
}

void MainUI::playAudioInShowingList(const int &index, PlayContent *content) {
  ui->playControlWidget->updatePlayInfo(content);
  ui->playControlWidget->setContentPath(content->contentPath);
  ui->playlistWidget->setShowingListCurrentContent(content);
  Config::AppConfig::getInstance()->setConfig(CONFIG_CUR_PLAYCONTENT, index);
}

void MainUI::playAudioInPlayingList(const int &index, PlayContent *content) {
  ui->playControlWidget->updatePlayInfo(content);
  ui->playControlWidget->setContentPath(content->contentPath);
  ui->playlistWidget->setPlayingListCurrentContent(content);
  Config::AppConfig::getInstance()->setConfig(CONFIG_CUR_PLAYCONTENT, index);
}

void MainUI::saveAllPlaylist() {
  const QString filePath =
      QFileDialog::getSaveFileName(this, tr("Save playlist"));
  if (filePath.isEmpty()) {
    return;
  }
  ui->listTabWidget->saveAllPlaylist(filePath);
}

void MainUI::saveConfig() {
  Config::AppConfig::getInstance()->saveConfigSoon();
}

void MainUI::savePlayingListIndex(PlaylistModel *playlistModel) {
  Config::AppConfig::getInstance()->setConfig(
      CONFIG_CUR_PLAYLIST, ui->listTabWidget->indexOf(playlistModel));
}

void MainUI::removeLastHistory() {
  if (m_historyPos == m_history->count()) {
    m_historyPos--;
  }
  m_history->removeLast();
}

void MainUI::handleDoubleClickPlay(const int &index, PlayContent *content) {
  addHistory(PlayContentPos{index, content});
  playAudioInShowingList(index, content);
}

void MainUI::InitStatusBar() {
  ui->statusbar->addPermanentWidget(m_statusLabel);
}

void MainUI::addHistory(const PlayContentPos &cp) {
  m_history->append(Ui::PlayContentPair(cp.index, cp.content));
  m_historyPos = m_history->count() - 1;
}

void MainUI::openSearchWindow() {
  m_searchDialog->setModel(ui->listTabWidget->CurrentPlaylist());
  m_searchDialog->show();
}

void MainUI::showAboutInfo() {
  AboutDialog *aboutDialog = new AboutDialog(this);
  aboutDialog->setWindowTitle(tr("About MPax"));
  aboutDialog->exec();
}

void MainUI::showAboutQtInfo() { QMessageBox::aboutQt(this, tr("About Qt")); }

void MainUI::updateReloadInfoStatus(const QString &playlistName, bool finished,
                                    int count, qint64 time) {
  if (!finished) {
    m_statusLabel->setText(
        QString(tr("reloading audio info") + " %1/%2ms")
            .arg(QString::number(count), QString::number(time)));
  } else {
    m_statusLabel->setText(
        QString(tr("reloaded audio info") + " %1/%2ms")
            .arg(QString::number(count), QString::number(time)));
  }
}
