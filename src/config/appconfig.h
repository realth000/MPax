#ifndef MPAX_APPCONFIG_H
#define MPAX_APPCONFIG_H

#include <QtCore/QPair>
#include <QtCore/QTimer>
#include <QtCore/QVariant>

//#define CONFIG_ALL_PLAYLIST "AllPlaylist"
#define CONFIG_PLAYLIST_HEADER "PlaylistHeader"
#define CONFIG_CUR_PLAYLIST "CurrentPlaylist"
#define CONFIG_CUR_PLAYCONTENT "CurrentPlayContent"
#define CONFIG_PLAY_MODE "PlayMode"
#define CONFIG_VOLUME "Volume"
#define CONFIG_VOLUME_MUTE "VolumeMute"
#define CONFIG_SHORTCUT_PLAY_PAUSE "ShortcutPlayPause"
#define CONFIG_SHORTCUT_PLAY_PRE "ShortcutPlayPre"
#define CONFIG_SHORTCUT_PLAY_NEXT "ShortcutPlayNext"

namespace Config {
struct ConfigPair {
  QString name;
  QVariant value;
  QString type;
};

typedef QMap<QString, ConfigPair> ConfigPairMap;

class AppConfig : public QObject {
  Q_OBJECT

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
  void saveConfigSoon();
  void saveConfigDefer();

 private:
  QMap<QString, ConfigPair> m_configMap;
  QTimer *m_saveConfigDeferTimer;

  AppConfig();
  ~AppConfig();
  void addConfig(const QString &name, const QVariant &value,
                 const QString &type);
  void loadConfig(const QString &filePath);

 private slots:
  void saveConfig();
};
}  // namespace Config
#endif  // MPAX_APPCONFIG_H
