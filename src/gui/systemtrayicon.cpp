#include "systemtrayicon.h"

#include <QtCore/QDebug>
#include <QtGui/QIcon>

SystemTrayIcon::SystemTrayIcon(QWidget *parent)
    : QSystemTrayIcon(parent),
      m_contextMenu(new QMenu()),
      m_isMainWindowMinimized(false) {
  initContextMenu();
  this->setContextMenu(m_contextMenu);
  this->setIcon(QIcon(":/pic/logo/MPax@4x.png"));
  connect(this, &SystemTrayIcon::activated, this,
          &SystemTrayIcon::onIconActivated);
}

void SystemTrayIcon::initContextMenu() {
  QAction *showWindowAction =
      new QAction(tr("Show/Hide main window"), m_contextMenu);
  QAction *exitAction = new QAction(tr("Exit app"), m_contextMenu);
  QAction *restartAction = new QAction(tr("Restart app"), m_contextMenu);
  m_contextMenu->addAction(showWindowAction);
  m_contextMenu->addSeparator();
  m_contextMenu->addAction(exitAction);
  m_contextMenu->addAction(restartAction);
  connect(showWindowAction, &QAction::triggered, this,
          &SystemTrayIcon::showMainWindow);
  connect(exitAction, &QAction::triggered, this,
          &SystemTrayIcon::appExitRequested);
  connect(restartAction, &QAction::triggered, this,
          &SystemTrayIcon::appRestartRequested);
  connect(m_contextMenu, &QMenu::aboutToShow, this, [this, showWindowAction]() {
    !m_isMainWindowMinimized
    ? showWindowAction->setText(tr("Hide main window"))
    : showWindowAction->setText(tr("Show main window"));
  });
}

void SystemTrayIcon::showMainWindow(bool show) {
  emit mainWindowVisibleRequested(show);
}

void SystemTrayIcon::updateMainWindowActiveState(bool active) {
  m_isMainWindowMinimized = active;
}
void SystemTrayIcon::onIconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case QSystemTrayIcon::Trigger:showMainWindow(!m_isMainWindowMinimized);
      break;
    default:break;
  }
}
