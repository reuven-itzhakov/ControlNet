# ControlNet

A comprehensive Windows monitoring and activity tracking application with Firebase cloud synchronization. Built with C++ and featuring real-time keystroke logging, automatic screenshots, and secure cloud backup.

> ⚠️ **Educational Purpose Only**: This software is designed for legitimate monitoring purposes only (parental control, employee monitoring with consent, personal device backup). Unauthorized use may violate local laws. Use responsibly and ethically.

## 🎯 Features

### Core Functionality
- **Real-time Keystroke Logging** with full Unicode/Emoji support 🌍
- **Automatic Screenshots** every 5 minutes with intelligent compression
- **Cloud Synchronization** with Firebase/Firestore
- **Device Tracking** using Windows Machine GUID
- **Secure Authentication** with per-device registration
- **Background Operation** with minimal resource usage

### Advanced Capabilities
- ✅ Full Unicode support including emojis (😀 🎉 🌟)
- ✅ Multi-language keyboard support
- ✅ Process tracking and window title monitoring
- ✅ Automatic data compression (50% screenshot reduction)
- ✅ Offline data storage with automatic sync
- ✅ Comprehensive device information collection
- ✅ Customizable hotkeys for UI access

## 🏗️ System Architecture

<img width="1247" height="1229" alt="system diagram" src="https://github.com/user-attachments/assets/ddeb89d2-fe80-4423-bd41-a2558c0d66a7" />

## 📋 Requirements

### System Requirements
- **OS**: Windows 10/11 (64-bit)
- **Compiler**: Visual Studio 2019 or later with C++14 support
- **RAM**: Minimum 2GB (4GB recommended)
- **Storage**: 100MB for application + variable for logs

### Dependencies
- **Firebase C++ SDK** (v11.0.0 or later)
  - Firebase App
  - Firebase Auth
  - Firebase Firestore
- **GDI+** (included with Windows)
- **nlohmann/json** library
- **Windows SDK** 10.0 or later

## 🚀 Installation

### 1. Clone the Repository
```bash
git clone https://github.com/reuven-itzhakov/ControlNet.git
cd ControlNet
```

