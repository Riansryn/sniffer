# Network Sniffer - Qt5 Cross-Platform Application

A simple Wireshark-like network packet sniffer built with Qt5 and C++, supporting both Linux and Android platforms.

![Version](https://img.shields.io/badge/version-2.1.0-blue)
![Qt](https://img.shields.io/badge/Qt-5.15-green)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Android-lightgrey)

## Features

- ✅ Real-time packet capture and display
- ✅ Automatic network interface detection (including "any" with real interface names)
- ✅ **Interface column** showing actual network interface per packet
- ✅ **Unix epoch timestamps** with microsecond precision
- ✅ **Newest packets on top** for better real-time monitoring
- ✅ Packet details view with hex dump
- ✅ Protocol identification (TCP, UDP, ICMP, ARP)
- ✅ **Export to PCAP format** (Wireshark compatible)
- ✅ Cross-platform: Linux (using libpcap) and Android (basic socket capture)
- ✅ Modern Qt Quick/QML interface
- ✅ Thread-safe packet capture
- ✅ Linux Cooked Capture (SLL2) support for "any" interface

## Requirements

### Linux
- Qt5 (qt5-default or qtbase5-dev)
- Qt5 Widgets
- Qt5 Quick
- Qt5 QuickControls2
- Qt5 Network
- Qt5 QML
- libpcap-dev
- qmake
- g++ or clang++

### Android
- Qt5 for Android
- Android NDK
- Android SDK
- Qt Creator (recommended for easy deployment)

## Building

### Linux

1. Install dependencies (Ubuntu/Debian):
```bash
sudo apt-get update
sudo apt-get install qt5-default libqt5network5 libpcap-dev build-essential
```

2. Build the project:
```bash
qmake sniffer.pro
make
```

3. Run (requires root/sudo for packet capture):
```bash
sudo ./NetworkSniffer
```

### Android

1. Open Qt Creator
2. File → Open File or Project → Select `sniffer.pro`
3. Configure Android Kit (ARM or x86 depending on target)
4. Build → Build Project
5. Deploy to device or emulator

**Note:** Android has security restrictions on raw packet capture. The application uses a simplified capture method for Android that may require root access or special permissions.

## Usage

1. **Select Interface**: Choose a network interface from the dropdown (automatically detected)
   - Linux: Shows all available interfaces with IP addresses
   - Option to capture on "any" interface (captures from all interfaces simultaneously)
   - When using "any", the Interface column shows the actual interface for each packet
2. **Start Capture**: Click "Start" to begin capturing packets
3. **View Packets**: Captured packets appear in the table with (newest on top):
   - Packet number
   - Unix timestamp (seconds with 6 decimal precision)
   - **Interface name** (actual interface where packet was captured)
   - Source IP:Port
   - Destination IP:Port
   - Protocol type
   - Packet length
   - Additional info
4. **Inspect Details**: Click on any packet to see:
   - Complete packet information
   - Hex dump of raw data
5. **Stop Capture**: Click "Stop" to stop capturing
6. **Clear**: Click "Clear" to remove all captured packets from the view
7. **Save to PCAP**: Click "Save to PCAP" to export captured packets
   - Standard PCAP format
   - Compatible with Wireshark, tcpdump, and other tools
   - Preserves timing information and raw packet data

## Architecture

### Core Components

- **main.qml** (`main.qml`)
  - Qt Quick/QML declarative UI
  - ApplicationWindow with MenuBar and ToolBar
  - ListView for packet display
  - SplitView for packet list and details
  - Binds to C++ controller properties

- **MainWindow** (`mainwindow.h/cpp`)
  - QObject-based controller (not QMainWindow)
  - Exposes Q_PROPERTY for QML binding
  - Handles user interactions via Q_INVOKABLE methods
  - Manages packet display and file export
  - Auto-detects network interfaces using QNetworkInterface

- **PacketSniffer** (`packetsniffer.h/cpp`)
  - Core packet capture engine
  - Linux: Uses libpcap for raw packet capture in separate thread
  - Android: Uses QUdpSocket for basic network monitoring
  - Registers custom PacketInfo metatype for thread-safe signals

- **PacketModel** (`packetmodel.h/cpp`)
  - QAbstractListModel for displaying packets in QML
  - Custom role names for QML delegates
  - Efficient data management
  - Provides interface for packet retrieval and export

- **CaptureThread** (`packetsniffer.cpp`, Linux only)
  - Background thread for packet capture
  - Parses Ethernet, IP, TCP, UDP, ICMP, and ARP packets
  - Non-blocking capture with libpcap

### Data Flow

```
Network Interface → libpcap/Socket → CaptureThread/Sniffer 
    → PacketInfo (signal) → MainWindow (Controller) → PacketModel
                                      ↓                     ↓
                                  PCAP Export          QML ListView
```

### Qt Quick Architecture

```
QGuiApplication
    ↓
QQmlApplicationEngine
    ↓
main.qml (UI Layer)
    ↓
MainWindow (Controller)
    ↓
PacketModel & PacketSniffer (Data Layer)
```

## Permissions

### Linux
Packet capture requires root privileges or CAP_NET_RAW capability:
```bash
# Run with sudo
sudo ./NetworkSniffer

# Or set capabilities (preferred)
sudo setcap cap_net_raw,cap_net_admin=eip ./NetworkSniffer
```

### Android
Add to AndroidManifest.xml:
```xml
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
```

## Project Structure

```
sniffer/
├── sniffer.pro           # Qt project file
├── main.cpp              # Application entry point (QML setup)
├── main.qml              # Qt Quick UI (declarative)
├── mainwindow.h/cpp      # Controller (QObject-based)
├── packetsniffer.h/cpp   # Packet capture engine
├── packetmodel.h/cpp     # Data model for QML ListView
├── resources.qrc         # Qt resources file
├── README.md             # This file
└── QML_MIGRATION.md      # Qt Quick migration guide
```

## Limitations

- Android version has limited packet capture capabilities due to platform restrictions
- Some protocols may not be fully parsed (limited to TCP, UDP, ICMP, ARP)
- Packet capture is limited to accessible interfaces
- Android requires root for full raw socket access
- Large packet captures may consume significant memory

## Future Enhancements

- [ ] Packet filtering (BPF filters)
- [ ] More protocol parsers (HTTP, DNS, DHCP, etc.)
- [ ] Network statistics and graphs
- [ ] Full Android raw socket support with root
- [ ] Packet search and filtering in GUI
- [ ] Color coding by protocol type
- [ ] Follow TCP stream functionality
- [ ] Performance optimizations for large captures

## Troubleshooting

### No packets captured
- Ensure you're running with sudo/root privileges
- Check that the selected interface is active
- Verify network traffic exists on the interface

### Permission denied error
```bash
sudo ./NetworkSniffer
# or
sudo setcap cap_net_raw,cap_net_admin=eip ./NetworkSniffer
```

### Build errors
- Ensure all Qt5 dependencies are installed
- Verify libpcap-dev is installed on Linux
- Clean build: `make clean && qmake && make`

## Contributing

This is an educational project. Feel free to fork and enhance it!

## License

Educational/Personal use. This project is for learning purposes to demonstrate Qt5 cross-platform development and network programming concepts.

## Acknowledgments

- Built with Qt5 framework
- Uses libpcap for packet capture on Linux
- Inspired by Wireshark and tcpdump

## Author

Created for educational purposes to demonstrate:
- Qt5 cross-platform GUI development
- Network programming with libpcap
- Thread-safe programming with Qt signals/slots
- PCAP file format implementation
