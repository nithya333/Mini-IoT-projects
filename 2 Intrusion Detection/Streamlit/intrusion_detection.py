import firebase_admin
from firebase_admin import db
from firebase_admin import credentials
import streamlit as st
import time
import pygame
# from playsound import playsound as play
from streamlit_extras.let_it_rain import rain
from notifypy import Notify

# Pip installs
# firebase_admin, streamlit, pygame, streamlit-extras, notifypy

##################################### Some steps: #########################################

# 1. Go to your firebase RTDB.
# 2. Go to project settings , then Service accounts
# 3. In service accounts, click on generate new private key for python. Do this once (No worries if you do it again and again, just for security purposes)
# 4. Save this in the directory of your current app.py project
# 5. In the place where I have put 'secret.json', put this private key which is downloaded in json format.
# 6. Enter the URL of your database in 'databaseURL'
# 7. Your good to use my code now ! :)



# Firebase Connection
if not firebase_admin._apps:
    cred = credentials.Certificate('Intrusion.json')
    firebase_admin.initialize_app(cred, {
        
        'databaseURL': ""
    })

# def intrusion_check(key):
#     intr_data = db.reference(key)
#     intr = intr_data.get()
#     return intr

# Streamlit web interface
st.set_page_config(layout="wide")
st.title("Intrusion Detection 🕵🏻️")

placeholder = st.empty()

intr_flag = db.reference('Intrusion')
intr_status = db.reference('IntrusionStatus')
on = st.toggle('Are you at home ?')
placeholder = st.info(intr_status.get())


if on:
    st.warning('You are at home!')
    intr_flag.set(0)
    intr_status.set("All Clear!")
    placeholder.info(intr_status.get())

else:
    st.error('You are Outside!')
    while True:
        intr_detect = intr_flag.get()
        placeholder.info(intr_status.get())
        if intr_detect:
            # play('intruder_alarm.mp3')
            
            intr_status.set('Intruder at home!')
            
            notification = Notify()
            notification.title = "Intrusion Detection System"
            notification.message = "Intruder at Home!!"
            notification.send()

            rain(emoji="🕵🏻️", font_size=54,falling_speed=5,animation_length=1)
            pygame.mixer.init()
            pygame.mixer.music.load('Final_alarm.mp3')
            pygame.mixer.music.play()
            while pygame.mixer.music.get_busy():  # Wait until the music finishes playing
                pass
        
        else:
            intr_status.set('All Clear!')
            # continue
        time.sleep(5)
