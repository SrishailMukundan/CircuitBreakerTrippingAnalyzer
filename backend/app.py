from flask import Flask, request, jsonify, render_template
from flask_cors import CORS
import time
import math
import os

# Point Flask to the 'frontend' directory for HTML assets
# This looks up one level from 'backend' and enters 'frontend'
frontend_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'frontend'))

app = Flask(__name__, 
            template_folder=frontend_dir, 
            static_folder=frontend_dir)
CORS(app)

telemetry_history = []
breaker_data = {
    "current_amps": 0.0,
    "status": "UNKNOWN",
    "total_packets_received": 0,
    "rolling_5min_average": 0.0,
    "rolling_5min_p95": 0.0,
    "rolling_5min_peak_spike": 0.0,
    "rolling_packet_count": 0
}

def clean_and_calculate():
    global telemetry_history, breaker_data
    current_time = time.time()
    telemetry_history = [r for r in telemetry_history if current_time - r[0] <= 300]
    
    if not telemetry_history:
        breaker_data["rolling_5min_average"] = 0.0
        breaker_data["rolling_5min_p95"] = 0.0
        breaker_data["rolling_5min_peak_spike"] = 0.0
        breaker_data["rolling_packet_count"] = 0
        return

    amps_values = [r[1] for r in telemetry_history]
    breaker_data["rolling_packet_count"] = len(amps_values)
    breaker_data["rolling_5min_average"] = round(sum(amps_values) / len(amps_values), 2)
    breaker_data["rolling_5min_peak_spike"] = round(max(amps_values), 2)
    
    sorted_amps = sorted(amps_values)
    idx = math.ceil(len(sorted_amps) * 0.95) - 1
    idx = max(0, min(idx, len(sorted_amps) - 1))
    breaker_data["rolling_5min_p95"] = round(sorted_amps[idx], 2)

@app.route('/')
def serve_dashboard():
    """Serves the intuitive dashboard directly on the root URL."""
    return render_template('index.html')

@app.route('/api/update_esp32', methods=['POST'])
def update_esp32():
    global breaker_data
    data = request.get_json(silent=True)
    if not data:
        return jsonify({"status": "error"}), 400
    
    current = data.get("current", 0.0)
    telemetry_history.append((time.time(), current))
    clean_and_calculate()
    
    breaker_data["current_amps"] = round(current, 2)
    breaker_data["total_packets_received"] += 1
    breaker_data["status"] = "CRITICAL" if current > 12.0 else "NORMAL"
    return jsonify({"status": "success"}), 200

@app.route('/api/stats', methods=['GET'])
def get_stats():
    clean_and_calculate()
    return jsonify(breaker_data), 200

@app.route('/api/history', methods=['GET'])
def get_history():
    clean_and_calculate()
    if not telemetry_history:
        return jsonify({"timestamps": [time.time()], "values": [0.0]}), 200
    return jsonify({
        "timestamps": [r[0] for r in telemetry_history],
        "values": [r[1] for r in telemetry_history]
    }), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)