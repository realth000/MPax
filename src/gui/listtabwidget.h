#ifndef LISTTABWIDGET_H
#define LISTTABWIDGET_H

#include <QtWidgets/QWidget>

namespace Ui {
class ListTabWidget;
}

class ListTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ListTabWidget(QWidget *parent = nullptr);
    ~ListTabWidget();

private:
    Ui::ListTabWidget *ui;
};

#endif // LISTTABWIDGET_H
