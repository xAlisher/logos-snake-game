#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <module_lib/interface.h>

class HighScorePlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.logos.HighScoreInterface" FILE "high_score_metadata.json")
    Q_INTERFACES(PluginInterface)

public:
    explicit HighScorePlugin(QObject* parent = nullptr);

    QString name()    const override { return QStringLiteral("high_score"); }
    QString version() const override { return QStringLiteral("1.0.0"); }

    Q_INVOKABLE void    initLogos(LogosAPI* api);
    Q_INVOKABLE QString initialize();
    Q_INVOKABLE QString submitScore(const QString& playerName, int score);
    Q_INVOKABLE QString getHighScores();
    Q_INVOKABLE QString clearScores();
    Q_INVOKABLE QString getPlayerBest(const QString& playerName);

signals:
    void eventResponse(const QString& eventName, const QVariantList& data);

private:
    struct Entry {
        QString name;
        int score;
    };

    QList<Entry> m_scores;
    static const int MAX_ENTRIES = 10;
};
