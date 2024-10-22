import streamlit as st
import firebase_admin
from firebase_admin import credentials, db
import time

def initialize_firebase():
    if not firebase_admin._apps:
        cred = credentials.Certificate('forestFire.json')
        firebase_admin.initialize_app(cred, {
            'databaseURL': ''

        })

def get_forest_fire_data():
    ref = db.reference('/')  # Access root level to get all data
    data = ref.get()  # Fetch the data
    return data

# Initialize Firebase
initialize_firebase()

# Streamlit app content
st.set_page_config(page_title="Forest Fire Detection Dashboard", page_icon="🔥")
st.markdown(
    """
    <style>
    body {
        background-color: #FFFAF0; /* Light orange background */
    }
    .sensor-box {
        background-color: #FFF8DC; /* Light beige color for sensor boxes */
        border-radius: 10px;
        padding: 20px;
        margin: 20px;
        box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
        text-align: center;
    }
    .result-box {
        border-radius: 10px;
        padding: 30px;
        margin: 20px;
        box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
        text-align: center;
    }
    </style>
    """, unsafe_allow_html=True
)

st.title("Forest Fire Detection Dashboard")

# print(data)
placeholder = st.empty()

while True:

    data = get_forest_fire_data()

    with placeholder.container():
        if data:
            led_status = data['LED_STATUS']  # Fetch LED status from Firebase
            smoke_value = data['Smoke']  # Fetch smoke value from Firebase
            temperature = data['Temperature']  # Fetch temperature value directly

            # Display sensor values in boxes
            col1, col2, col3 = st.columns(3)
            with col1:
                st.markdown(f"<div class='sensor-box'>LED Status: {led_status}</div>", unsafe_allow_html=True)  # LED status box
            with col2:
                st.markdown(f"<div class='sensor-box'>Smoke Value: {smoke_value}</div>", unsafe_allow_html=True)  # Smoke value box
            with col3:
                st.markdown(f"<div class='sensor-box'>Temperature: {temperature} °C</div>", unsafe_allow_html=True)  # Temperature box

            # Fire detection logic
            SMOKE_THRESHOLD = 400
            TEMP_THRESHOLD= 40
            # Final Fire Detection Message
            if smoke_value > SMOKE_THRESHOLD or temperature> TEMP_THRESHOLD :
                result_message = "🔥 Fire Detected! Immediate action required!"
                st.markdown(f"<div class='result-box' style='background-color:red;'><span style='color:white;'>{result_message}</span></div>", unsafe_allow_html=True)
            else:
                result_message = "✅ No Fire Detected."
                st.markdown(f"<div class='result-box' style='background-color:green;'><span style='color:white;'>{result_message}</span></div>", unsafe_allow_html=True)
        else:
            st.write("No data available from sensors.")

    time.sleep(5)
