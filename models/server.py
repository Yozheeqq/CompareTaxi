from flask import Flask, request, jsonify
from catboost import CatBoostRegressor
import numpy as np

app = Flask(__name__)

model = CatBoostRegressor()
model.load_model("model_one_price.bin")

@app.route('/predict', methods=['POST'])
def predict():
    try:
        data = request.get_json()
        features = data.get('features')

        if features is None:
            return jsonify({'error': 'Missing "features" field in request'}), 400

        input_array = np.array(features)
        prediction = model.predict(input_array)

        return jsonify({'prediction': prediction.tolist()})
    except Exception as e:
        return jsonify({'error': str(e)}), 500

# Запуск сервера
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
