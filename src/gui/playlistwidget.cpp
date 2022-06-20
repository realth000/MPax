#include "playlistwidget.h"
#include "ui_playlistwidget.h"

PlaylistWidget::PlaylistWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistWidget)
{
    ui->setupUi(this);
}

PlaylistWidget::~PlaylistWidget()
{
    delete ui;
}
