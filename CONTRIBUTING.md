# Contributing to FRIDAY

First off, thank you for considering contributing to FRIDAY! It's people like you that make this DeskBuddy awesome.

## 📜 Code of Conduct
By participating in this project, you are expected to uphold a welcoming, inclusive, and harassment-free environment. Please be respectful to all contributors.

## 🛠️ How Can I Contribute?

### Reporting Bugs
If you find a bug, please create an issue and include:
- A clear descriptive title.
- Steps to reproduce the bug.
- Expected behavior vs actual behavior.
- Your hardware setup (ESP32 variant, display type).

### Suggesting Enhancements
Have an idea for a new feature (e.g., a new app for the radial menu)? Open an issue describing your idea, how it would look, and how it aligns with the Iron-Man HUD aesthetic.

### Submitting Pull Requests
1. **Fork the repository** and create your branch from `main`.
2. **Ensure your code follows the existing style**:
   - Use modular design (put new apps in their own folders inside `src/`).
   - Follow the established UI theme (Cyan, Blue, White, Black).
3. **Test your changes** on actual hardware if possible.
4. **Issue a Pull Request**: Provide a detailed description of the changes you made. Include screenshots or GIFs if you changed the UI.

## 💻 Development Setup
1. Install [PlatformIO](https://platformio.org/).
2. Clone your fork of the repository.
3. Open the project folder in VS Code with the PlatformIO extension.
4. Build and upload to your ESP32 board (`env:esp32`).

Thank you for helping make FRIDAY the ultimate DeskBuddy!
