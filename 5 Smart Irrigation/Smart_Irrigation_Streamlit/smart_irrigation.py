import firebase_admin
from firebase_admin import db
from firebase_admin import credentials
import streamlit as st
import time
# import datetime
import pandas as pd
# import matplotlib.pyplot as plt
import plotly.express as px



# Pip installs
# firebase_admin, streamlit, plotly, pandas

##################################### Some steps: #########################################
# 1. Go to your firebase RTDB.
# 2. Go to project settings , then Service accounts
# 3. In service accounts, click on generate new private key for python. Do this once (No worries if you do it again and again, just for security purposes)
# 4. Save this in the directory of your current app.py project
# 5. In the place where I have put 'weather.json', put this private key which is downloaded in json format.
# 6. Enter the URL of your database in 'databaseURL'
# 7. Your good to use my code now ! :)



############################## Firebase Connection ########################
if not firebase_admin._apps:
    cred = credentials.Certificate('smartIrrigation.json')
    firebase_admin.initialize_app(cred, {
        'databaseURL': ""
    })




####################################### Streamlit web dash #################################
st.set_page_config(layout='wide')
st.title("Smart Irrigation System")

# LED status for motor turn on off
led = db.reference('LED_STATUS')

# Webpage columns - 3
# left_co,center_co, right_co = st.columns(3)
with st.sidebar:
    st.header('Manual Entry Mode')
    on = st.toggle("Turn on motor",key=f"motor_toggle")
    if on:
            led.set('true')
            st.success('Motor On, Irrigation Started!')
            st.image('Irrigation.gif')
    else:
            led.set('false')
            st.error('Motor Off, Irrigation Stopped!')
            st.image('Irrigation_stop.jpg')


if 'irrigation_df' not in st.session_state:
    st.session_state.irrigation_df = pd.DataFrame(columns=['Timestamp', 'Moisture'])


################################### Main functions: ####################################

def irrigation_data():
    moisture = db.reference('moisture')
    moist = moisture.get()

    return moist

placeholder = st.empty()
graph_placeholder = st.empty()

def update_irrigation_data():
    # moist = irrigation_data()

    moist = irrigation_data()
    
    with placeholder.container():
        col1, col2, col3 = st.columns(3)
        with col1:
            st.subheader("Soil Conditions")
        with col2:
            st.warning(f"Moisture Value : {moist}")
        with col3:
            if moist < 2048:
                led.set('true')
                st.info('Stopping Water 🔅')
                # st.image('Irrigation.gif')
            else:
                led.set('false')
                st.info('Starting Water 💦')
                # st.image('Irrigation_stop.jpg')
    # on = st.toggle("Turn on motor",key=f"motor_toggle_{time.time()}") # Unique key created at each instance
    # The unique assignment is needed as a new toggle is button is created ech time the while loop runs
    # Hence unique key depending on timestamp 

    # with st.container():
    timestamp = pd.Timestamp.now()  # Get current timestamp

    # Append new data to the DataFrame in session state
    new_data = pd.DataFrame([[timestamp, moist]], columns=['Timestamp', 'Moisture'])
    st.session_state.irrigation_df = pd.concat([st.session_state.irrigation_df, new_data], ignore_index=True)

    if not st.session_state.irrigation_df.empty:
        # graph_placeholder.line_chart(st.session_state.irrigation_df.set_index('Timestamp'),use_container_width=False,width=700, height= 200)
        fig = px.line(st.session_state.irrigation_df, x='Timestamp', y='Moisture', title='Moisture Over Time')
        fig.update_traces(line=dict(color='darkolivegreen', width=3))
        fig.update_layout(width=700, height=400)  
        graph_placeholder.plotly_chart(fig, use_container_width=True)


# Final loop
while True:
    update_irrigation_data()
    time.sleep(1)