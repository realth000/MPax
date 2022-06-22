#ifndef LISTTABWIDGET_H
#define LISTTABWIDGET_H

#include <QtWidgets/QWidget>

#include "core/listtabmodel.h"

namespace Ui {
class ListTabWidget;
}

class ListTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ListTabWidget(QWidget *parent = nullptr);
    ~ListTabWidget();

public slots:
    void setModel(ListTabModel *listTabModel);

private:
    Ui::ListTabWidget *ui;
};

#endif // LISTTABWIDGET_H
