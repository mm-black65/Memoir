# app.py - Memoir Backend with Groq AI + Hugging Face STT
import os
import uuid
import json
import requests
from flask import Flask, request, jsonify
from flask_cors import CORS
from dotenv import load_dotenv

load_dotenv()

app = Flask(__name__)
CORS(app)

# Create temp folder
os.makedirs('temp', exist_ok=True)

# ===== Configuration =====
GROQ_API_KEY = os.getenv('GROQ_API_KEY', '')
GROQ_API_URL = "https://api.groq.com/openai/v1/chat/completions"
# Option 1: Llama 3.1 (latest, best quality)
GROQ_MODEL = "llama-3.1-8b-instant"

HF_STT_URL = "https://api-inference.huggingface.co/models/openai/whisper-base"

CHAT_HISTORY_FILE = 'chat_history.json'

# ===== Helper Functions =====

def load_chat_history():
    try:
        with open(CHAT_HISTORY_FILE, 'r') as f:
            return json.load(f)
    except:
        return []

def save_chat_history(history):
    with open(CHAT_HISTORY_FILE, 'w') as f:
        json.dump(history, f, indent=2)

def call_groq(user_message, history=None):
    if not GROQ_API_KEY:
        return "⚠️ Groq API key not configured. Please add GROQ_API_KEY to .env file."
    
    messages = [
        {"role": "system", "content": "You are Memoir, a helpful AI voice assistant. Keep responses concise (under 100 words)."}
    ]
    
    if history:
        for msg in history[-10:]:
            messages.append({"role": "user", "content": msg.get('user', '')})
            messages.append({"role": "assistant", "content": msg.get('assistant', '')})
    
    messages.append({"role": "user", "content": user_message})
    
    headers = {
        "Authorization": f"Bearer {GROQ_API_KEY}",
        "Content-Type": "application/json"
    }
    
    payload = {
        "model": GROQ_MODEL,
        "messages": messages,
        "temperature": 0.7,
        "max_tokens": 300
    }
    
    try:
        response = requests.post(GROQ_API_URL, json=payload, headers=headers)
        if response.status_code != 200:
            return f"⚠️ Groq error: {response.json().get('error', {}).get('message', 'Unknown error')}"
        return response.json()['choices'][0]['message']['content']
    except Exception as e:
        return f"⚠️ Error: {str(e)}"

# ===== Routes =====

@app.route('/health', methods=['GET'])
def health():
    return jsonify({
        'status': 'ok',
        'provider': 'huggingface',
        'groq': 'enabled' if GROQ_API_KEY else 'disabled',
        'message': 'Memoir backend is running!'
    })

@app.route('/transcribe', methods=['POST'])
def transcribe():
    try:
        if 'audio' not in request.files:
            return jsonify({'error': 'No audio file provided'}), 400
        
        audio_file = request.files['audio']
        if audio_file.filename == '':
            return jsonify({'error': 'Empty filename'}), 400
        
        file_id = str(uuid.uuid4())
        file_path = f"temp/{file_id}_{audio_file.filename}"
        audio_file.save(file_path)
        
        with open(file_path, 'rb') as f:
            audio_data = f.read()
        
        response = requests.post(HF_STT_URL, headers={"Content-Type": "audio/wav"}, data=audio_data)
        os.remove(file_path)
        
        if response.status_code != 200:
            return jsonify({'error': response.json().get('error', 'Unknown error')}), response.status_code
        
        return jsonify({
            'text': response.json().get('text', ''),
            'provider': 'huggingface',
            'status': 'success'
        })
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/chat', methods=['POST'])
def chat():
    try:
        data = request.get_json()
        if not data or 'text' not in data:
            return jsonify({'error': 'Missing "text" field'}), 400
        
        user_text = data['text'].strip()
        if not user_text:
            return jsonify({'error': 'Empty message'}), 400
        
        history = data.get('history', [])
        assistant_text = call_groq(user_text, history)
        
        chat_history = load_chat_history()
        chat_history.append({'user': user_text, 'assistant': assistant_text, 'timestamp': int(os.times().elapsed)})
        save_chat_history(chat_history)
        
        return jsonify({'response': assistant_text, 'user_text': user_text, 'history': chat_history[-10:]})
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/assistant', methods=['POST'])
def assistant():
    try:
        if 'audio' not in request.files:
            return jsonify({'error': 'No audio file provided'}), 400
        
        audio_file = request.files['audio']
        if audio_file.filename == '':
            return jsonify({'error': 'Empty filename'}), 400
        
        file_id = str(uuid.uuid4())
        file_path = f"temp/{file_id}_{audio_file.filename}"
        audio_file.save(file_path)
        
        with open(file_path, 'rb') as f:
            audio_data = f.read()
        
        stt_response = requests.post(HF_STT_URL, headers={"Content-Type": "audio/wav"}, data=audio_data)
        os.remove(file_path)
        
        if stt_response.status_code != 200:
            return jsonify({'error': 'STT failed'}), stt_response.status_code
        
        user_text = stt_response.json().get('text', '')
        if not user_text:
            return jsonify({'error': 'No speech detected'}), 400
        
        assistant_text = call_groq(user_text, [])
        
        chat_history = load_chat_history()
        chat_history.append({'user': user_text, 'assistant': assistant_text, 'timestamp': int(os.times().elapsed)})
        save_chat_history(chat_history)
        
        return jsonify({'text': assistant_text, 'user_text': user_text, 'provider': 'groq', 'status': 'success'})
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/groq-status', methods=['GET'])
def groq_status():
    if not GROQ_API_KEY:
        return jsonify({'status': 'disabled', 'message': 'GROQ_API_KEY not configured'})
    return jsonify({'status': 'online', 'model': GROQ_MODEL, 'message': 'Groq API is working!'})

@app.route('/history', methods=['GET'])
def get_history():
    return jsonify(load_chat_history())

@app.route('/history/clear', methods=['POST'])
def clear_history():
    save_chat_history([])
    return jsonify({'message': 'Chat history cleared'})

if __name__ == '__main__':
    print("═" * 50)
    print("📝 MEMOIR BACKEND (Groq + Hugging Face)")
    print("═" * 50)
    print(f"Groq API: {'✅ Enabled' if GROQ_API_KEY else '❌ Disabled'}")
    print("\nStarting server on http://localhost:5000")
    print("═" * 50)
    app.run(host='0.0.0.0', port=5000, debug=True)