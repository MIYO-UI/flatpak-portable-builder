# Flatpak Portable Builder

A KDE application for building Flatpak packages from Windows PortableApps using Wine.

## Features

- Import and analyze Windows PortableApps
- Configure Wine environment for optimal compatibility
- Generate Flatpak manifests automatically
- Build and install Flatpak packages
- Support for DXVK (DirectX to Vulkan translation)
- Easy-to-use KDE-based interface

## Requirements

- KDE Plasma or KDE Frameworks
- Qt 5.12+
- Flatpak and flatpak-builder
- Wine (for testing and running Windows applications)
- bsdtar (for extracting archives)

## Installation

### From Source

```bash
# Install dependencies
sudo dnf install cmake extra-cmake-modules qt5-qtbase-devel kf5-ki18n-devel kf5-kxmlgui-devel kf5-kcrash-devel flatpak flatpak-builder wine bsdtar

# Clone the repository
git clone https://github.com/yourusername/flatpack-portable-builder.git
cd flatpack-portable-builder

# Build
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make -j$(nproc)

# Install
sudo make install
```

## Usage

1. Launch the application from your application menu or run `flatpack-portable-builder`
2. Click "Import New App" to select a Windows PortableApp directory
3. Fill in the application details or let the app detect them automatically
4. Configure Wine settings for the application
5. Generate the Flatpak manifest
6. Click "Build Flatpak" to create and install the Flatpak package

## How It Works

Flatpak Portable Builder converts Windows PortableApps to Flatpak packages by:

1. **Analyzing the PortableApp**: The app scans the directory structure to identify the main executable and dependencies.

2. **Configuring Wine**: The appropriate Wine version and settings are determined based on the application type.

3. **Creating a Flatpak manifest**: A Flatpak manifest is generated that includes:
   - Basic application metadata
   - Wine runtime configuration
   - Filesystem permissions
   - DXVK support for DirectX applications (optional)

4. **Building the Flatpak**: The app uses `flatpak-builder` to create a Flatpak package that contains:
   - The Windows application
   - A properly configured Wine environment
   - All necessary dependencies

5. **Installation**: The resulting Flatpak is installed into the user's Flatpak repository.

## PortableApps Compatibility

This tool works best with self-contained Windows portable applications that:
- Don't require installation
- Store all settings in their own directory
- Are designed to run from removable media

Examples of compatible PortableApps:
- Applications from [PortableApps.com](https://portableapps.com/)
- Standalone portable software
- Portable versions of popular Windows utilities

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the GPL-3.0 License - see the LICENSE file for details.