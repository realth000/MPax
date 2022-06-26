#include "appconfig.h"

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QtDebug>

#define CONFIG_FILE_PATH "./mpax.conf"
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

Config::AppConfig::AppConfig() : m_configMap(QMap<QString, ConfigPair>()) {
  addConfig(CONFIG_ALL_PLAYLIST, QStringList{}, TYPE_STRING_LIST);
  addConfig(CONFIG_PLAYLIST_HEADER, QMap<QString, QVariant>(),
            TYPE_MAP_STRING_INT);
  addConfig(CONFIG_CUR_PLAYLIST, 0, TYPE_INT);
  addConfig(CONFIG_CUR_PLAYCONTENT, 0, TYPE_INT);
  addConfig(CONFIG_PLAY_MODE, 0, TYPE_INT);
  addConfig(CONFIG_VOLUME, 50, TYPE_INT);
  addConfig(CONFIG_VOLUME_MUTE, false, TYPE_BOOL);
}

Config::AppConfig::~AppConfig() {}

void Config::AppConfig::addConfig(const QString& name, const QVariant& value,
                                  const QString& type) {
  ConfigPair config = {.name = name, .value = value, .type = type};
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

const Config::ConfigPair Config::AppConfig::config(
    const QString& configName) const {
  return m_configMap[configName];
}
