# Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
# Use of this source code is governed by the MIT license.

import os
import sys
import threading
import customtkinter as ctk
from tkinter import filedialog
import yt_dlp


def formatBytes(bytes_num):
  """Utility to convert raw bytes to human-readable size string."""
  if not bytes_num or bytes_num <= 0:
    return "Unknown size"
  for unit in ["B", "KB", "MB", "GB", "TB"]:
    if bytes_num < 1024.0:
      return f"{bytes_num:.2f} {unit}"
    bytes_num /= 1024.0
  return f"{bytes_num:.2f} PB"

class YtDlpView(ctk.CTkFrame):
    print("1")
    def __init__(self, parent, onNavigate):
      print("2")
      super().__init__(parent)
      self._onNavigate = onNavigate

      self._videoFormats = {}
      self._audioFormats = {}
      self._downloadDir = os.path.expanduser("~/Downloads")

      self._createWidgets()


    def _createWidgets(self):
      print("3")
      # Title Frame
      self._titleFrame = ctk.CTkFrame(self)
      self._titleFrame.pack(padx=20, pady=10, fill="x")

      # Back Button
      self._backBtn = ctk.CTkButton(
          self._titleFrame,
          text="←",
          width=35,  # Small button dimensions
          height=30,
          command=lambda: self._onNavigate("main"),
      )
      self._backBtn.pack(side="left", padx=10, pady=10)

      # Header Title
      self._titleLabel = ctk.CTkLabel(
          self._titleFrame,
          text="Video / Audio Downloader", 
          font=ctk.CTkFont(size=22, weight="bold"),
      )
      self._titleLabel.pack(side="left", expand=True, padx=10, pady=10)
      #self._titleLabel.pack(padx=20, pady=(20, 10))

      # URL Frame
      self._urlFrame = ctk.CTkFrame(self)
      self._urlFrame.pack(padx=20, pady=10, fill="x")

      self._urlEntry = ctk.CTkEntry(
          self._urlFrame,
          placeholder_text="Paste Video / Playlist URL here...",
          width=530,
      )
      self._urlEntry.pack(side="left", padx=(10, 5), pady=10)

      self._analyzeBtn = ctk.CTkButton(
          self._urlFrame,
          text="Analyze",
          width=120,
          command=self._startAnalyzeThread,
      )
      self._analyzeBtn.pack(side="right", padx=(5, 10), pady=10)

      # Mode Selector Frame
      self._modeFrame = ctk.CTkFrame(self)
      self._modeFrame.pack(padx=20, pady=10, fill="x")

      self._modeLabel = ctk.CTkLabel(
          self._modeFrame,
          text="Download Mode:",
          font=ctk.CTkFont(weight="bold"),
      )
      self._modeLabel.pack(side="left", padx=10, pady=10)

      self._modeOption = ctk.CTkOptionMenu(
          self._modeFrame,
          values=["Highest Quality", "Audio Only", "Video Only", "Custom"],
          command=self._onModeChange,
          width=200,
      )
      self._modeOption.pack(side="right", padx=10, pady=10)

      # Qualities Frame
      self._qualitiesFrame = ctk.CTkFrame(self)
      self._qualitiesFrame.pack(padx=20, pady=10, fill="x")

      # Video Quality Dropdown
      self._vLabel = ctk.CTkLabel(self._qualitiesFrame, text="Video Format:")
      self._vLabel.grid(row=0, column=0, padx=10, pady=10, sticky="w")

      self._videoMenu = ctk.CTkOptionMenu(
          self._qualitiesFrame,
          values=["Analyze URL first"],
          command=lambda _: self._updateEstimatedSize(),
          state="disabled",
          width=500,
      )
      self._videoMenu.grid(row=0, column=1, padx=10, pady=10)

      # Audio Quality Dropdown
      self._aLabel = ctk.CTkLabel(self._qualitiesFrame, text="Audio Format:")
      self._aLabel.grid(row=1, column=0, padx=10, pady=10, sticky="w")

      self._audioMenu = ctk.CTkOptionMenu(
          self._qualitiesFrame,
          values=["Analyze URL first"],
          command=lambda _: self._updateEstimatedSize(),
          state="disabled",
          width=500,
      )
      self._audioMenu.grid(row=1, column=1, padx=10, pady=10)

      # File Size Estimation Display
      self.size_frame = ctk.CTkFrame(self)
      self.size_frame.pack(padx=20, pady=5, fill="x")

      self._sizeTitleLabel = ctk.CTkLabel(
          self.size_frame,
          text="Approx. File Size:",
          font=ctk.CTkFont(weight="bold"),
      )
      self._sizeTitleLabel.pack(side="left", padx=10, pady=8)

      self._sizeValueLabel = ctk.CTkLabel(
          self.size_frame,
          text="--",
          font=ctk.CTkFont(size=14, weight="bold"),
          text_color="cyan",
      )
      self._sizeValueLabel.pack(side="left", padx=5, pady=8)

      # Path Selector Frame
      self._pathFrame = ctk.CTkFrame(self)
      self._pathFrame.pack(padx=20, pady=10, fill="x")

      self._pathEntry = ctk.CTkEntry(self._pathFrame, width=540)
      self._pathEntry.insert(0, self._downloadDir)
      self._pathEntry.pack(side="left", padx=(10, 5), pady=10)

      self._browseBtn = ctk.CTkButton(
          self._pathFrame,
          text="Browse Path",
          width=110,
          command=self._browsePath,
      )
      self._browseBtn.pack(side="right", padx=(5, 10), pady=10)

      # Progress & Action Frame
      self._progressBar = ctk.CTkProgressBar(self, width=700)
      self._progressBar.set(0)
      self._progressBar.pack(padx=20, pady=(10, 5))

      self._statusLabel = ctk.CTkLabel(
          self, text="Status: Ready", text_color="gray"
      )
      self._statusLabel.pack(padx=20, pady=5)

      self._downloadBtn = ctk.CTkButton(
          self,
          text="Download",
          font=ctk.CTkFont(size=16, weight="bold"),
          height=40,
          width=200,
          command=self._startDownloadThread,
      )
      self._downloadBtn.pack(padx=20, pady=(5, 15))

      self._onModeChange("Highest Quality")

    def _browsePath(self):
      print("4")
      selected = filedialog.askdirectory(initialdir=self._downloadDir)
      if selected:
        self._downloadDir = selected
        self._pathEntry.delete(0, "end")
        self._pathEntry.insert(0, self._downloadDir)

    def _onModeChange(self, selected_mode):
      print("5")
      """Enables or disables selector menus based on mode rules and recalculates file size."""
      has_analyzed = len(self._videoFormats) > 0 or len(self._audioFormats) > 0

      if selected_mode == "Highest Quality":
        self._videoMenu.configure(state="disabled")
        self._audioMenu.configure(state="disabled")
      elif selected_mode == "Audio Only":
        self._videoMenu.configure(state="disabled")
        self._audioMenu.configure(
            state="normal" if has_analyzed else "disabled"
        )
      elif selected_mode == "Video Only":
        self._videoMenu.configure(state="normal" if has_analyzed else "disabled")
        self._audioMenu.configure(state="disabled")
      elif selected_mode == "Custom":
        self._videoMenu.configure(state="normal" if has_analyzed else "disabled")
        self._audioMenu.configure(
            state="normal" if has_analyzed else "disabled"
        )

      self._updateEstimatedSize()

    def _startAnalyzeThread(self):
      print("6")
      url = self._urlEntry.get().strip()
      if not url:
        self._statusLabel.configure(
            text="Status: Please paste a URL first", text_color="red"
        )
        return

      self._analyzeBtn.configure(state="disabled")
      self._statusLabel.configure(
          text="Status: Extracting streams & formats...", text_color="yellow"
      )
      threading.Thread(target=self._analyzeUrl, args=(url,), daemon=True).start()

    def _analyzeUrl(self, url):
      print("7")
      ydl_opts = {
          "quiet": True,
          "no_warnings": True,
          "socket_timeout": 15,      # Stop socket hanging
          "noplaylist": True,          # Process individual video URLs cleanly
      }
      try:
          with yt_dlp.YoutubeDL(ydl_opts) as ydl:
              info = ydl.extract_info(url, download=False)
              if not info:
                  raise ValueError("Could not extract video metadata.")

              formats = info.get("formats", [])

              v_formats = {}
              a_formats = {}

              for f in formats:
                  f_id = f.get("format_id")
                  ext = f.get("ext", "N/A")
                  size_bytes = f.get("filesize") or f.get("filesize_approx") or 0
                  size_str = (
                      formatBytes(size_bytes) if size_bytes > 0 else "Unknown size"
                  )

                  if f.get("vcodec") != "none":
                      height = f.get("height")
                      res_str = f"{height}p" if height else f.get("format_note", "N/A")
                      label = (
                          f"Video: {res_str} | {ext} | Bitrate: {f.get('tbr', 0)}kbps |"
                          f" ~{size_str} (ID: {f_id})"
                      )
                      v_formats[label] = {"id": f_id, "size": size_bytes}

                  if f.get("acodec") != "none" and f.get("vcodec") == "none":
                      abr = f.get("abr") or f.get("tbr") or 0
                      label = (
                          f"Audio: {ext} | ~{int(abr)} kbps | ~{size_str} (ID: {f_id})"
                      )
                      a_formats[label] = {"id": f_id, "size": size_bytes}

              # Safely schedule UI update on the main Tkinter thread
              self.after(0, self._onAnalysisSuccess, info.get("title", ""), v_formats, a_formats)

      except Exception as e:
          # Safely schedule error display on the main Tkinter thread
          self.after(0, self._onAnalysisError, str(e))


    def _onAnalysisSuccess(self, title, v_formats, a_formats):
        """Executes safely on the main GUI event loop."""
        self._videoFormats = v_formats
        self._audioFormats = a_formats
    
        v_list = list(v_formats.keys()) if v_formats else ["No standalone video found"]
        a_list = list(a_formats.keys()) if a_formats else ["No standalone audio found"]
    
        self._videoMenu.configure(values=v_list)
        self._videoMenu.set(v_list[0])
    
        self._audioMenu.configure(values=a_list)
        self._audioMenu.set(a_list[0])
    
        self._statusLabel.configure(
            text=f"Status: Analyzed successfully! ({title[:30]}...)",
            text_color="green",
        )
        self._analyzeBtn.configure(state="normal")
        self._onModeChange(self._modeOption.get())
    
    
    def _onAnalysisError(self, error_msg):
        """Executes safely on the main GUI event loop."""
        self._statusLabel.configure(
            text=f"Analysis Error: {error_msg[:60]}", text_color="red"
        )
        self._analyzeBtn.configure(state="normal")

    """
    def _analyzeUrl(self, url):
      print("7")
      ydlOpts = {"quiet": True, 
                 "no_warnings": True,
                 "socket_timeout": 15,
                 "noplaylist": True }
      try:
        with yt_dlp.YoutubeDL(ydlOpts) as ydl:
          info = ydl.extract_info(url, download=False)
          formats = info.get("formats", [])

          self._videoFormats.clear()
          self._audioFormats.clear()

          for f in formats:
            f_id = f.get("format_id")
            ext = f.get("ext", "N/A")

            # Retrieve exact or estimated size from yt-dlp
            size_bytes = f.get("filesize") or f.get("filesize_approx") or 0
            size_str = (
                formatBytes(size_bytes) if size_bytes > 0 else "Unknown size"
            )

            # Separate Video Formats
            if f.get("vcodec") != "none":
              height = f.get("height")
              res_str = f"{height}p" if height else f.get("format_note", "N/A")
              label = (
                  f"Video: {res_str} | {ext} | Bitrate: {f.get('tbr', 0)}kbps |"
                  f" ~{size_str} (ID: {f_id})"
              )
              self._videoFormats[label] = {"id": f_id, "size": size_bytes}

            # Separate Audio Formats
            if f.get("acodec") != "none" and f.get("vcodec") == "none":
              abr = f.get("abr") or f.get("tbr") or 0
              label = (
                  f"Audio: {ext} | ~{int(abr)} kbps | ~{size_str} (ID: {f_id})"
              )
              self._audioFormats[label] = {"id": f_id, "size": size_bytes}

          v_list = (
              list(self._videoFormats.keys())
              if self._videoFormats
              else ["No standalone video found"]
          )
          a_list = (
              list(self._audioFormats.keys())
              if self._audioFormats
              else ["No standalone audio found"]
          )

          self._videoMenu.configure(values=v_list)
          self._videoMenu.set(v_list[0])

          self._audioMenu.configure(values=a_list)
          self._audioMenu.set(a_list[0])

          self._statusLabel.configure(
              text=f"Status: Analyzed successfully! ({info.get('title', '')[:30]}...)",
              text_color="green",
          )
          self._onModeChange(self._modeOption.get())

      except Exception as e:
        self._statusLabel.configure(
            text=f"Analysis Error: {str(e)[:60]}", text_color="red"
        )
      finally:
        self._analyzeBtn.configure(state="normal")
        """

    def _updateEstimatedSize(self):
      print("8")
      """Calculates combined file size based on selected mode and options."""
      mode = self._modeOption.get()

      if not self._videoFormats and not self._audioFormats:
        self._sizeValueLabel.configure(text="--")
        return

      v_size = 0
      a_size = 0

      selected_v_key = self._videoMenu.get()
      selected_a_key = self._audioMenu.get()

      if selected_v_key in self._videoFormats:
        v_size = self._videoFormats[selected_v_key]["size"]

      if selected_a_key in self._audioFormats:
        a_size = self._audioFormats[selected_a_key]["size"]

      total_bytes = 0

      if mode == "Highest Quality":
        # Take highest known sizes or sum defaults
        v_max = (
            max([item["size"] for item in self._videoFormats.values()])
            if self._videoFormats
            else 0
        )
        a_max = (
            max([item["size"] for item in self._audioFormats.values()])
            if self._audioFormats
            else 0
        )
        total_bytes = v_max + a_max

      elif mode == "Audio Only":
        total_bytes = a_size

      elif mode == "Video Only":
        total_bytes = v_size

      elif mode == "Custom":
        total_bytes = v_size + a_size

      if total_bytes > 0:
        self._sizeValueLabel.configure(text=formatBytes(total_bytes))
      else:
        self._sizeValueLabel.configure(text="Unknown / Variable")

    def _startDownloadThread(self):
      url = self._urlEntry.get().strip()
      target_path = self._pathEntry.get().strip()

      if not url or not target_path:
        self._statusLabel.configure(
            text="Status: Please check URL and download path", text_color="red"
        )
        return

      self._downloadBtn.configure(state="disabled")
      self._statusLabel.configure(
          text="Status: Initializing download...", text_color="yellow"
      )

      threading.Thread(
          target=self._runDownload, args=(url, target_path), daemon=True
      ).start()

    def _progressHook(self, d):
      print("9")
      if d["status"] == "downloading":
        total_bytes = d.get("total_bytes") or d.get("total_bytes_estimate", 0)
        downloaded = d.get("downloaded_bytes", 0)
        if total_bytes > 0:
          percent = downloaded / total_bytes
          self._progressBar.set(percent)
          self._statusLabel.configure(
              text=f"Status: Downloading... {percent * 100:.1f}%",
              text_color="cyan",
          )
      elif d["status"] == "finished":
        self._progressBar.set(1.0)
        self._statusLabel.configure(
            text="Status: Merging/Converting with FFmpeg...", text_color="cyan"
        )

    def _runDownload(self, url, save_path):
      print("10")
      mode = self._modeOption.get()

      out_template = os.path.join(save_path, "%(title)s.%(ext)s")
      ydlOpts = {
          "outtmpl": out_template,
          "progress_hooks": [self._progressHook],
      }

      if mode == "Highest Quality":
        ydlOpts["format"] = "bestvideo+bestaudio/best"
        ydlOpts["merge_output_format"] = "mp4"

      elif mode == "Audio Only":
        selected_a = self._audioMenu.get()
        aData = self._audioFormats.get(selected_a)
        ydlOpts["format"] = (
            aData["id"] if aData and aData.get("id") else "bestaudio/best"
        )
        ydlOpts["postprocessors"] = [{
            "key": "FFmpegExtractAudio",
            "preferredcodec": "mp3",
            "preferredquality": "192",
        }]

      elif mode == "Video Only":
        selectedV = self._videoMenu.get()
        vData = self._videoFormats.get(selectedV)
        ydlOpts["format"] = (
            vData["id"] if vData and vData.get("id") else "bestvideo"
        )

      elif mode == "Custom":
        selectedV = self._videoMenu.get()
        selected_a = self._audioMenu.get()

        vData = self._videoFormats.get(selectedV)
        aData = self._audioFormats.get(selected_a)

        vId = vData["id"] if vData else None
        aId = aData["id"] if aData else None

        if vId and aId:
          ydlOpts["format"] = f"{vId}+{aId}"
        else:
          ydlOpts["format"] = "bestvideo+bestaudio/best"
        ydlOpts["merge_output_format"] = "mkv"

      try:
        with yt_dlp.YoutubeDL(ydlOpts) as ydl:
          ydl.download([url])
        self._statusLabel.configure(
            text="Status: Download Complete!", text_color="green"
        )
      except Exception as e:
        self._statusLabel.configure(
            text=f"Download Error: {str(e)[:60]}", text_color="red"
        )
      finally:
        self._downloadBtn.configure(state="normal")

    #def __init__(self):
    #    super().__init__()

        #self._title("Advance Media Downloader")
        #self._geometry("740x630")
        #self._resizable(False, False)

        #self._vidFormats = {}
        #self._audFormats = {}
        #self._downlaodDir = os.path.expanduser("~/Donwloads")


    #def
