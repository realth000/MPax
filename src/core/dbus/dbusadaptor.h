#pragma once

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

#define MPAX_DBUS_SERVICE_NAME "kzs.th000.MPax"

class DbusAdaptor : public QDBusAbstractAdaptor {
  Q_OBJECT

  Q_CLASSINFO("D-Bus Interface", MPAX_DBUS_SERVICE_NAME)

 public:
  DbusAdaptor(QObject* obj) : QDBusAbstractAdaptor(obj) {}

 public slots:
  QDBusVariant playPrevious();
  QDBusVariant playNext();
  QDBusVariant pause();
};
