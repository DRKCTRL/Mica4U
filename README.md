<h1 align="center">
<img src="Mica4UHeader.png" alt="Mica4U" width="256">
</h1>

A modern, user-friendly tool to apply Mica, Acrylic, and Blur effects to Windows Explorer — for Windows 10 and 11.

<div align="center">

[![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0) [![GitHub release](https://img.shields.io/github/v/release/DRKCTRLDEV/Mica4U)](https://github.com/DRKCTRLDEV/Mica4U/releases) [![Downloads](https://img.shields.io/github/downloads/DRKCTRLDEV/Mica4U/total)](https://github.com/DRKCTRLDEV/Mica4U/releases) ![Last Commit](https://img.shields.io/github/last-commit/DRKCTRLDEV/Mica4U)

</div>

---

## ⚠️ Important Notices

> **Compatibility**  
> Some effects may not work as expected on **Windows 11 Version 24H2 (Build 26100.3624)** due to this they have been disabled until further notice.  
> Confirmed working on:
> - **Windows 10 Pro 22H2 (Build 19045.5608)**
> - **Windows 11 Home 24H2 (Build 26100.3775)**

> **Disclaimer**  
> The developer/s (DRK & Maplespe) are not responsible for any damage caused by this software. *Use at your own risk*

---

## ✨ Features

<details>
<summary><strong>Visual Effects</strong></summary>

| Effect        | Description                        | Compatibility                   |
|---------------|------------------------------------|----------------------------------|
| Acrylic       | Blur with noise                    | Windows 10/11                   |
| Blur          | Classic blurred background         | Windows 10/11                   |
| Blur (Clear)  | Clean blur without noise           | Windows 11 or 10 22H2 or lower |
| Mica          | System-coloured glass effect        | Windows 11 only                 |
| Mica Alt      | Alternative version of Mica        | Windows 11 only                 |

</details>

<details>
<summary><strong>UI & Functionality</strong></summary>

- Custom RGBA colour & transparency controls  
- Real-time colour preview with built-in colour picker  
- Light/Dark mode presets  
- Font Awesome icons in a modern layout  
- Configurable window size, theme & appearance  
- Logging system for debugging  
- Portable mode support  
- Preset management for quick switching  

</details>

---

## 🖼️ Showcase

<details>
<summary><strong>View App Screenshots</strong></summary>

### Mica4U - General UI
![M4UGeneralUI](https://raw.githubusercontent.com/DRKCTRLDEV/Mica4U/main/screenshots/Mica4UGeneral.png)

### Mica4U - ColourPicker UI
![M4UColourPickerUI](https://raw.githubusercontent.com/DRKCTRLDEV/Mica4U/main/screenshots/Mica4UColourPicker.png)

### Mica4U - Settings UI
![M4USettingsUI](https://raw.githubusercontent.com/DRKCTRLDEV/Mica4U/main/screenshots/Mica4USettings.png)

### Mica4U - UpdateDialog UI
![M4UUpdateDialogUI](https://raw.githubusercontent.com/DRKCTRLDEV/Mica4U/main/screenshots/Mica4UUpdateDialog.png)

</details>

<details>
<summary><strong>View Explorer Screenshots</strong></summary>

### Acrylic (Win11) (Dark Preset)
![AcrylicWin11Dark](https://raw.githubusercontent.com/DRKCTRLDEV/Mica4U/main/screenshots/AcrylicWin11Dark.png)

### Blur (Win11) (Dark Preset)
![BlurWin11Dark](https://raw.githubusercontent.com/DRKCTRLDEV/Mica4U/main/screenshots/BlurWin11Dark.png)

### Mica (Win11) (Dark Preset)
![MicaWin11Dark](https://raw.githubusercontent.com/DRKCTRLDEV/Mica4U/main/screenshots/MicaWin11Dark.png)

### Mica Alt (Win11) (Dark Preset)
![MicaAltWin11Dark](https://raw.githubusercontent.com/DRKCTRLDEV/Mica4U/main/screenshots/MicaAltWin11Dark.png)

</details>

---

## 💻 System Requirements

- Windows 10 or Windows 11  
- Administrator privileges  
- Compatible with StartAllBack, Rectify11, etc.

---

## 📥 Installation

<details>
<summary><strong>Method 1: Installer (Recommended)</strong></summary>

1. Download the latest installer from the [Releases](https://github.com/DRKCTRLDEV/Mica4U/releases) page
   - Choose `Mica4U_Setup_x64.exe` for 64-bit Windows (recommended for most users)
   - Choose `Mica4U_Setup_x86.exe` for 32-bit Windows
2. Run the installer and follow the instructions  
3. Launch from the Start Menu or Desktop shortcut  

</details>

<details>
<summary><strong>Method 2: Portable</strong></summary>

1. Download the ZIP from the [Releases](https://github.com/DRKCTRLDEV/Mica4U/releases) page
   - Choose `Mica4U_portable_x64.zip` for 64-bit Windows (recommended for most users)
   - Choose `Mica4U_portable_x86.zip` for 32-bit Windows  
2. Extract to any location  
3. Run `Mica4U.exe`  

</details>

> **Note**: Both methods require admin rights.
>
> **Which version should I choose?**  
> - If you're using a modern computer with Windows 10/11, choose the x64 version
> - If you're using an older computer or a 32-bit version of Windows, choose the x86 version
> - If you're unsure, right-click on "This PC" or "My Computer", select "Properties", and check your "System type"

---

## 🎮 Usage

<details>
<summary><strong>Basic Setup</strong></summary>

1. Launch Mica4U  
2. Choose your desired effect from the main panel  

</details>

<details>
<summary><strong>Customization Options</strong></summary>

### 🖌️ Effects  
- Clear Address Bar  
- Clear Toolbar  
- Clear Background  
- Show Separator  

### 🎨 colours  
- Adjust RGBA values with sliders or use the colour picker  
- Real-time preview  
- Save custom colours as presets  

### 🧩 Presets  
- Light/Dark mode  
- Create and save your own colour presets  
- Quick preset switching  

### ⚙️ Settings Panel    
- Effects: Toggle unsupported options & effects 
- Advanced: Logging level, config path  
- About: Version info and credits  

</details>

<details>
<summary><strong>Applying Changes</strong></summary>

1. Click the **Install** button (download icon)  
2. Explorer will automatically restart  
3. Your settings will take effect  

</details>

---

## 🛠️ Troubleshooting

- **Explorer crash recovery**: Hold `ESC` when opening Explorer to skip effect injection  
- **Remove effects**: Use the Uninstall (trash icon) button  
- **Logs**:  
  - Installer: `%APPDATA%\Mica4U\logs\mica4u.log`  
  - Portable: `logs\mica4u.log` in the app folder  
- **Check Compatibility**: Use options in Settings > Compatibility  

---

## 🗑️ Uninstallation

<details>
<summary><strong>Method 1: Windows Settings</strong></summary>

1. Open **Settings > Apps > Installed Apps**  
2. Find "Mica4U"  
3. Click **Uninstall**  

</details>

<details>
<summary><strong>Method 2: Manual Cleanup</strong></summary>

1. Open Mica4U and click **Uninstall**  
2. Delete the Mica4U folder  
3. (Optional) Remove `%APPDATA%\Mica4U`  

</details>

---

## 🔧 Building from Source

<details>
<summary><strong>Quick Build</strong></summary>

### 🧱 Prerequisites  
- Python 3.10+  
- `PyQt6`, `psutil`, `qtawesome`, `PyInstaller`  
- Inno Setup 6  
- (Optional) 7-Zip for compressed portable builds  

### ⚙️ Build Steps

```bash
git clone https://github.com/DRKCTRLDEV/Mica4U.git
cd Mica4U
pip install -r requirements.txt
```

1. Install [Inno Setup 6](https://jrsoftware.org/isdl.php)  
2. Run one of the following commands:

```bash
# Build both x86 and x64 versions (default)
./build.cmd

# Build only x64 version
./build.cmd --x64

# Build only x86 version
./build.cmd --x86

# Build with verbose output
./build.cmd --verbose
```

3. The output files will be in the `compiled/output` directory:
   - Installers: `Mica4U_Setup_x64.exe` and/or `Mica4U_Setup_x86.exe`
   - Portable: `Mica4U_portable_x64.zip` and/or `Mica4U_portable_x86.zip`

</details>

<details>
<summary><strong>Manual Build (Not Recommended)</strong></summary>

```bash
# For x64 build
set TARGET_ARCH=x64
pyinstaller compiled/Mica4U.spec
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" "compiled\installer.iss" /DArch="x64"

# For x86 build
set TARGET_ARCH=x86
pyinstaller compiled/Mica4U.spec
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" "compiled\installer.iss" /DArch="x86"
```

</details>

---

## 📄 License & Credits

- Licensed under [LGPL v3](https://www.gnu.org/licenses/lgpl-3.0)  
- Built with Python, PyQt6, and PyInstaller  
- Icons by [Font Awesome](https://fontawesome.com)
- Core Functionality by [MapleSpe](https://github.com/Maplespe)
- GUI and Automation by [←{ 𝓓𝓡𝓚 }→](https://github.com/DRKCTRLDEV)
