#include "SnakeGameBackend.h"

SnakeGameBackend::SnakeGameBackend(LogosAPI* api, QObject* parent)
    : QObject(parent), m_api(api) {}

QVariantList SnakeGameBackend::items() const { return m_items; }

void SnakeGameBackend::addItem(const QString& name) {
    QVariantMap item;
    item["name"] = name;
    m_items.append(item);
    emit itemsChanged();
}

void SnakeGameBackend::removeItem(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_items.removeAt(index);
        emit itemsChanged();
    }
}
