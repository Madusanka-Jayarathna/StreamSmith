import customtkinter as ctk
import os

#class YtDlpView(ctk.CTkToplevel):
class YtDlpView(ctk.CTkFrame):
    def __init__(self, parent, onNavigate):
        super().__init__(parent)
        self._onNavigate = onNavigate

        #self.title("Second Window")
        #self.geometry("400x300")

        self._label = ctk.CTkLabel(
            self,
            text="Second Window",
            font=ctk.CTkFont(size=18, weight="bold"),
        )
        self._label.pack(padx=20, pady=20)

        self._backBtn = ctk.CTkButton(
            self,
            text="Back to Main",
            command=lambda: self._onNavigate("main"),
        )
        self._backBtn.pack(padx=20, pady=10)

    #def __init__(self):
    #    super().__init__()

        #self._title("Advance Media Downloader")
        #self._geometry("740x630")
        #self._resizable(False, False)

        #self._vidFormats = {}
        #self._audFormats = {}
        #self._downlaodDir = os.path.expanduser("~/Donwloads")


    #def
