#ifndef MPAX_SYSTEMTRAYICON_H
#define MPAX_SYSTEMTRAYICON_H

#include <QtWidgets/QMenu>
#include <QtWidgets/QSystemTrayIcon>

class SystemTrayIcon : public QSystemTrayIcon {
 Q_OBJECT

 public:
  explicit SystemTrayIcon(QWidget *parent = nullptr);

 signals:
  void mainWindowVisibleRequested(bool show);
  void appExitRequested();
  void appRestartRequested();

 public slots:
  void updateMainWindowActiveState(bool active);

 private:
  QMenu *m_contextMenu;
  bool m_isMainWindowMinimized;
  void initContextMenu();

 private slots:
  void showMainWindow(bool isShowing);
  void onIconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif  // MPAX_SYSTEMTRAYICON_H
