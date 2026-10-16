import customtkinter as ctk
import os
#from .YtDlpView import YtDlpAdvancedApp

class MainView(ctk.CTkFrame):
    def __init__(self, parent, onNavigate):
        super().__init__(parent)
        self._onNavigate = onNavigate
        self._createWidgets()

        #self.title("Advance Media Downloader & Converter")
        #self.geometry("740x630")
        #self.resizable(False, False)

        #self._vidFormats = {}
        #self._audFormats = {}
        #self._downlaodDir = os.path.expanduser("~/Donwloads")

        #self._frame = None
        #self._showView("main")
        #self._ytDlpApp = None
        #self._createWidgets()


    def _createWidgets(self):
       self._titleLabel = ctk.CTkLabel(
           self,
           text="Video",
           font=ctk.CTkFont(size=22, weight="bold"),
       )
       self._titleLabel.pack(padx=20, pady=(20, 10))
    
       self._openBtn = ctk.CTkButton(
           self,
           text="Open Next Window",
           command=lambda: self._onNavigate("downloder"),
       )
       self._openBtn.pack(padx=20, pady=10)
        

    

    #def _openNextWindow(self):
    #    # Prevent opening multiple instances of the second window
    #    if self._ytDlpApp is None or not self._ytDlpApp.winfo_exists():
    #        self._ytDlpApp = YtDlpAdvancedApp(self)
    #    else:
    #        self._ytDlpApp.focus()