#include "SnakeGamePlugin.h"
#include <QJsonDocument>
#include <QJsonObject>

SnakeGamePlugin::SnakeGamePlugin(QObject* parent)
    : QObject(parent)
{
}

void SnakeGamePlugin::initLogos(LogosAPI* api)
{
    logosAPI = api;
}

QString SnakeGamePlugin::initialize()
{
    QJsonObject result;
    result["initialized"] = true;
    return QJsonDocument(result).toJson(QJsonDocument::Compact);
}

QString SnakeGamePlugin::getState()
{
    QJsonObject result;
    result["state"] = "ready";
    return QJsonDocument(result).toJson(QJsonDocument::Compact);
}
