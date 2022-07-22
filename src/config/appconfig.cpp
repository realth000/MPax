#include "appconfig.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QtDebug>

#define CONFIG_FILE_PATH QCoreApplication::applicationDirPath() + "/mpax.conf"
#define TYPE_STRING_LIST "QStringList"
#define TYPE_STRING "QString"
#define TYPE_INT "int"
#define TYPE_BOOL "bool"
#define TYPE_MAP "QMap"
#define TYPE_MAP_STRING_INT "QMap<QString, int>"

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
  const QVariant v = CONFIG->value("/" + CONFIG_NAME);    \
  bool isZero = true;                                     \
  COMPARE_ZERO(v, m_configMap[CONFIG_NAME].type, isZero); \
  if (!isZero) {                                          \
    m_configMap[CONFIG_NAME].value = v;                   \
  }

#define SAVE_CONFIG(CONFIG, CONFIG_NAME) \
  CONFIG->setValue("/" + CONFIG_NAME, m_configMap[CONFIG_NAME].value)

Config::AppConfig* Config::AppConfig::getInstance() {
  static AppConfig ac;
  return &ac;
}

const Config::ConfigPairMap Config::AppConfig::config() const {
  return m_configMap;
}

void Config::AppConfig::setConfig(const QString& name, const QVariant& value) {
  if (!m_configMap.contains(name)) {
    qDebug() << "set config failed, config not exists:" << name;
    return;
  }
  m_configMap[name].value = value;
}

void Config::AppConfig::printConfig() {
  qDebug() << "AppConfig:";
  ConfigPairMap::const_iterator it = m_configMap.constBegin();
  while (it != m_configMap.constEnd()) {
    qDebug() << it.key() << it.value().value;
    it++;
  }
}

void Config::AppConfig::loadConfig() { loadConfig(CONFIG_FILE_PATH); }

Config::AppConfig::AppConfig()
    : m_configMap(QMap<QString, ConfigPair>()),
      m_saveConfigDeferTimer(new QTimer) {
  addConfig(CONFIG_PLAYLIST_HEADER, QMap<QString, QVariant>(),
            TYPE_MAP_STRING_INT);
  addConfig(CONFIG_PLAYLIST_SORT_HEADER, "Title", TYPE_STRING);
  addConfig(CONFIG_PLAYLIST_SORT_ORDER, 0, TYPE_INT);
  addConfig(CONFIG_CUR_PLAYLIST, 0, TYPE_INT);
  addConfig(CONFIG_CUR_PLAYCONTENT, 0, TYPE_INT);
  addConfig(CONFIG_PLAY_MODE, 0, TYPE_INT);
  addConfig(CONFIG_VOLUME, 50, TYPE_INT);
  addConfig(CONFIG_VOLUME_MUTE, false, TYPE_BOOL);
  addConfig(CONFIG_SHORTCUT_PLAY_PAUSE, "Ctrl+Meta+B", TYPE_STRING);
  addConfig(CONFIG_SHORTCUT_PLAY_PRE, "Ctrl+Meta+Left", TYPE_STRING);
  addConfig(CONFIG_SHORTCUT_PLAY_NEXT, "Ctrl+Meta+Right", TYPE_STRING);
  // Auto save has a delay.
  m_saveConfigDeferTimer->setInterval(300);
  m_saveConfigDeferTimer->setSingleShot(true);
  connect(m_saveConfigDeferTimer, &QTimer::timeout, this,
          &AppConfig::saveConfig);
}

Config::AppConfig::~AppConfig() {}

void Config::AppConfig::addConfig(const QString& name, const QVariant& value,
                                  const QString& type) {
  ConfigPair config;
  config.name = name;
  config.value = value;
  config.type = type;
  m_configMap.insert(config.name, config);
}

void Config::AppConfig::loadConfig(const QString& filePath) {
  QSettings* config =
      new QSettings(QDir::toNativeSeparators(filePath), QSettings::IniFormat);
  if (config == nullptr) {
    qDebug() << "can not load config";
    return;
  }
  ConfigPairMap::iterator it = m_configMap.begin();
  while (it != m_configMap.end()) {
    LOAD_CONFIG(config, it.key());
    it++;
  }
  delete config;
}

void Config::AppConfig::saveConfigSoon() {
  m_saveConfigDeferTimer->stop();
  saveConfig();
}

void Config::AppConfig::saveConfigDefer() { m_saveConfigDeferTimer->start(); }

const Config::ConfigPair Config::AppConfig::config(
    const QString& configName) const {
  return m_configMap[configName];
}

void Config::AppConfig::saveConfig() {
  QSettings* config = new QSettings(QDir::toNativeSeparators(CONFIG_FILE_PATH),
                                    QSettings::IniFormat);
  if (config == nullptr) {
    qDebug() << "can not save config";
    return;
  }
  ConfigPairMap::const_iterator it = m_configMap.constBegin();
  while (it != m_configMap.constEnd()) {
    SAVE_CONFIG(config, it.key());
    it++;
  }
  delete config;
  saveConfigDefer();
}
