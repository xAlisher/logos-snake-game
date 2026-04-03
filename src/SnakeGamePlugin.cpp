#include "SnakeGamePlugin.h"
#include "SnakeGameBackend.h"
#include <QQuickWidget>
#include <QQmlContext>

QWidget* SnakeGamePlugin::createWidget(LogosAPI* logosAPI) {
    auto* widget = new QQuickWidget;
    auto* backend = new SnakeGameBackend(logosAPI, widget);
    widget->rootContext()->setContextProperty("backend", backend);
    widget->setSource(QUrl("qrc:/qml/Main.qml"));
    widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    return widget;
}

void SnakeGamePlugin::destroyWidget(QWidget* widget) {
    delete widget;
}
