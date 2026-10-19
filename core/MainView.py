import customtkinter as ctk
import os

class MainView(ctk.CTkFrame):
    def __init__(self, parent, onNavigate):
        super().__init__(parent)
        self._onNavigate = onNavigate
        self._createWidgets()

    def _createWidgets(self):
        # Title Frame
        self._titleFrame = ctk.CTkFrame(self)
        self._titleFrame.pack(padx=20, pady=10, fill="x")

        # Header Title
        self._titleLabel = ctk.CTkLabel(
            self._titleFrame,
            text="Advance Media Downloader & Converter", 
            font=ctk.CTkFont(size=22, weight="bold"),
        )
        self._titleLabel.pack(side="left", expand=True, padx=10, pady=10)

        # Button Frame
        self._buttonFrame = ctk.CTkFrame(self)
        self._buttonFrame.pack(padx=20, pady=10, fill="x")
        # Downloader Button
        self._donwloaderBtn = ctk.CTkButton( 
            self._buttonFrame,
            text="Downloader",
            font=ctk.CTkFont(size=16, weight="bold"),
            height=100,
            width=200,
            command=lambda: self._onNavigate("downloder")
        )
        self._donwloaderBtn.pack(side="left", expand=True, padx=20, pady=(200))
        # Converter Button
        self._converterBtn = ctk.CTkButton( 
            self._buttonFrame,
            text="Converter",
            font=ctk.CTkFont(size=16, weight="bold"),
            height=100,
            width=200
            #command=lambda: self._onNavigate("downloder")
        )
        self._converterBtn.pack(side="left", expand=True, padx=20, pady=(20))