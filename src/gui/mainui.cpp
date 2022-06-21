#include "mainui.h"
#include "./ui_mainui.h"

#include <QtCore/QtDebug>

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI),
      m_playlistModel(new PlaylistModel(PlaylistModelHeader::defaultHeaderList()))
{
    ui->setupUi(this);
    if (ui->playControlWidget == nullptr) {
        qDebug() << "empy";
    }
    this->setMinimumSize(800, 600);

    // Bind playlist view and model.

    // This one not works.
    // ui->playlistWidget->setModel(m_playlistModel);
    InitConnections();
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::InitConnections()
{
    connect(ui->openAudioAction, &QAction::triggered, this, &MainUI::openAudio);
    connect(ui->playControlWidget, &PlayControlWidget::contentChanged, this, &MainUI::checkIncommingContent);
}

void MainUI::openAudio()
{
    ui->playControlWidget->setContentPath("/home/th000/Desktop/QtProjects/MPax/Aoibridge.mp3");

}

void MainUI::checkIncommingContent(const PlayContent *content)
{
    qDebug() << "add content11111111";
    // TODO: Some check if content already in current playlist.
    m_playlistModel->addContent(*content);
    ui->playlistWidget->setModel(m_playlistModel);
}
