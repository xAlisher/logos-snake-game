#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <module_lib/interface.h>

class SnakeGamePlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.logos.SnakeGameInterface" FILE "plugin_metadata.json")
    Q_INTERFACES(PluginInterface)

public:
    explicit SnakeGamePlugin(QObject* parent = nullptr);

    QString name()    const override { return QStringLiteral("snake_game"); }
    QString version() const override { return QStringLiteral("1.0.0"); }

    Q_INVOKABLE void    initLogos(LogosAPI* api);
    Q_INVOKABLE QString initialize();
    Q_INVOKABLE QString getState();

signals:
    void eventResponse(const QString& eventName, const QVariantList& data);
};
