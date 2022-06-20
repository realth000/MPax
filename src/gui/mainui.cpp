#include "mainui.h"
#include "./ui_mainui.h"

#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtDebug>

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI)
{
    ui->setupUi(this);

    // test
    connect(ui->testButton, &QPushButton::clicked, this, &MainUI::Play);
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::Play()
{
    QMediaPlayer *p = new QMediaPlayer;
    QMediaPlaylist *l = new QMediaPlaylist;
    l->addMedia(QMediaContent(QUrl("c:/QtProjects/MPax/Aoibridge.mp3")));
    l->setCurrentIndex(0);
    p->setPlaylist(l);
    p->play();
    qDebug() << "finish";
}

