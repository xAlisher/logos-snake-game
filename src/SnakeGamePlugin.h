#pragma once
#include <QObject>
#include <QWidget>
#include <QtPlugin>
#include "IComponent.h"

class SnakeGamePlugin : public QObject, public IComponent {
    Q_OBJECT
    Q_INTERFACES(IComponent)
    Q_PLUGIN_METADATA(IID IComponent_iid FILE "metadata.json")
public:
    QWidget* createWidget(LogosAPI* logosAPI = nullptr) override;
    void destroyWidget(QWidget* widget) override;
};
