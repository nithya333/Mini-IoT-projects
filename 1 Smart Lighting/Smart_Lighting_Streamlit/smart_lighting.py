import streamlit as st
import firebase_admin
from firebase_admin import credentials, db
import time

def initialize_firebase():
    if not firebase_admin._apps:
        cred = credentials.Certificate('lights.json')  # Update this with your Firebase credentials
        firebase_admin.initialize_app(cred, {
            'databaseURL': ''  # Your database URL
        })

def get_smart_lighting_data():
    ref = db.reference('/')  # Access root level to get all data
    data = ref.get()  # Fetch the data
    return data 

# Initialize Firebase
initialize_firebase()

# Streamlit app content
st.set_page_config(page_title="Smart Lighting Dashboard", page_icon="💡")
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

st.title("Smart Lighting Dashboard")

placeholder = st.empty()

while True:
    data = get_smart_lighting_data()
    with placeholder.container():
        if data:
            ldr_value = data['LDR']  # Fetch LDR value from Firebase
            led_status = data['LED_STATUS']  # Fetch LED status from Firebase

            # Display sensor values in boxes
            col1, col2 = st.columns(2)
            with col1:
                st.markdown(f"<div class='sensor-box'>LDR Value: {ldr_value}</div>", unsafe_allow_html=True)  # LDR value box
            with col2:
                st.markdown(f"<div class='sensor-box'>LED Status: {led_status}</div>", unsafe_allow_html=True)  # LED status box

            # LED status logic
            if led_status == "ON":
                result_message = "💡 Lights are ON."
                st.markdown(f"<div class='result-box' style='background-color:green;'><span style='color:white;'>{result_message}</span></div>", unsafe_allow_html=True)
            else:
                result_message = "❌ Lights are OFF."
                st.markdown(f"<div class='result-box' style='background-color:red;'><span style='color:white;'>{result_message}</span></div>", unsafe_allow_html=True)
        else:
            st.write("No data available from sensors.")

        time.sleep(5)
