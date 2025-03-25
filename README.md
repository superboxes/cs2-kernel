# 🎯 CS2 External Cheat | Tactical Advantage

![CS2 Cheat Showcase](./imgs/tut.gif)

## 📋 Table of Contents

- [Overview](#-cheat-info)
- [Features](#-features)
  - [ESP](#%EF%B8%8F-esp)
  - [Triggerbot](#-triggerbot)
- [Configuration](#%EF%B8%8F-configuration)
- [Quick Setup](#-quick-setup)
- [Build Instructions](#-build-guide)
- [Customization](#-customization)
- [Disclaimer](#%EF%B8%8F-disclaimer)
- [Contributions](#-community--contributions)

## 💥 Cheat info

This is an external cheat for Counter-Strike 2 that provides various ESP and triggerbot features. The cheat uses a kernel driver to read memory for low-detection functionality.

## 🚀 Features

### 👁 ESP

- **Box ESP**: Render enemies and teammates with surgical precision
  - Fully customizable box colors
- **Skeleton ESP**: Reveal hidden player structures
  - Expose enemy movements
  - Optional skeletal rendering
- **Tracking**
  - Head tracking
  - Render distance control
  - Team ESP toggling

### 🎯 Triggerbot

- **Precision Activation**
  - Configurable trigger mechanics
  - Adjustable reaction delay
  - Team check protection
  - Custom activation key

## ⚙️ Configuration

Full customization through `config.json`:

```json
{
  "esp_box_color_enemy": [225, 75, 75],
  "esp_box_color_team": [75, 175, 75],
  "esp_distance_color": [75, 75, 175],
  "esp_name_color": [75, 75, 175],
  "esp_skeleton_color_enemy": [225, 75, 75],
  "esp_skeleton_color_team": [75, 175, 75],
  "flag_render_distance": 200,
  "render_distance": -1.0,
  "show_box_esp": true,
  "show_extra_flags": false,
  "show_head_tracker": false,
  "show_skeleton_esp": false,
  "team_esp": false,
  "triggerbot_delay": 10,
  "triggerbot_enabled": true,
  "triggerbot_key": 5,
  "triggerbot_team_check": true
}
```

## 🚀 Quick Setup

1. Map the kernel driver (you can use [kdmapper](https://github.com/TheCruZ/kdmapper))
2. Launch CS2
3. Activate the cheat (run the executable)
4. Press END to exit

## 🛡️ Customization Playground

Modify `config.json` to fine-tune:

- ESP color schemes
- Render distances
- Triggerbot stuff

## 🛠️ Build Guide

Both the cheat executable and kernel driver are built with [Visual Studio](https://visualstudio.microsoft.com/).

## ⚠️ Disclaimer

**EDUCATIONAL PURPOSE ONLY**

- Respect game integrity
- Understand potential consequences
- Use responsibly

## 🤝 Community & Contributions

Passionate about pushing boundaries?

- Open issues
- Submit pull requests
