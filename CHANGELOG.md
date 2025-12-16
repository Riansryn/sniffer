# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.1.0] - 2024-12-16

### Added
- **Interface Column**: New column in packet table showing the network interface where each packet was captured
- **Timestamp as Float**: Unix epoch timestamp with microsecond precision (6 decimal places)
- **Linux Cooked Capture Support**: Proper handling of "any" interface with SLL2 format
- **Real Interface Names**: When capturing on "any", displays actual interface name (e.g., wlp0s20f3, eth0, lo) instead of "any"

### Changed
- **Packet Display Order**: Latest packets now appear at the TOP of the table (reversed order)
- **Table Layout**: Updated column widths and added Interface column
  - No. (60px) | Time (s) (120px) | Interface (100px) | Source (150px) | Destination (150px) | Protocol (80px) | Length (80px) | Info (flexible)
- **Timestamp Format**: Changed from "hh:mm:ss.zzz" to floating-point seconds (e.g., 1734378443.456789)
- **Application Class**: Changed from QGuiApplication to QApplication for Qt Widgets support

### Fixed
- **QFileDialog Error**: Fixed "Cannot create a QWidget without QApplication" crash when saving PCAP files
- **Interface Detection**: Proper extraction of interface name from SLL2 header when using "any" interface
- **Packet Parsing**: Corrected header offset calculation for Linux cooked capture vs Ethernet frames

### Technical Details
- Added `timestampSecs` (double) and `interfaceName` (QString) fields to PacketInfo
- Implemented SLL2 header parsing with if_indextoname() for interface resolution
- Added pcap_datalink() detection for DLT_LINUX_SLL and DLT_LINUX_SLL2
- Changed PacketModel to insert at row 0 (prepend) instead of appending
- Includes: `<pcap/sll.h>` and `<net/if.h>` for Linux cooked capture support

## [2.0.0] - 2024-12-16

### Added
- Initial release of Network Sniffer Qt5 application
- Real-time packet capture using libpcap (Linux) and QUdpSocket (Android)
- Automatic network interface detection with QNetworkInterface
- Table view displaying captured packets with:
  - Packet number
  - Timestamp
  - Source IP:Port
  - Destination IP:Port
  - Protocol (TCP, UDP, ICMP, ARP)
  - Packet length
  - Additional information
- Detailed packet view with hex dump
- PCAP file export functionality (Wireshark compatible)
- Thread-safe packet capture with separate capture thread
- Qt metatype registration for PacketInfo structure
- Cross-platform support (Linux and Android)
- Clean and intuitive GUI with Qt Widgets

### Features
- **Capture Controls**: Start, Stop, Clear buttons
- **Interface Selection**: Dropdown with auto-detected interfaces
- **Live Display**: Real-time packet table updates
- **Packet Details**: Click any packet to view full details
- **PCAP Export**: Save captured packets to standard PCAP format
- **Protocol Parsing**: Ethernet, IPv4, TCP, UDP, ICMP, ARP

### Technical Details
- Built with Qt 5.15.2
- Uses libpcap for Linux packet capture
- QDataStream for PCAP file writing
- Qt Signals/Slots for thread communication
- MVC pattern with QAbstractTableModel

### Documentation
- Comprehensive README with setup instructions
- Git setup guide (GIT_SETUP.md)
- MIT License with educational notice
- Troubleshooting section
- Architecture documentation

### Build System
- qmake project file with platform detection
- Platform-specific defines (LINUX_PLATFORM, ANDROID_PLATFORM)
- Automatic MOC, UIC, and RCC integration
- Proper .gitignore for Qt projects

## [2.0.0] - 2024-12-16

### Changed - Major Refactoring to Qt Quick/QML
- **UI Framework**: Migrated from Qt Widgets to Qt Quick/QML
  - Replaced mainwindow.ui (XML) with main.qml (declarative)
  - Modern, fluid user interface
  - Better suited for cross-platform deployment
  
- **Architecture**: Refactored to Model-View-Controller pattern
  - MainWindow changed from QMainWindow to QObject controller
  - Exposes Q_PROPERTY for QML data binding
  - Q_INVOKABLE methods for QML interaction
  
- **Data Model**: Updated PacketModel
  - Changed from QAbstractTableModel to QAbstractListModel
  - Custom role names for QML delegates
  - More efficient QML integration
  
- **Application Entry**: Updated main.cpp
  - Uses QGuiApplication instead of QApplication
  - QQmlApplicationEngine for QML loading
  - Context properties for controller access

### Added
- main.qml - Complete declarative UI definition
- QML_MIGRATION.md - Detailed migration documentation
- Custom QML components (HeaderCell, TableCell)
- Property bindings for reactive UI updates

### Technical Details
- **Qt Modules**: Added qml, quick, quickcontrols2
- **Build System**: Updated sniffer.pro
- **Code Style**: Declarative UI with imperative business logic
- All existing features preserved (capture, export, interface detection)

### Benefits
- More maintainable codebase
- Better separation of concerns
- Improved cross-platform consistency
- Foundation for mobile optimization
- Hardware-accelerated rendering
- Hot reload capability for UI development

## [Unreleased]

### Planned Features
- BPF packet filtering
- More protocol parsers (HTTP, DNS, DHCP)
- Network statistics and graphs
- Packet search functionality
- Color coding by protocol
- Follow TCP stream
- Performance optimizations for large captures
- Qt Quick animations and transitions

---

## Version History Format

- **Added** for new features
- **Changed** for changes in existing functionality
- **Deprecated** for soon-to-be removed features
- **Removed** for now removed features
- **Fixed** for any bug fixes
- **Security** in case of vulnerabilities