### 2. Set Up Firebase
1. Create a Firebase project at [Firebase Console](https://console.firebase.google.com/)
2. Enable **Authentication** (Email/Password)
3. Enable **Cloud Firestore**
4. Download your `google-services.json` configuration
5. Place Firebase C++ SDK in your project directory

### 3. Configure the Project
1. Open `ControlNet.sln` in Visual Studio
2. Update include paths for Firebase SDK
3. Link Firebase libraries:
   ```
   firebase_app.lib
   firebase_auth.lib
   firebase_firestore.lib
   ```
4. Add your Firebase configuration in `FirebaseManager.cpp`

### 4. Build
```
Build > Build Solution (Ctrl+Shift+B)
```

## 💻 Usage

### First Launch
1. Run the application
2. Sign in with your Firebase credentials
3. The application minimizes to background
4. Use the hotkey (default: `Ctrl+Shift+F12`) to show/hide the UI

### Viewing Logs
- **Left Pane**: Shows dates with activity
- **Right Pane**: Displays keystrokes and timestamps
- **Screenshots**: Stored locally and synced to Firebase

### Hotkey Configuration
Access via: **Menu > Settings**
- Default: `Ctrl+Shift+F12`
- Customizable to any key combination

## 📁 File Structure

```
ControlNet/
├── ControlNet/
│   ├── ControlNet.cpp           # Main application entry
│   ├── KeyLogger.cpp            # Keyboard/mouse hooks
│   ├── ScreenCapture.cpp        # Screenshot functionality
│   ├── JsonLog.cpp              # Local JSON logging
│   ├── DBcommunication.cpp      # Firebase sync
│   ├── AuthService.cpp          # Authentication
│   ├── DeviceInfo.cpp           # Device information
│   ├── UIUtils.cpp              # User interface
│   ├── UnicodeUtils.cpp         # Unicode/emoji support
│   └── ...
├── {DeviceGUID}/                # Local storage
│   └── {date}/
│       ├── {date}.json          # Keystroke logs
│       └── screenshots/         # Screenshot images
└── README.md
```

## 🗄️ Data Storage

### Local Storage Structure
```
{DeviceGUID}/
├── 2024-12-19/
│   ├── 2024-12-19.json
│   └── screenshots/
│       ├── 1703001234.png
│       └── 1703001534.png
└── 2024-12-20/
    ├── 2024-12-20.json
    └── screenshots/
        └── 1703087634.png
```

### Firebase Structure
```
/records/{deviceID}/
  ├── keyStrokes/{date}        # Keystroke logs
  └── screenshots/{date}       # Base64 encoded images

/devices/{deviceID}            # Device information
  ├── computerName
  ├── userName
  ├── cpuName
  ├── totalMemory
  └── ...

/users/users/{email}           # User's devices
  └── [deviceID1, deviceID2]
```

## 🔒 Security & Privacy

### Data Protection
- ✅ All data encrypted in transit (Firebase SSL/TLS)
- ✅ Authentication required for cloud access
- ✅ Device-based access control
- ✅ Local storage secured by Windows file permissions

### Privacy Features
- Device identification via Windows Machine GUID (not MAC address)
- No administrator privileges required
- User consent required for monitoring
- Transparent data collection

### Compliance
⚠️ **Important**: Ensure compliance with local laws:
- Obtain proper consent before deployment
- Notify monitored individuals as required by law
- Use only for legitimate purposes
- Respect privacy regulations (GDPR, CCPA, etc.)

## 🛠️ Development

### Project Structure
See [PROJECT_ORGANIZATION.md](ControlNet/PROJECT_ORGANIZATION.md) for detailed module documentation.

### Key Components

#### Core Modules
- **ControlNet.cpp**: Application entry point and message loop
- **KeyLogger.cpp**: Low-level keyboard/mouse hooks
- **ScreenCapture.cpp**: Screenshot capture with compression
- **JsonLog.cpp**: JSON file management
- **DBcommunication.cpp**: Firebase synchronization

#### Utility Modules
- **UnicodeUtils.cpp**: Unicode/emoji handling
- **DeviceInfo.cpp**: System information collection
- **AuthService.cpp**: Authentication management
- **UIUtils.cpp**: User interface components

### Building from Source

```bash
# Requirements
- Visual Studio 2019+
- Windows SDK 10.0+
- Firebase C++ SDK

# Build steps
1. Install dependencies
2. Configure Firebase
3. Open solution in Visual Studio
4. Build > Build Solution
```

## 🐛 Troubleshooting

### Common Issues

**Firebase not uploading screenshots**
- Check internet connection
- Verify Firebase initialization
- Check file sizes (Firestore has 1MB document limit)
- Review Firebase console for errors

**Keyboard hooks not working**
- Run application with appropriate permissions
- Check for conflicting applications
- Verify Windows security settings

**UI not showing with hotkey**
- Check hotkey configuration in settings
- Verify no conflicts with other applications
- Try default hotkey: `Ctrl+Shift+F12`

### Debug Mode
Enable detailed logging by uncommenting debug statements in:
- `ScreenCapture.cpp`
- `DBcommunication.cpp`
- `KeyLogger.cpp`

## 📊 Performance

- **CPU Usage**: < 1% idle, < 3% during capture
- **Memory**: ~50MB baseline, ~100MB with active logging
- **Storage**: ~1-5MB per day (varies by activity)
- **Network**: Minimal (uploads only when changes occur)

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Code Style
- Follow existing C++ code style
- Use meaningful variable names
- Comment complex logic
- Test thoroughly before submitting

## 📝 License

This project is provided for educational and legitimate monitoring purposes only. 

**Disclaimer**: The developers are not responsible for misuse of this software. Users must comply with all applicable laws and regulations in their jurisdiction.

## 👨‍💻 Author

**Reuven Itzhakov**
- GitHub: [@reuven-itzhakov](https://github.com/reuven-itzhakov)
- Repository: [ControlNet](https://github.com/reuven-itzhakov/ControlNet)

## 🙏 Acknowledgments

- Firebase C++ SDK by Google
- nlohmann/json library
- Windows GDI+ for image processing
- The C++ community for invaluable resources

## 📜 Version History

### v1.0.0 (Current)
- ✅ Real-time keystroke logging
- ✅ Automatic screenshot capture
- ✅ Firebase cloud synchronization
- ✅ Full Unicode/emoji support
- ✅ Device tracking and authentication
- ✅ Image compression
- ✅ Background operation

### Roadmap
- [ ] Multi-monitor support
- [ ] Video recording capability
- [ ] Advanced filtering options
- [ ] Web dashboard for remote access
- [ ] Encrypted local storage
- [ ] Plugin system for extensions

---

⚠️ **Legal Notice**: This software is intended for legitimate monitoring purposes only. Unauthorized monitoring or surveillance may be illegal. Always obtain proper consent and comply with local laws and regulations. Use responsibly.

## 📞 Support

For issues, questions, or suggestions:
1. Check [Troubleshooting](#-troubleshooting) section
2. Review [PROJECT_ORGANIZATION.md](PROJECT_ORGANIZATION.md)
3. Open an issue on GitHub
4. Contact via GitHub profile

---

**Made with ❤️ for legitimate monitoring needs**
