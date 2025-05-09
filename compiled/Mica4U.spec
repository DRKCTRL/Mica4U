# -*- mode: python ; coding: utf-8 -*-
import os
import sys

block_cipher = None

# Get target architecture from environment variable
target_arch = os.environ.get('TARGET_ARCH', None)

a = Analysis(
    ['..\\main.py'],
    pathex=[
        '..\\',
    ],
    binaries=[],
    datas=[
        ('..\\ExplorerBlurMica.dll', '.'),
        ('..\\icon.ico', '.'),
        ('..\\initialise.cmd', '.'),
    ],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)

pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    [],
    name='Mica4U',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    target_arch=target_arch,
    codesign_identity=None,
    entitlements_file=None,
    icon='..\\icon.ico',
    onefile=True
)
