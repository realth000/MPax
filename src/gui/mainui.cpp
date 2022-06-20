#include "mainui.h"
#include "./ui_mainui.h"

#include <QtCore/QtDebug>

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainUI)
{
    ui->setupUi(this);
    if (ui->playControlWidget == nullptr) {
        qDebug() << "empy";
    }
    this->setMinimumSize(800, 600);
}

MainUI::~MainUI()
{
    delete ui;
}
