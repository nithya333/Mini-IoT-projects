import firebase_admin
from firebase_admin import db
from firebase_admin import credentials
import streamlit as st
import time
# import datetime
import pandas as pd
import matplotlib.pyplot as plt

##################################### Some steps: #########################################
# 1. Go to your firebase RTDB.
# 2. Go to project settings , then Service accounts
# 3. In service accounts, click on generate new private key for python. Do this once (No worries if you do it again and again, just for security purposes)
# 4. Save this in the directory of your current app.py project
# 5. In the place where I have put 'weather.json', put this private key which is downloaded in json format.
# 6. Enter the URL of your database in 'databaseURL'
# 7. Your good to use my code now ! :)



# Firebase Connection
if not firebase_admin._apps:
    cred = credentials.Certificate('Weather.json')
    firebase_admin.initialize_app(cred, {
        'databaseURL': ""
    })

st.title("Weather Monitoring System")

# Main functions:
def weather_data():
    heatIndex = db.reference('Heat Index')
    humidity = db.reference('Humidity')
    temperature = db.reference('Temperature')

    Hi = heatIndex.get()
    Humi = humidity.get()
    Temp = temperature.get()

    return Hi, Humi, Temp

placeholder = st.empty()
graph_placeholder = st.empty()

def update_weather_data():
    hi, humi, temp = weather_data()

    with placeholder.container():
        st.subheader("Current Weather Conditions")
        col1, col2, col3 = st.columns(3)
        col1.metric("Temperature", f"{temp} °C")
        col2.metric("Heat Index", f"{hi} °C")
        col3.metric("Humidity", f"{humi} %")

    timestamp = pd.Timestamp.now()  # Get current timestamp

    # Append new data to the DataFrame in session state
    new_data = pd.DataFrame([[timestamp, temp, hi, humi]], columns=['Timestamp', 'Temperature', 'Heat Index', 'Humidity'])
    st.session_state.weather_df = pd.concat([st.session_state.weather_df, new_data], ignore_index=True)

    if not st.session_state.weather_df.empty:
        graph_placeholder.line_chart(st.session_state.weather_df.set_index('Timestamp'))

# Streamlit web interface
# st.set_page_config(layout="wide")

# Chatbot
def chatbot():
    with st.sidebar:
        hi, humi, temp = weather_data()

        st.subheader("Weather Chatbot")
        messages = st.container(height=350)
        user_input = st.chat_input("Get to know today's weather!", key="weather_input")

        if user_input:
            with messages:
                st.chat_message("user").write(f"User: {user_input}")
                st.chat_message("assistant").write(f"Echo: Hi! Current conditions : \n {temp} °C | {hi} °C | {humi} °C")
chatbot()

if 'weather_df' not in st.session_state:
    st.session_state.weather_df = pd.DataFrame(columns=['Timestamp', 'Temperature', 'Heat Index', 'Humidity'])

while True:
    update_weather_data()
    time.sleep(5)