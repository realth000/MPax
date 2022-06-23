#include "listtabwidget.h"
#include "ui_listtabwidget.h"

ListTabWidget::ListTabWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::ListTabWidget),
    m_currentListTabModel(nullptr)
{
    ui->setupUi(this);
}

ListTabWidget::~ListTabWidget()
{
    delete ui;
}

void ListTabWidget::setModel(ListTabModel *listTabModel)
{
    m_currentListTabModel = listTabModel;
    ui->listView->setModel(m_currentListTabModel);
}

void ListTabWidget::addPlaylist(const QString &playlistName, PlaylistModel *playlistModel) {
    m_currentListTabModel->addPlaylist(playlistModel);
}
