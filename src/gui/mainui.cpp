#include "mainui.h"
#include "./ui_mainui.h"

#include <QtCore/QtDebug>

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI),
      m_corePlayer(new CorePlayer),
      m_corePlayerState(QMediaPlayer::StoppedState),
      m_volMute(false),
      m_vol(50)
{
    ui->setupUi(this);
    InitConfig();
    InitConnections();
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::InitConfig()
{
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setMinimum(0);
    ui->volumeSlider->setValue(m_vol);
    m_corePlayer->setVol(m_vol);
    if(m_vol == 0) {
        m_volMute = true;
    }
    m_corePlayer->setVolMute(m_volMute);
}

void MainUI::InitConnections()
{
    connect(ui->playButton, &QPushButton::clicked, this, &MainUI::updatePlay);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainUI::stopPlay);
    connect(ui->muteButton, &QPushButton::clicked, this, &MainUI::updateMute);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &MainUI::updateVolume);

    connect(m_corePlayer, &CorePlayer::playStateChanged, this, &MainUI::updatePlayerState);
}

void MainUI::UpdateCurrentUrl()
{
    m_currentContentUrl.setScheme("file");
    m_currentContentUrl.setPath("/home/th000/Desktop/QtProjects/MPax/Aoibridge.mp3");
}

void MainUI::updatePlay()
{
    switch (m_corePlayer->PlayState()) {
    case QMediaPlayer::PlayingState:
        m_corePlayer->pause();
        break;
    case QMediaPlayer::PausedState:
        m_corePlayer->play();
        break;
    case QMediaPlayer::StoppedState:
        UpdateCurrentUrl();
        m_corePlayer->play(m_currentContentUrl);
        break;
    default:
        break;
    }
}

void MainUI::updatePlayerState(const QMediaPlayer::State &state)
{
    m_corePlayerState = state;
}

void MainUI::stopPlay()
{
    m_corePlayer->stop();
}

void MainUI::updateMute()
{
    updateMuteWithValue(!m_volMute);
}

void MainUI::updateMuteWithValue(const bool &muted)
{
    m_volMute = muted;
    m_corePlayer->setVolMute(m_volMute);
}

void MainUI::updateVolume(const int &vol)
{
    if (vol < 0 || vol > 100) {
        qDebug() << "invalid volume" << vol;
        return;
    }
    if (vol == 0) {
        updateMuteWithValue(true);
    } else if (m_volMute) {
        updateMuteWithValue(false);
    }
    m_corePlayer->setVol(vol);
}
