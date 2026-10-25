// Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
// Use of this source code is governed by the MIT license.

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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