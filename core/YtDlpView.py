import os
import sys
import threading
import customtkinter as ctk
from tkinter import filedialog
import yt_dlp


def format_bytes(bytes_num):
  """Utility to convert raw bytes to human-readable size string."""
  if not bytes_num or bytes_num <= 0:
    return "Unknown size"
  for unit in ["B", "KB", "MB", "GB", "TB"]:
    if bytes_num < 1024.0:
      return f"{bytes_num:.2f} {unit}"
    bytes_num /= 1024.0
  return f"{bytes_num:.2f} PB"

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


      # Data structures for format extraction
      # Format: label -> {"id": format_id, "size": filesize_in_bytes}
        self.video_formats = {}
        self.audio_formats = {}
        self.download_dir = os.path.expanduser("~/Downloads")

      # Layout Containers
        self.create_widgets()

    def create_widgets(self):
      # Header Title
      self.title_label = ctk.CTkLabel(
          self,
          text="Video / Audio Downloader & Converter",
          font=ctk.CTkFont(size=22, weight="bold"),
      )
      self.title_label.pack(padx=20, pady=(20, 10))

      # URL Frame
      self.url_frame = ctk.CTkFrame(self)
      self.url_frame.pack(padx=20, pady=10, fill="x")

      self.url_entry = ctk.CTkEntry(
          self.url_frame,
          placeholder_text="Paste Video / Playlist URL here...",
          width=530,
      )
      self.url_entry.pack(side="left", padx=(10, 5), pady=10)

      self.analyze_btn = ctk.CTkButton(
          self.url_frame,
          text="Analyze",
          width=120,
          command=self.start_analyze_thread,
      )
      self.analyze_btn.pack(side="right", padx=(5, 10), pady=10)

      # Mode Selector Frame
      self.mode_frame = ctk.CTkFrame(self)
      self.mode_frame.pack(padx=20, pady=10, fill="x")

      self.mode_label = ctk.CTkLabel(
          self.mode_frame,
          text="Download Mode:",
          font=ctk.CTkFont(weight="bold"),
      )
      self.mode_label.pack(side="left", padx=10, pady=10)

      self.mode_option = ctk.CTkOptionMenu(
          self.mode_frame,
          values=["Highest Quality", "Audio Only", "Video Only", "Custom"],
          command=self.on_mode_change,
          width=200,
      )
      self.mode_option.pack(side="right", padx=10, pady=10)

      # Qualities Frame
      self.qualities_frame = ctk.CTkFrame(self)
      self.qualities_frame.pack(padx=20, pady=10, fill="x")

      # Video Quality Dropdown
      self.v_label = ctk.CTkLabel(self.qualities_frame, text="Video Format:")
      self.v_label.grid(row=0, column=0, padx=10, pady=10, sticky="w")

      self.video_menu = ctk.CTkOptionMenu(
          self.qualities_frame,
          values=["Analyze URL first"],
          command=lambda _: self.update_estimated_size(),
          state="disabled",
          width=500,
      )
      self.video_menu.grid(row=0, column=1, padx=10, pady=10)

      # Audio Quality Dropdown
      self.a_label = ctk.CTkLabel(self.qualities_frame, text="Audio Format:")
      self.a_label.grid(row=1, column=0, padx=10, pady=10, sticky="w")

      self.audio_menu = ctk.CTkOptionMenu(
          self.qualities_frame,
          values=["Analyze URL first"],
          command=lambda _: self.update_estimated_size(),
          state="disabled",
          width=500,
      )
      self.audio_menu.grid(row=1, column=1, padx=10, pady=10)

      # File Size Estimation Display
      self.size_frame = ctk.CTkFrame(self)
      self.size_frame.pack(padx=20, pady=5, fill="x")

      self.size_title_label = ctk.CTkLabel(
          self.size_frame,
          text="Approx. File Size:",
          font=ctk.CTkFont(weight="bold"),
      )
      self.size_title_label.pack(side="left", padx=10, pady=8)

      self.size_value_label = ctk.CTkLabel(
          self.size_frame,
          text="--",
          font=ctk.CTkFont(size=14, weight="bold"),
          text_color="cyan",
      )
      self.size_value_label.pack(side="left", padx=5, pady=8)

      # Path Selector Frame
      self.path_frame = ctk.CTkFrame(self)
      self.path_frame.pack(padx=20, pady=10, fill="x")

      self.path_entry = ctk.CTkEntry(self.path_frame, width=540)
      self.path_entry.insert(0, self.download_dir)
      self.path_entry.pack(side="left", padx=(10, 5), pady=10)

      self.browse_btn = ctk.CTkButton(
          self.path_frame,
          text="Browse Path",
          width=110,
          command=self.browse_path,
      )
      self.browse_btn.pack(side="right", padx=(5, 10), pady=10)

      # Progress & Action Frame
      self.progress_bar = ctk.CTkProgressBar(self, width=700)
      self.progress_bar.set(0)
      self.progress_bar.pack(padx=20, pady=(10, 5))

      self.status_label = ctk.CTkLabel(
          self, text="Status: Ready", text_color="gray"
      )
      self.status_label.pack(padx=20, pady=5)

      self.download_btn = ctk.CTkButton(
          self,
          text="Download",
          font=ctk.CTkFont(size=16, weight="bold"),
          height=40,
          width=200,
          command=self.start_download_thread,
      )
      self.download_btn.pack(padx=20, pady=(5, 15))

      self.on_mode_change("Highest Quality")

    def browse_path(self):
      selected = filedialog.askdirectory(initialdir=self.download_dir)
      if selected:
        self.download_dir = selected
        self.path_entry.delete(0, "end")
        self.path_entry.insert(0, self.download_dir)

    def on_mode_change(self, selected_mode):
      """Enables or disables selector menus based on mode rules and recalculates file size."""
      has_analyzed = len(self.video_formats) > 0 or len(self.audio_formats) > 0

      if selected_mode == "Highest Quality":
        self.video_menu.configure(state="disabled")
        self.audio_menu.configure(state="disabled")
      elif selected_mode == "Audio Only":
        self.video_menu.configure(state="disabled")
        self.audio_menu.configure(
            state="normal" if has_analyzed else "disabled"
        )
      elif selected_mode == "Video Only":
        self.video_menu.configure(state="normal" if has_analyzed else "disabled")
        self.audio_menu.configure(state="disabled")
      elif selected_mode == "Custom":
        self.video_menu.configure(state="normal" if has_analyzed else "disabled")
        self.audio_menu.configure(
            state="normal" if has_analyzed else "disabled"
        )

      self.update_estimated_size()

    def start_analyze_thread(self):
      url = self.url_entry.get().strip()
      if not url:
        self.status_label.configure(
            text="Status: Please paste a URL first", text_color="red"
        )
        return

      self.analyze_btn.configure(state="disabled")
      self.status_label.configure(
          text="Status: Extracting streams & formats...", text_color="yellow"
      )
      threading.Thread(target=self.analyze_url, args=(url,), daemon=True).start()

    def analyze_url(self, url):
      ydl_opts = {"quiet": True, "no_warnings": True}
      try:
        with yt_dlp.YoutubeDL(ydl_opts) as ydl:
          info = ydl.extract_info(url, download=False)
          formats = info.get("formats", [])

          self.video_formats.clear()
          self.audio_formats.clear()

          for f in formats:
            f_id = f.get("format_id")
            ext = f.get("ext", "N/A")

            # Retrieve exact or estimated size from yt-dlp
            size_bytes = f.get("filesize") or f.get("filesize_approx") or 0
            size_str = (
                format_bytes(size_bytes) if size_bytes > 0 else "Unknown size"
            )

            # Separate Video Formats
            if f.get("vcodec") != "none":
              height = f.get("height")
              res_str = f"{height}p" if height else f.get("format_note", "N/A")
              label = (
                  f"Video: {res_str} | {ext} | Bitrate: {f.get('tbr', 0)}kbps |"
                  f" ~{size_str} (ID: {f_id})"
              )
              self.video_formats[label] = {"id": f_id, "size": size_bytes}

            # Separate Audio Formats
            if f.get("acodec") != "none" and f.get("vcodec") == "none":
              abr = f.get("abr") or f.get("tbr") or 0
              label = (
                  f"Audio: {ext} | ~{int(abr)} kbps | ~{size_str} (ID: {f_id})"
              )
              self.audio_formats[label] = {"id": f_id, "size": size_bytes}

          v_list = (
              list(self.video_formats.keys())
              if self.video_formats
              else ["No standalone video found"]
          )
          a_list = (
              list(self.audio_formats.keys())
              if self.audio_formats
              else ["No standalone audio found"]
          )

          self.video_menu.configure(values=v_list)
          self.video_menu.set(v_list[0])

          self.audio_menu.configure(values=a_list)
          self.audio_menu.set(a_list[0])

          self.status_label.configure(
              text=f"Status: Analyzed successfully! ({info.get('title', '')[:30]}...)",
              text_color="green",
          )
          self.on_mode_change(self.mode_option.get())

      except Exception as e:
        self.status_label.configure(
            text=f"Analysis Error: {str(e)[:60]}", text_color="red"
        )
      finally:
        self.analyze_btn.configure(state="normal")

    def update_estimated_size(self):
      """Calculates combined file size based on selected mode and options."""
      mode = self.mode_option.get()

      if not self.video_formats and not self.audio_formats:
        self.size_value_label.configure(text="--")
        return

      v_size = 0
      a_size = 0

      selected_v_key = self.video_menu.get()
      selected_a_key = self.audio_menu.get()

      if selected_v_key in self.video_formats:
        v_size = self.video_formats[selected_v_key]["size"]

      if selected_a_key in self.audio_formats:
        a_size = self.audio_formats[selected_a_key]["size"]

      total_bytes = 0

      if mode == "Highest Quality":
        # Take highest known sizes or sum defaults
        v_max = (
            max([item["size"] for item in self.video_formats.values()])
            if self.video_formats
            else 0
        )
        a_max = (
            max([item["size"] for item in self.audio_formats.values()])
            if self.audio_formats
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
        self.size_value_label.configure(text=format_bytes(total_bytes))
      else:
        self.size_value_label.configure(text="Unknown / Variable")

    def start_download_thread(self):
      url = self.url_entry.get().strip()
      target_path = self.path_entry.get().strip()

      if not url or not target_path:
        self.status_label.configure(
            text="Status: Please check URL and download path", text_color="red"
        )
        return

      self.download_btn.configure(state="disabled")
      self.status_label.configure(
          text="Status: Initializing download...", text_color="yellow"
      )

      threading.Thread(
          target=self.run_download, args=(url, target_path), daemon=True
      ).start()

    def progress_hook(self, d):
      if d["status"] == "downloading":
        total_bytes = d.get("total_bytes") or d.get("total_bytes_estimate", 0)
        downloaded = d.get("downloaded_bytes", 0)
        if total_bytes > 0:
          percent = downloaded / total_bytes
          self.progress_bar.set(percent)
          self.status_label.configure(
              text=f"Status: Downloading... {percent * 100:.1f}%",
              text_color="cyan",
          )
      elif d["status"] == "finished":
        self.progress_bar.set(1.0)
        self.status_label.configure(
            text="Status: Merging/Converting with FFmpeg...", text_color="cyan"
        )

    def run_download(self, url, save_path):
      mode = self.mode_option.get()

      out_template = os.path.join(save_path, "%(title)s.%(ext)s")
      ydl_opts = {
          "outtmpl": out_template,
          "progress_hooks": [self.progress_hook],
      }

      if mode == "Highest Quality":
        ydl_opts["format"] = "bestvideo+bestaudio/best"
        ydl_opts["merge_output_format"] = "mp4"

      elif mode == "Audio Only":
        selected_a = self.audio_menu.get()
        a_data = self.audio_formats.get(selected_a)
        ydl_opts["format"] = (
            a_data["id"] if a_data and a_data.get("id") else "bestaudio/best"
        )
        ydl_opts["postprocessors"] = [{
            "key": "FFmpegExtractAudio",
            "preferredcodec": "mp3",
            "preferredquality": "192",
        }]

      elif mode == "Video Only":
        selected_v = self.video_menu.get()
        v_data = self.video_formats.get(selected_v)
        ydl_opts["format"] = (
            v_data["id"] if v_data and v_data.get("id") else "bestvideo"
        )

      elif mode == "Custom":
        selected_v = self.video_menu.get()
        selected_a = self.audio_menu.get()

        v_data = self.video_formats.get(selected_v)
        a_data = self.audio_formats.get(selected_a)

        v_id = v_data["id"] if v_data else None
        a_id = a_data["id"] if a_data else None

        if v_id and a_id:
          ydl_opts["format"] = f"{v_id}+{a_id}"
        else:
          ydl_opts["format"] = "bestvideo+bestaudio/best"
        ydl_opts["merge_output_format"] = "mkv"

      try:
        with yt_dlp.YoutubeDL(ydl_opts) as ydl:
          ydl.download([url])
        self.status_label.configure(
            text="Status: Download Complete!", text_color="green"
        )
      except Exception as e:
        self.status_label.configure(
            text=f"Download Error: {str(e)[:60]}", text_color="red"
        )
      finally:
        self.download_btn.configure(state="normal")

    #def __init__(self):
    #    super().__init__()

        #self._title("Advance Media Downloader")
        #self._geometry("740x630")
        #self._resizable(False, False)

        #self._vidFormats = {}
        #self._audFormats = {}
        #self._downlaodDir = os.path.expanduser("~/Donwloads")


    #def
