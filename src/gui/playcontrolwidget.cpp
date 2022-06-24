#include "playcontrolwidget.h"
#include "ui_playcontrolwidget.h"

#include <QtCore/QtDebug>
#include <QtGui/QFontDatabase>

#include "util/cssloader.h"

#define ICON_COVER    QChar(0xf51f) // 0xf51f 0xf8d9
#define ICON_PLAY     QChar(0xf04b)
#define ICON_PAUSE    QChar(0xf04c)
#define ICON_PRE      QChar(0xf04a)
#define ICON_NEXT     QChar(0xf04e)
#define ICON_STOP     QChar(0xf04d)
#define ICON_VOL_MUTE QChar(0xf6a9) // 0xf2e2, 0xf6a9
#define ICON_VOL_LVL1 QChar(0xf027)
#define ICON_VOL_LVL2 QChar(0xf6a8)
#define ICON_VOL_LVL3 QChar(0xf028)
#define ICON_PLAY_MODE_LIST_REPEAT QChar(0xf079)
#define ICON_PLAY_MODE_SING_REPEAT QChar(0xf365)
#define ICON_PLAY_MODE_LIST_RANDOM QChar(0xf074)

PlayControlWidget::PlayControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayControlWidget),
    m_corePlayer(new CorePlayer),
    m_corePlayerState(QMediaPlayer::StoppedState),
    m_volMute(false),
    m_vol(50),
    m_playMode(PlayMode::ListRepeat)
{
    ui->setupUi(this);
    InitConfig();
    ui->audioInfoGroupBox->setLayout(ui->audoInfoHBoxLayout);
    ui->playPosSlider->setEnabled(false);
    InitCss(":/css/playcontrolwidget.css");
    InitIconFont();
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
    connect(ui->preButton, &QPushButton::clicked, this, &PlayControlWidget::playPre);
    connect(ui->playButton, &QPushButton::clicked, this, &PlayControlWidget::updatePlay);
    connect(ui->nextButton, &QPushButton::clicked, this, &PlayControlWidget::playNext);
    connect(ui->stopButton, &QPushButton::clicked, this, &PlayControlWidget::stopPlay);
    connect(ui->playModeButton, &QPushButton::clicked, this, &PlayControlWidget::updatePlayMode);
    connect(ui->muteButton, &QPushButton::clicked, this, &PlayControlWidget::updateMute);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &PlayControlWidget::updateVolume);
    connect(ui->playPosSlider, &QSlider::sliderReleased, this, &PlayControlWidget::setPlayPosition);

    connect(m_corePlayer, &CorePlayer::playStateChanged, this, &PlayControlWidget::updatePlayerState);
    connect(m_corePlayer, &CorePlayer::playPositionChanged, this, &PlayControlWidget::updatePlayPosition);
    connect(m_corePlayer, &CorePlayer::playDurationChanged, this, &PlayControlWidget::setPlayDuration);
    connect(m_corePlayer, &CorePlayer::playMediaStatusChanged, this, &PlayControlWidget::handleMediaStatusChanged);
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
        ui->playButton->setText(ICON_PLAY);
        ui->playPosSlider->setEnabled(false);
        break;
    case QMediaPlayer::PausedState:
        m_corePlayer->play();
        ui->playButton->setText(ICON_PAUSE);
        ui->playPosSlider->setEnabled(true);
        break;
    case QMediaPlayer::StoppedState:
        if (m_currentContentUrl.toLocalFile().isEmpty()) {
            qDebug() << "audio not selected";
            return;
        }
        m_corePlayer->play(m_currentContentUrl);
        ui->playButton->setText(ICON_PAUSE);
        ui->playPosSlider->setEnabled(true);
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
    ui->playButton->setText(ICON_PLAY);
}

void PlayControlWidget::updateMute()
{
    updateMuteWithValue(!m_volMute);
}

void PlayControlWidget::updateMuteWithValue(const bool &muted)
{
    m_volMute = muted;
    m_corePlayer->setVolMute(m_volMute);
    updateMuteButtonIcon();
}

void PlayControlWidget::updateVolume(const int &vol)
{
    if (vol < 0 || vol > 100) {
        qDebug() << "invalid volume" << vol;
        return;
    }
    m_vol = vol;
    if (m_vol == 0) {
        updateMuteWithValue(true);
    } else if (m_volMute) {
        updateMuteWithValue(false);
    }
    m_corePlayer->setVol(m_vol);
    updateMuteButtonIcon();
}

void PlayControlWidget::updatePlayPosition(const qint64 &position)
{
    ui->timePostLabel->setText(MiliSecondToString(position));
    if (ui->playPosSlider->isSliderDown()) {
        return;
    }
    ui->playPosSlider->setValue(position);
}

