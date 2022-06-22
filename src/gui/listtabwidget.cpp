#include "listtabwidget.h"
#include "ui_listtabwidget.h"

ListTabWidget::ListTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListTabWidget)
{
    ui->setupUi(this);
}

ListTabWidget::~ListTabWidget()
{
    delete ui;
}

void ListTabWidget::setModel(ListTabModel *listTabModel)
{
    ui->listView->setModel(listTabModel);
}
