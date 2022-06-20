#include "mainui.h"
#include "./ui_mainui.h"

#include <QtCore/QtDebug>

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI),
      m_corePlayer(new CorePlayer)
{
    ui->setupUi(this);
    InitConnections();
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::InitConnections()
{
    connect(ui->testButton, &QPushButton::clicked, this, &MainUI::Play);
}

void MainUI::Play()
{
    m_currentContentPath.setScheme("file");
    m_currentContentPath.setPath("/home/th000/Desktop/QtProjects/MPax/Aoibridge.mp3");
    m_corePlayer->Play(m_currentContentPath);
}

