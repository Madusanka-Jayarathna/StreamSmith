# Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
# Use of this source code is governed by the MIT license.

import os
import sys
import customtkinter as ctk

if __package__ in (None, ""):
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from core.YtDlpView import YtDlpView 
from core.MainView import MainView


class MainApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Advance Media Downloader & Converter")
        self.geometry("740x630")
        self.resizable(False, False)

        self._frame = None
        self._showView("main")

    def _showView(self, viewName):
        if self._frame is not None:
            self._frame.destroy()

        if viewName == "main":
            self._frame = MainView(self, self._showView)
        elif viewName == "downloder":
            self._frame = YtDlpView(self, self._showView)

        self._frame.pack(fill="both", expand=True)



if __name__ == "__main__":
    app = MainApp()
    app.mainloop()