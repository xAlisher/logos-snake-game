#include "HighScorePlugin.h"
#include <algorithm>

HighScorePlugin::HighScorePlugin(QObject* parent)
    : QObject(parent)
{
}

void HighScorePlugin::initLogos(LogosAPI* api)
{
    logosAPI = api;
}

QString HighScorePlugin::initialize()
{
    QJsonObject result;
    result["initialized"] = true;
    return QJsonDocument(result).toJson(QJsonDocument::Compact);
}

QString HighScorePlugin::submitScore(const QString& playerName, int score)
{
    m_scores.append({playerName, score});

    std::sort(m_scores.begin(), m_scores.end(),
        [](const Entry& a, const Entry& b) { return a.score > b.score; });

    if (m_scores.size() > MAX_ENTRIES)
        m_scores.resize(MAX_ENTRIES);

    int rank = -1;
    for (int i = 0; i < m_scores.size(); i++) {
        if (m_scores[i].name == playerName && m_scores[i].score == score) {
            rank = i + 1;
            break;
        }
    }

    QJsonObject result;
    result["submitted"] = true;
    result["rank"] = rank;
    result["total"] = m_scores.size();
    return QJsonDocument(result).toJson(QJsonDocument::Compact);
}

QString HighScorePlugin::getHighScores()
{
    QJsonArray arr;
    for (const auto& e : m_scores) {
        QJsonObject obj;
        obj["name"] = e.name;
        obj["score"] = e.score;
        arr.append(obj);
    }

    QJsonObject result;
    result["scores"] = arr;
    return QJsonDocument(result).toJson(QJsonDocument::Compact);
}

QString HighScorePlugin::clearScores()
{
    m_scores.clear();
    QJsonObject result;
    result["cleared"] = true;
    return QJsonDocument(result).toJson(QJsonDocument::Compact);
}

QString HighScorePlugin::getPlayerBest(const QString& playerName)
{
    int best = -1;
    for (const auto& e : m_scores) {
        if (e.name == playerName && e.score > best)
            best = e.score;
    }

    QJsonObject result;
    result["name"] = playerName;
    result["found"] = (best >= 0);
    if (best >= 0)
        result["score"] = best;
    return QJsonDocument(result).toJson(QJsonDocument::Compact);
}
