#include "appconfig.h"

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QtDebug>

#define CONFIG_FILE_PATH "./mpax.conf"
#define TYPE_STRING_LIST "QStringList"
#define TYPE_STRING "QString"
#define TYPE_INT "int"
#define TYPE_BOOL "bool"

#define COMPARE_ZERO(VALUE, TYPE, IS)                       \
  if (TYPE == "QStringList") {                              \
    IS = VALUE.toStringList().length() == 0 ? true : false; \
  } else if (TYPE == "QString") {                           \
    IS = VALUE.toString().isEmpty() ? true : false;         \
  } else if (TYPE == "int") {                               \
    IS = VALUE.toInt() == 0 ? true : false;                 \
  } else if (TYPE == "bool") {                              \
    IS = !VALUE.toBool();                                   \
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
  ConfigPairMap::const_iterator it = m_configMap.cbegin();
  while (it != m_configMap.end()) {
    qDebug() << it.key() << it.value().value;
    it++;
  }
}

void Config::AppConfig::loadConfig() { loadConfig(CONFIG_FILE_PATH); }

Config::AppConfig::AppConfig() : m_configMap(QMap<QString, ConfigPair>()) {
  addConfig(CONFIG_ALL_PLAYLIST, QStringList{}, TYPE_STRING_LIST);
  addConfig(CONFIG_CUR_PLAYLIST, "", TYPE_STRING);
  addConfig(CONFIG_CUR_PLAYCONTENT, "", TYPE_STRING);
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
