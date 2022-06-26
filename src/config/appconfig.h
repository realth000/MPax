#ifndef MPAX_APPCONFIG_H
#define MPAX_APPCONFIG_H

#include <QtCore/QPair>
#include <QtCore/QVariant>

#define CONFIG_ALL_PLAYLIST "AllPlaylist"
#define CONFIG_PLAYLIST_HEADER "PlaylistHeader"
#define CONFIG_CUR_PLAYLIST "CurrentPlaylist"
#define CONFIG_CUR_PLAYCONTENT "CurrentPlayContent"
#define CONFIG_PLAY_MODE "PlayMode"
#define CONFIG_VOLUME "Volume"
#define CONFIG_VOLUME_MUTE "VolumeMute"

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
#endif  // MPAX_APPCONFIG_H
