import firebase_admin
from firebase_admin import db
from firebase_admin import credentials
import streamlit as st
import time



##################################### Some steps: #########################################
# 1. Go to your firebase RTDB.
# 2. Go to project settings , then Service accounts
# 3. In service accounts, click on generate new private key for python. Do this once (No worries if you do it again and again, just for security purposes)
# 4. Save this in the directory of your current app.py project
# 5. In the place where I have put 'secret.json', put this private key which is downloaded in json format.
# 6. Enter the URL of your database in 'databaseURL'
# 7. Your good to use my code now ! :)
# 8. To use the code, open cmd -> streamlit run filename.py


# Firebase Connection
if not firebase_admin._apps:
    cred = credentials.Certificate('secret.json')
    firebase_admin.initialize_app(cred, {
        'databaseURL': ""
    })

# ref1 = db.reference('spot1')
# ref2 = db.reference('spot2')
# print(ref1.get())
# print(ref2.get())



# Streamlit web interface
# st.set_page_config(layout="wide")
st.title("IKEA Parking Spot Availability")

def parking_spots_data():
    Spot1 = db.reference('spot1')
    Spot2 = db.reference('spot2')

    spot1_status = Spot1.get()
    spot2_status = Spot2.get()

    return spot1_status, spot2_status

placeholder = st.empty()

def update_parking_data():
    spot1, spot2 = parking_spots_data()

    with placeholder.container():
        st.subheader("Parking Spot Status")
        # st.write(f"**Spot 1:** {'Occupied' if not spot1 else 'Available'}")
        # st.write(f"**Spot 2:** {'Occupied' if not spot2 else 'Available'}")
        if not spot1:
            st.error(f'**Spot 1:** **Occupied**')
        else:
            st.success(f'**Spot 1:** **Available**')

        if not spot2:
            st.error(f'**Spot 2:** **Occupied**')
        else:
            st.success(f'**Spot 2:** **Available**')

while True:
    update_parking_data()
# st.experimental_rerun()
    time.sleep(5)