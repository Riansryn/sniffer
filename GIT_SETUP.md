# Git Repository Setup Guide

This guide will help you initialize and set up this project as a Git repository.

## Quick Start

### 1. Initialize Git Repository

```bash
cd /home/flavio/draganfly/estudos/sniffer
git init
```

### 2. Add Files to Repository

```bash
# Add source files
git add *.cpp *.h *.ui *.pro *.qrc
git add README.md .gitignore

# Verify what will be committed
git status
```

### 3. Create Initial Commit

```bash
git commit -m "Initial commit: Qt5 Network Sniffer with PCAP export"
```

## Setting Up Remote Repository

### GitHub

```bash
# Create repository on GitHub first, then:
git remote add origin https://github.com/YOUR_USERNAME/network-sniffer.git
git branch -M main
git push -u origin main
```

### GitLab

```bash
# Create repository on GitLab first, then:
git remote add origin https://gitlab.com/YOUR_USERNAME/network-sniffer.git
git branch -M main
git push -u origin main
```

### Bitbucket

```bash
# Create repository on Bitbucket first, then:
git remote add origin https://bitbucket.org/YOUR_USERNAME/network-sniffer.git
git branch -M main
git push -u origin main
```

## Files Included in Repository

The following source files will be tracked:

- `main.cpp` - Application entry point
- `mainwindow.h/cpp/ui` - Main window implementation
- `packetsniffer.h/cpp` - Packet capture engine
- `packetmodel.h/cpp` - Data model for packet display
- `sniffer.pro` - Qt project file
- `resources.qrc` - Qt resources
- `README.md` - Project documentation
- `.gitignore` - Git ignore rules

## Files Excluded (via .gitignore)

Build artifacts and generated files are automatically excluded:

- Object files (*.o)
- MOC generated files (moc_*.cpp, moc_*.h)
- UI generated files (ui_*.h)
- QRC generated files (qrc_*.cpp)
- Compiled executables (NetworkSniffer)
- Build directories (build-*, debug/, release/)
- Qt Creator files (*.pro.user)
- Makefiles
- Temporary files

## Useful Git Commands

### Check Status
```bash
git status
```

### View Changes
```bash
git diff
```

### Add Changes
```bash
git add <filename>
# or add all changes
git add .
```

### Commit Changes
```bash
git commit -m "Description of changes"
```

### View History
```bash
git log --oneline
```

### Create a Branch
```bash
git checkout -b feature/new-feature
```

### Push Changes
```bash
git push origin main
```

## Recommended .git/config

After setting up your remote, your `.git/config` should look like:

```ini
[core]
    repositoryformatversion = 0
    filemode = true
    bare = false
    logallrefupdates = true
[remote "origin"]
    url = https://github.com/YOUR_USERNAME/network-sniffer.git
    fetch = +refs/heads/*:refs/remotes/origin/*
[branch "main"]
    remote = origin
    merge = refs/heads/main
```

## Cleaning Build Artifacts

Before committing, ensure build artifacts are removed:

```bash
make clean
rm -f *.o moc_* ui_* qrc_* NetworkSniffer
```

## Creating a Release

### Tag a Version
```bash
git tag -a v1.0 -m "Version 1.0 - Initial release with PCAP export"
git push origin v1.0
```

### Create Release Archive
```bash
git archive --format=tar.gz --prefix=network-sniffer-1.0/ v1.0 > network-sniffer-1.0.tar.gz
```

## Tips

1. **Always commit source files only** - Never commit build artifacts
2. **Write meaningful commit messages** - Describe what changed and why
3. **Commit often** - Make small, logical commits
4. **Test before committing** - Ensure the code builds and runs
5. **Use branches** - For new features or experimental changes

## Suggested Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

Example:
```
feat: Add PCAP file export functionality

- Implemented writePcapFile() method
- Added Save to PCAP button in UI
- Store raw packet data and timestamps
- Files compatible with Wireshark

Resolves #1
```

Common types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting)
- `refactor`: Code refactoring
- `test`: Adding tests
- `chore`: Maintenance tasks
