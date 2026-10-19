#pragma once

#include <QObject>
#include <QWidget>

class MainView : public QWidget {
    Q_OBJECT

public:
    explicit MainView(QWidget* parent = nullptr);

signals:
    void downloadRequested();

private:
    void setupUI();
};