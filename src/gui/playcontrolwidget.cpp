#include "playcontrolwidget.h"
#include "ui_playcontrolwidget.h"

#include <QtCore/QtDebug>

PlayControlWidget::PlayControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayControlWidget),
    m_corePlayer(new CorePlayer),
    m_corePlayerState(QMediaPlayer::StoppedState),
    m_volMute(false),
    m_vol(50)
{
    ui->setupUi(this);
    InitConfig();
    InitConnections();
}

PlayControlWidget::~PlayControlWidget()
{
    delete ui;
}

void PlayControlWidget::setContentPath(const QString &contentPath)
{
    m_currentContentUrl.setPath(contentPath);
    m_corePlayer->stop();
    updatePlay();
}

void PlayControlWidget::InitConfig()
{
    m_currentContentUrl.setScheme("file");
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setMinimum(0);
    ui->volumeSlider->setValue(m_vol);
    m_corePlayer->setVol(m_vol);
    if(m_vol == 0) {
        m_volMute = true;
    }
    m_corePlayer->setVolMute(m_volMute);
}

void PlayControlWidget::InitConnections()
{
    connect(ui->playButton, &QPushButton::clicked, this, &PlayControlWidget::updatePlay);
    connect(ui->stopButton, &QPushButton::clicked, this, &PlayControlWidget::stopPlay);
    connect(ui->muteButton, &QPushButton::clicked, this, &PlayControlWidget::updateMute);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &PlayControlWidget::updateVolume);
    connect(ui->playPosSlider, &QSlider::sliderReleased, this, &PlayControlWidget::setPlayPosition);

    connect(m_corePlayer, &CorePlayer::playStateChanged, this, &PlayControlWidget::updatePlayerState);
    connect(m_corePlayer, &CorePlayer::playPositionChanged, this, &PlayControlWidget::updatePlayPosition);
    connect(m_corePlayer, &CorePlayer::playContentChanged, this, &PlayControlWidget::updatePlayContent);
    connect(m_corePlayer, &CorePlayer::playDurationChanged, this, &PlayControlWidget::updatePlayDuration);
}

QString PlayControlWidget::MiliSecondToString(const qint64 &ms)
{
    int hh = std::chrono::duration_cast<std::chrono::hours>(std::chrono::milliseconds(ms)).count();
    int mm = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::milliseconds(ms)).count() % 60;
    int ss = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(ms)).count() % 60;
    return QString("%1:%2:%3").arg(hh, 2, 10, QLatin1Char('0')).arg(mm, 2, 10, QLatin1Char('0')).arg(ss, 2, 10, QLatin1Char('0'));
}

void PlayControlWidget::updatePlay()
{
    switch (m_corePlayer->PlayState()) {
    case QMediaPlayer::PlayingState:
        m_corePlayer->pause();
        break;
    case QMediaPlayer::PausedState:
        m_corePlayer->play();
        break;
    case QMediaPlayer::StoppedState:
        if (m_currentContentUrl.toLocalFile().isEmpty()) {
            qDebug() << "audio not selected";
            return;
        }
        m_corePlayer->play(m_currentContentUrl);
        break;
    default:
        break;
    }
}

void PlayControlWidget::updatePlayerState(const QMediaPlayer::State &state)
{
    m_corePlayerState = state;
}

void PlayControlWidget::stopPlay()
{
    m_corePlayer->stop();
}

void PlayControlWidget::updateMute()
{
    updateMuteWithValue(!m_volMute);
}

void PlayControlWidget::updateMuteWithValue(const bool &muted)
{
    m_volMute = muted;
    m_corePlayer->setVolMute(m_volMute);
}

void PlayControlWidget::updateVolume(const int &vol)
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

void PlayControlWidget::updatePlayPosition(const qint64 &position)
{
    ui->timePostLabel->setText(MiliSecondToString(position));
    if (ui->playPosSlider->isSliderDown()) {
        return;
    }
    ui->playPosSlider->setValue(position);
}

void PlayControlWidget::updatePlayDuration(const qint64 &duration)
{
    ui->playPosSlider->setMaximum(duration);
    ui->timeTotalLabel->setText(MiliSecondToString(duration));
}

void PlayControlWidget::updatePlayContent(const PlayContent *content)
{
    if (content->duration != 0) {
        updatePlayDuration(content->duration);
    }
    emit contentChanged(content);
}

void PlayControlWidget::setPlayPosition()
{
    qDebug() << "setPlayPosition" << ui->playPosSlider->value();
    m_corePlayer->setPlayPosition(ui->playPosSlider->value());
}
