#include "libMainView/public/MainView.h"

#include <QFont>
#include <QLabel>
#include <QVBoxLayout>

MainView::MainView(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Advanced Media Downloader & Converter");
    resize(740, 630);
    setFixedSize(740, 630);
    setupUI();
}

void
MainView::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);

    auto* titleLabel = new QLabel("Video / Audio Downloader & Converter", this);
    titleLabel->setFont(QFont("Arial", 16, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
}