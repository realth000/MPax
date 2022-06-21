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

    ui->playlistWidget->setModel(m_playlistModel);
}

MainUI::~MainUI()
{
    delete ui;
}
