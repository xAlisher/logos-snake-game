#pragma once
#include <QObject>
#include <QVariantList>

class LogosAPI;

class SnakeGameBackend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
public:
    explicit SnakeGameBackend(LogosAPI* api, QObject* parent = nullptr);
    QVariantList items() const;

    Q_INVOKABLE void addItem(const QString& name);
    Q_INVOKABLE void removeItem(int index);

signals:
    void itemsChanged();

private:
    LogosAPI* m_api;
    QVariantList m_items;
};
