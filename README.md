<div align="center">

# 📝 Memoir
### Your AI-Powered Voice Notebook

*One tap to record • Double tap to transcribe • Long press to talk with AI*

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![Platform](https://img.shields.io/badge/Platform-ESP32--C3-blueviolet)
![PWA](https://img.shields.io/badge/PWA-Installable-success)
![Python](https://img.shields.io/badge/Python-3.11+-blue)

---

*"Your ideas are waiting for you."*

</div>

# 📖 Overview

Memoir is an **AI-powered smart voice pen** built around the **ESP32-C3**. It allows you to instantly capture thoughts, transcribe them into text using AI, and even converse with an AI assistant.

Designed with an **offline-first architecture**, Memoir combines embedded systems, BLE communication, a Progressive Web App, and modern AI services into one seamless experience.

---

# ✨ Features

## 🎙️ Voice Recording

- Single tap to start/stop recording
- High-quality I2S MEMS microphone
- Automatic recording storage on MicroSD card
- Haptic and LED feedback

---

## 📝 AI Transcription

- Double tap to transcribe
- Powered by **Whisper**
- Beautiful dark-mode transcript viewer
- Works directly from the PWA

---

## 🤖 AI Voice Assistant

- Long press to activate
- Chat naturally with AI
- Powered by **Groq Llama 3**
- Voice responses using Web Speech API

---

## 📱 Progressive Web App

- Install like a native mobile app
- Works offline
- IndexedDB storage
- BLE connectivity
- Live notifications
- Apple Notes inspired interface

---

## 🔋 Power Features

- Deep Sleep (<5µA)
- Touch wake-up
- USB-C charging
- 400mAh LiPo battery

---

# 🏗️ System Architecture

```text
                        MEMOIR ECOSYSTEM

              BLE                       WiFi
      ┌────────────────┐          ┌─────────────────┐
      │                │          │                 │
      │   ESP32-C3     │──────────│       PWA       │
      │   Smart Pen    │          │ (Phone/Desktop) │
      │                │          │                 │
      └───────┬────────┘          └────────┬────────┘
              │                            │
              │                            │
      ┌───────▼────────┐          ┌────────▼────────┐
      │   SD Card      │          │   IndexedDB     │
      │ Audio Storage  │          │ Offline Storage │
      └────────────────┘          └────────┬────────┘
                                           │
                                           │ HTTP
                                           ▼
                                   ┌─────────────────┐
                                   │ Flask Backend   │
                                   │ Whisper + Groq  │
                                   └─────────────────┘
```

---

# 🎮 Gesture Controls

| Gesture | Action |
|---------|--------|
| 📌 Right-side Up + Single Tap | Start / Stop Recording |
| 📌 Right-side Up + Double Tap | Transcribe |
| 🔄 Upside Down + Single Tap | Play Last Recording |
| 🔄 Upside Down + Double Tap | Delete Recording |
| ⏱️ Long Press | AI Assistant |

---

# 🛠️ Tech Stack

## Hardware

| Component | Model |
|-----------|-------|
| MCU | ESP32-C3 |
| Microphone | INMP441 |
| Amplifier | MAX98357 |
| Speaker | 8Ω 1W |
| IMU | MPU6050 |
| Touch Sensor | TTP223 |
| Storage | MicroSD SPI |
| Battery | 400mAh LiPo |
| Charging | TP4056 |
| LED | WS2812B NeoPixel |

---

## Software

| Layer | Technology |
|--------|------------|
| Embedded | C++, Arduino Framework, PlatformIO |
| Backend | Python, Flask |
| Frontend | HTML, CSS, JavaScript |
| Database | IndexedDB |
| Communication | BLE + WiFi |
| AI | Whisper + Groq Llama 3 |

---

# 📂 Project Structure

```text
Memoir
│
├── dashboard/
│   ├── index.html
│   ├── api.js
│   ├── manifest.json
│   └── sw.js
│
├── memoir-backend/
│   ├── app.py
│   ├── requirements.txt
│   └── .env
│
├── src/
│   ├── main.cpp
│   └── Config.h
│
├── lib/
│   ├── AudioManager/
│   ├── GestureManager/
│   ├── FileManager/
│   ├── VirtualButton/
│   └── ...
│
├── docs/
├── examples/
├── LICENSE
└── README.md
```

---

# 🚀 Getting Started

## Clone Repository

```bash
git clone https://github.com/mm-black65/Memoir.git

cd Memoir
```

---

## ESP32 Firmware

```bash
pio run -t upload
```

---

## Backend

```bash
cd memoir-backend

python -m venv venv

source venv/bin/activate
# Windows
venv\Scripts\activate

pip install -r requirements.txt
```

Create a `.env`

```env
GROQ_API_KEY=your_api_key
```

Run

```bash
python app.py
```

---

## PWA

```bash
cd dashboard

python -m http.server 8080
```

Open

```
http://localhost:8080
```

---

# 🌐 Deployment

## Backend

Deploy easily on **Render**

```bash
Build Command:
pip install -r requirements.txt

Start Command:
python app.py
```

---

## Frontend

Deploy using **Vercel**

```bash
npm install -g vercel

vercel
```

---

# 🔌 Backend API

| Endpoint | Method | Purpose |
|-----------|--------|---------|
| `/health` | GET | Server status |
| `/transcribe` | POST | Whisper STT |
| `/chat` | POST | AI chat |
| `/assistant` | POST | Voice Assistant |
| `/providers` | GET | Available AI providers |
| `/history` | GET | Chat history |
| `/history/clear` | POST | Clear history |

---

# 📊 Project Status

| Module | Progress |
|---------|----------|
| UI/UX | ✅ 100% |
| BLE Integration | ✅ 100% |
| IndexedDB | ✅ 100% |
| Offline Sync | ✅ 100% |
| Whisper Integration | ✅ 100% |
| AI Assistant | ✅ 100% |
| Hardware Assembly | 🟡 30% |
| 3D Enclosure | ⚪ 0% |

---

# 💰 Bill of Materials

| Component | Cost |
|-----------|------|
| ESP32-C3 | ₹250 |
| INMP441 | ₹250 |
| MAX98357 | ₹200 |
| Speaker | ₹80 |
| MPU6050 | ₹120 |
| TTP223 | ₹40 |
| MicroSD Module | ₹300 |
| NeoPixel | ₹50 |
| Vibration Motor | ₹80 |
| LiPo Battery | ₹300 |
| TP4056 | ₹50 |
| Misc Components | ₹400 |

### **Estimated Total**

# **≈ ₹2,500**

---

# 🤝 Contributing

1. Fork the repository

2. Create your feature branch

```bash
git checkout -b feature/amazing-feature
```

3. Commit changes

```bash
git commit -m "Added amazing feature"
```

4. Push

```bash
git push origin feature/amazing-feature
```

5. Open a Pull Request

---

# 🙏 Acknowledgements

- ESP32
- PlatformIO
- Hugging Face Whisper
- Groq
- Web Bluetooth API
- Flask

---

# 👨‍💻 Author

**Mahi Ahalawat**

B.Tech Robotics & AI  
JIIT Noida

GitHub: **[@mm-black65](https://github.com/mm-black65)**

---

<div align="center">

## ⭐ If you like this project, give it a star!

Made with ❤️, Embedded Systems, AI, and a lot of coffee ☕

</div>