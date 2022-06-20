#ifndef MAINUI_H
#define MAINUI_H

#include <QtWidgets/QMainWindow>

#include "core/coreplayer.h"

QT_BEGIN_NAMESPACE
    namespace Ui { class MainUI; }
QT_END_NAMESPACE
class MainUI : public QMainWindow
{
    Q_OBJECT

public:
    MainUI(QWidget *parent = nullptr);
    ~MainUI();

private:
    Ui::MainUI *ui;
    CorePlayer *m_corePlayer;
    QUrl m_currentContentPath;

    void InitConnections();
    void Play();
};
#endif // MAINUI_H
