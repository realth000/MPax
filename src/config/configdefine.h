#ifndef MPAX_CONFIGDEFINE_H
#define MPAX_CONFIGDEFINE_H

#include <QtCore/QPair>
#include <QtCore/QVariant>

namespace Config {
struct ConfigPair {
  QString name;
  QVariant value;
  QString type;
};

typedef QMap<QString, ConfigPair> ConfigPairMap;

class AppConfig {
 public:
  static AppConfig *getInstance();
  AppConfig(const AppConfig &) = delete;
  AppConfig &operator=(const AppConfig &) = delete;
  const ConfigPairMap config() const;
  const ConfigPair config(const QString &configName) const;
  void setConfig(const QString &name, const QVariant &value);
  void printConfig();

 public slots:
  void loadConfig();

 private:
  QMap<QString, ConfigPair> m_configMap;

  AppConfig();
  ~AppConfig();
  void addConfig(const QString &name, const QVariant &value,
                 const QString &type);
  void loadConfig(const QString &filePath);
};
}  // namespace Config
#endif  // MPAX_CONFIGDEFINE_H