void PlayControlWidget::setPlayPosition()
{
    if (m_corePlayerState != QMediaPlayer::PlayingState) {
        return;
    }
    qDebug() << "setPlayPosition" << ui->playPosSlider->value();
    m_corePlayer->setPlayPosition(ui->playPosSlider->value());
}

void PlayControlWidget::InitIconFont() {
    QFont awesome6Font = QFont(QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/font/fa-regular-400.ttf")).at(0));;
    ui->coverLabel->setFont(awesome6Font);
    ui->coverLabel->setText(ICON_COVER);
    ui->preButton->setFont(awesome6Font);
    ui->preButton->setText(ICON_PRE);
    ui->playButton->setFont(awesome6Font);
    ui->playButton->setText(ICON_PLAY);
    ui->nextButton->setFont(awesome6Font);
    ui->nextButton->setText(ICON_NEXT);
    ui->stopButton->setFont(awesome6Font);
    ui->stopButton->setText(ICON_STOP);
    ui->playModeButton->setFont(awesome6Font);
    updatePlayModeButtonIcon();
    ui->muteButton->setFont(awesome6Font);
    updateMuteButtonIcon();
}

void PlayControlWidget::updateMuteButtonIcon() {
    if (m_volMute || m_vol == 0) {
        ui->muteButton->setText(ICON_VOL_MUTE);
    } else if (m_vol <= 30) {
        ui->muteButton->setText(ICON_VOL_LVL1);
    } else if (m_vol <= 70) {
        ui->muteButton->setText(ICON_VOL_LVL2);
    } else {
        ui->muteButton->setText(ICON_VOL_LVL3);
    }
}

void PlayControlWidget::updatePlayModeButtonIcon() {
    switch (m_playMode) {
        case PlayMode::ListRepeat:
            ui->playModeButton->setText(ICON_PLAY_MODE_LIST_REPEAT);
            break;
        case PlayMode::SingleRepeat:
            ui->playModeButton->setText(ICON_PLAY_MODE_SING_REPEAT);
            break;
        case PlayMode::Random:
            ui->playModeButton->setText(ICON_PLAY_MODE_LIST_RANDOM);
            break;
    }
}

void PlayControlWidget::updatePlayMode() {
    switch (m_playMode) {
        case PlayMode::ListRepeat:
            m_playMode = PlayMode::SingleRepeat;
            break;
        case PlayMode::SingleRepeat:
            m_playMode = PlayMode::Random;
            break;
        case PlayMode::Random:
            m_playMode = PlayMode::ListRepeat;
            break;
        default:
            return;
    }
    updatePlayModeButtonIcon();
}

void PlayControlWidget::setPlayDuration(const qint64 &duration) {
    ui->playPosSlider->setMaximum(duration);
    ui->timeTotalLabel->setText(MiliSecondToString(duration));
}

void PlayControlWidget::handleMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        switch (m_playMode) {
            case PlayMode::ListRepeat:
                emit playNext();
                break;
            case PlayMode::SingleRepeat:
                m_corePlayer->play();
                break;
            case PlayMode::Random:
                emit playRandom();
                break;
        }
    }
}

PlayControlWidget::PlayMode PlayControlWidget::playMode() const {
    return m_playMode;
}

void PlayControlWidget::InitCss(const QString &cssFilePath) {
    this->setStyleSheet(util::loadCssFromFile(cssFilePath));
}

void PlayControlWidget::updatePlayInfo(PlayContent *content) {
    if (content == nullptr) {
        qDebug() << "can not update null play info";
        return;
    }
    const QStringList nameInfo = content->contentName.split(" - ");
    if (!content->title.isEmpty()) {
        ui->titleButton->setText(content->title);
    } else if (nameInfo.length() == 2) {
        ui->titleButton->setText(QFileInfo(nameInfo[1]).baseName());
    } else {
        ui->titleButton->setText(QFileInfo(content->contentName).baseName());
    }
    if (!content->artist.isEmpty()) {
        ui->artistButton->setText(content->artist);
    } else if (nameInfo.length() == 2) {
        ui->artistButton->setText(nameInfo[0]);
    }
    if (!content->albumTitle.isEmpty()) {
        ui->albumButton->setText(content->albumTitle);
    } else {
        ui->albumButton->setText(content->contentPath);
    }
    if (!content->albumCover.isNull()) {
        ui->coverLabel->setPixmap(QPixmap::fromImage(content->albumCover));
    } else {
        ui->coverLabel->setText(ICON_COVER);
    }
}
