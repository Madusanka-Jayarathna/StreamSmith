#include "libMain/public/MainApp.h"
#include "libDownloader/public/DownloaderApp.h"
#include "libConverter/public/ConverterApp.h"
#include "libUtils/public/Utils.h"

void
MainApp::run(){
    showProductTitle();
    int product = selectedProduct();

    if(product == 1) {
        DownloaderApp downloader;
        downloader.run();
    } else if (product == 2) {
        ConverterApp converter;
        converter.run();
    }
    
    return;
}

void
MainApp::showProductTitle(){
    std::cout << "\n";
    std::cout << "--------------------------------------------------" << "\n";
    std::cout << "         Stream Downloader & Converter            " << "\n";    
    std::cout << "--------------------------------------------------" << "\n";
}

int
MainApp::selectedProduct(){
    std::cout << "              1    - Downloader\n";
    std::cout << "              2    - Converter\n";
    std::cout << "              0    - Back\n";
    std::cout << "             -1    - Exit\n\n";
    
    const std::string msg = "Select Product: ";
    int product = -1;
    bool success = Utils::isNumeric(Utils::getUserInput(msg), product);
    bool valid = false;
    while(!valid) {
        if(!success || (success && product > 2)) {
            std::cout << "ERROR     : Invalid product Selected. Please re-enter\n\n";
            success = Utils::isNumeric(Utils::getUserInput(msg), product);
            continue;
        }
        valid = true;
    }
    
    return product;
}

/*
// MainApp.cpp
#include "libMain/public/MainApp.h"
#include "libMainView/public/MainView.h"
#include "libDownloader/public/DownloaderView.h"
#include <QApplication>
#include <QStackedWidget>

MainApp::MainApp(QWidget* parent) : QMainWindow(parent) 
{
    setWindowTitle("Advanced Media Downloader & Converter");
    resize(740, 630);

    m_stackedWidget = new QStackedWidget(this);

    // 1. Persistent Main View allocation
    m_mainView = new MainView(this);
    m_stackedWidget->addWidget(m_mainView);

    // Listen for navigation request from MainView
    connect(m_mainView, &MainView::downloadRequested, this, &MainApp::showDownloaderView);

    setCentralWidget(m_stackedWidget);
}

void MainApp::showDownloaderView() {
    // 2. LAZY CREATION: Instantiate DownloaderView only when requested
    if (!m_downloaderView) {
        m_downloaderView = new DownloaderView(this);
        m_stackedWidget->addWidget(m_downloaderView);

        connect(m_downloaderView, &DownloaderView::backRequested, this, &MainApp::showMainView);
    }

    m_stackedWidget->setCurrentWidget(m_downloaderView);
}

void MainApp::showMainView() {
    // Switch back to MainView
    m_stackedWidget->setCurrentWidget(m_mainView);

    // 3. LAZY DESTRUCTION: Remove and destroy DownloaderView to free RAM
    if (m_downloaderView) {
        m_stackedWidget->removeWidget(m_downloaderView);
        m_downloaderView->deleteLater(); 
        m_downloaderView = nullptr; // Reset raw pointer
    }
}
*/