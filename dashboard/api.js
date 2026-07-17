// api.js - Complete Memoir API Client

const API_BASE = "http://127.0.0.1:5000";

window.API = {

    async health() {
        const r = await fetch(`${API_BASE}/health`);
        return await r.json();
    },

    async transcribe(audioBlob) {
        const form = new FormData();
        form.append("audio", audioBlob, "recording.wav");
        const r = await fetch(`${API_BASE}/transcribe`, {
            method: "POST",
            body: form
        });
        if (!r.ok) {
            throw new Error(await r.text());
        }
        return await r.json();
    },

    // ===== Chat with Groq (Text) =====
    async chat(text, history = []) {
        const r = await fetch(`${API_BASE}/chat`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ text, history })
        });
        if (!r.ok) {
            throw new Error(await r.text());
        }
        return await r.json();
    },

    // ===== Full Voice Assistant (STT → LLM) =====
    async assistant(audioBlob) {
        const form = new FormData();
        form.append("audio", audioBlob, "recording.wav");
        const r = await fetch(`${API_BASE}/assistant`, {
            method: "POST",
            body: form
        });
        if (!r.ok) {
            throw new Error(await r.text());
        }
        return await r.json();
    },

    // ===== Groq Status =====
    async groqStatus() {
        const r = await fetch(`${API_BASE}/groq-status`);
        return await r.json();
    },

    // ===== Chat History =====
    async getHistory() {
        const r = await fetch(`${API_BASE}/history`);
        return await r.json();
    },

    async clearHistory() {
        const r = await fetch(`${API_BASE}/history/clear`, { method: "POST" });
        return await r.json();
    },

    // ===== Text-to-Speech (Browser) =====
    async speak(text, options = {}) {
        return new Promise((resolve, reject) => {
            if (!window.speechSynthesis) {
                reject(new Error("Speech synthesis not supported"));
                return;
            }

            // Cancel any ongoing speech
            window.speechSynthesis.cancel();

            const utterance = new SpeechSynthesisUtterance(text);
            utterance.lang = options.lang || 'en-US';
            utterance.rate = options.rate || 1.0;
            utterance.pitch = options.pitch || 1.0;
            utterance.volume = options.volume || 1.0;

            // Try to get a male voice
            const voices = window.speechSynthesis.getVoices();
            const maleVoice = voices.find(v => v.lang.startsWith('en') && v.name.includes('Male'));
            if (maleVoice) utterance.voice = maleVoice;

            utterance.onend = () => resolve();
            utterance.onerror = (e) => reject(e);

            window.speechSynthesis.speak(utterance);
        });
    }
};