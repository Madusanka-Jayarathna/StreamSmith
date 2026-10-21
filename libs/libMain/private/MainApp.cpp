#include "libMain/public/MainApp.h"
#include "libDownloader/public/DownloaderApp.h"
#include "libUtils/public/Utils.h"

void
MainApp::run(){
    Products product = selectedProduct();
    if(product == MainApp::Products::DOWNLOADER) {
        DownloaderApp downloader;
        downloader.run();
    }

    
}

MainApp::Products
MainApp::selectedProduct(){
    std::string rawInput;
    int validProduct = 0;
    bool isValid = false;

    while(!isValid) {
        showProductSelector();
        std::getline(std::cin, rawInput);
        try{
            size_t chars;
            validProduct = std::stoi(rawInput, &chars);

            if(chars < rawInput.size() || validProduct > 2 || validProduct < 0){
                throw std::invalid_argument("ERROR      : Invalid Product selected\n");
            } else {
                isValid = true;
            }
        }
        catch (const std::invalid_argument& e) {
            std::cout <<  e.what() << "\n";
        }
        catch (const std::out_of_range& e) {
            std::cout << "ERROR      : Invalid Product selected\n\n";
        }
    }
    
    if(validProduct == 1) {
        return MainApp::Products::DOWNLOADER;
    } else if (validProduct == 2) {
        return MainApp::Products::CONVERTER;
    }
    return MainApp::Products::EXIT;

}

void
MainApp::showProductSelector(){
    std::cout << "1 - Downloader\n";
    std::cout << "2 - Converter\n";
    std::cout << "0 - Exit\n";

    std::cout << "Select Product : \t";
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