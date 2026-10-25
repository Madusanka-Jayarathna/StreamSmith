// Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
// Use of this source code is governed by the MIT license.

#include "libMain/public/MainApp.h"
#include "libUtils/public/Utils.h"

int
main(int argc, char* argv[]) {
    Utils::setupBundledPath(argc > 0 ? argv[0] : nullptr);
    MainApp app;
    app.run();
    return 0;
}