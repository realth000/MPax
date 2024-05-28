#include "appconfig.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtCore/QtDebug>

#include "util/util.h"

#define TYPE_STRING_LIST "QStringList"
#define TYPE_STRING "QString"
#define TYPE_INT "int"
#define TYPE_BOOL "bool"
#define TYPE_MAP "QMap"
#define TYPE_MAP_STRING_INT "QMap<QString, int>"
#define TYPE_MAP_STRING_BOOL "QMap<QString, bool>"

#define COMPARE_ZERO(VALUE, TYPE, IS)                       \
  if (TYPE == TYPE_STRING_LIST) {                           \
    IS = VALUE.toStringList().length() == 0 ? true : false; \
  } else if (TYPE == TYPE_STRING) {                         \
    IS = VALUE.toString().isEmpty() ? true : false;         \
  } else if (TYPE == TYPE_INT) {                            \
    IS = VALUE.toInt() == 0 ? true : false;                 \
  } else if (TYPE == TYPE_BOOL) {                           \
    IS = !VALUE.toBool();                                   \
  } else if (TYPE.left(4) == TYPE_MAP) {                    \
    IS = VALUE.toMap().count() == 0 ? true : false;         \
  } else {                                                  \
    IS = true;                                              \
  }

#define LOAD_CONFIG(CONFIG, CONFIG_NAME)                  \
  const auto &v = CONFIG.value("/" + CONFIG_NAME);        \
  auto isZero = true;                                     \
  COMPARE_ZERO(v, m_configMap[CONFIG_NAME].type, isZero); \
  if (!isZero) {                                          \
    m_configMap[CONFIG_NAME].value = v;                   \
  }

#define SAVE_CONFIG(CONFIG, CONFIG_NAME) CONFIG.setValue("/" + CONFIG_NAME, m_configMap[CONFIG_NAME].value)

Config::AppConfig *Config::AppConfig::getInstance() {
  static AppConfig ac;
  return &ac;
}

Config::ConfigPairMap Config::AppConfig::config() const { return m_configMap; }

void Config::AppConfig::setConfig(const QString &name, const QVariant &value) {
  if (!m_configMap.contains(name)) {
    qDebug() << "set config failed, config not exists:" << name;
    return;
  }
  m_configMap[name].value = value;
  saveConfigDefer();
}

void Config::AppConfig::printConfig() {
  qDebug() << "AppConfig:";
  for (const auto &[key, value] : ContainerAdapt(m_configMap)) {
    qDebug() << key << value.value;
  }
}

void Config::AppConfig::loadConfig() {
  makeConfigDir();
  auto config = QSettings(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/MPax/mpax.conf",
                          QSettings::IniFormat);
  auto it = m_configMap.begin();
  while (it != m_configMap.end()) {
    LOAD_CONFIG(config, it.key());
    it++;
  }
}

Config::AppConfig::AppConfig() : m_configMap(QMap<QString, ConfigPair>()), m_saveConfigDeferTimer(new QTimer) {
  addConfig(CONFIG_PLAYLIST_HEADER, QMap<QString, QVariant>(), TYPE_MAP_STRING_INT);
  addConfig(CONFIG_PLAYLIST_HEADER_SORT, QMap<QString, QVariant>(), TYPE_MAP_STRING_INT);
  addConfig(CONFIG_PLAYLIST_HEADER_USED, QMap<QString, QVariant>(), TYPE_MAP_STRING_BOOL);
  addConfig(CONFIG_CUR_PLAYLIST, 0, TYPE_INT);
  addConfig(CONFIG_CUR_PLAYCONTENT, 0, TYPE_STRING);
  addConfig(CONFIG_PLAY_MODE, 0, TYPE_INT);
  addConfig(CONFIG_VOLUME, 50, TYPE_INT);
  addConfig(CONFIG_VOLUME_MUTE, false, TYPE_BOOL);
  addConfig(CONFIG_SHORTCUT_PLAY_PAUSE, "Ctrl+Meta+B", TYPE_STRING);
  addConfig(CONFIG_SHORTCUT_PLAY_PRE, "Ctrl+Meta+Left", TYPE_STRING);
  addConfig(CONFIG_SHORTCUT_PLAY_NEXT, "Ctrl+Meta+Right", TYPE_STRING);
  addConfig(CONFIG_PLAY_SEEK_STEP, 5000, TYPE_INT);
  // Auto save has a delay.
  m_saveConfigDeferTimer->setInterval(300);
  m_saveConfigDeferTimer->setSingleShot(true);
  connect(m_saveConfigDeferTimer, &QTimer::timeout, this, &AppConfig::saveConfig);
}

Config::AppConfig::~AppConfig() = default;

void Config::AppConfig::addConfig(const QString &name, const QVariant &value, const QString &type) {
  ConfigPair config;
  config.name = name;
  config.value = value;
  config.type = type;
  m_configMap.insert(config.name, config);
}

void Config::AppConfig::makeConfigDir() {
#ifdef Q_OS_LINUX
  const auto &configPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
  if (configPath.isEmpty()) {
    qDebug() << "config location not writable";
    return;
  }
  QDir configDir(configPath);
  if (!configDir.exists("MPax") && !configDir.mkdir("MPax")) {
    qDebug() << "failed to create config path" << configDir.path() + "/MPax";
    return;
  }
#endif
}

void Config::AppConfig::saveConfigSoon() {
  m_saveConfigDeferTimer->stop();
  makeConfigDir();
  saveConfig();
}

void Config::AppConfig::saveConfigDefer() { m_saveConfigDeferTimer->start(); }

Config::ConfigPair Config::AppConfig::config(const QString &configName) const { return m_configMap[configName]; }

void Config::AppConfig::saveConfig() {
  auto config = QSettings(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/MPax/mpax.conf",
                          QSettings::IniFormat);
  auto it = m_configMap.constBegin();
  while (it != m_configMap.constEnd()) {
    SAVE_CONFIG(config, it.key());
    it++;
  }
}
