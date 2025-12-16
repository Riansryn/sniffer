# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-12-16

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

## [Unreleased]

### Planned Features
- BPF packet filtering
- More protocol parsers (HTTP, DNS, DHCP)
- Network statistics and graphs
- Packet search functionality
- Color coding by protocol
- Follow TCP stream
- Performance optimizations for large captures

---

## Version History Format

- **Added** for new features
- **Changed** for changes in existing functionality
- **Deprecated** for soon-to-be removed features
- **Removed** for now removed features
- **Fixed** for any bug fixes
- **Security** in case of vulnerabilities
