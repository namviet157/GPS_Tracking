import { initializeApp } from "https://www.gstatic.com/firebasejs/10.5.2/firebase-app.js";
import { getDatabase, ref, set, onValue } from "https://www.gstatic.com/firebasejs/10.5.2/firebase-database.js";

const firebaseConfig = {
  apiKey: "AIzaSyDMRQvPf9OZkhp1sO-WjvwW4kjRC07B3Ek",
  authDomain: "gps-test-68ee2.firebaseapp.com",
  databaseURL: "https://gps-test-68ee2-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "gps-test-68ee2",
  storageBucket: "gps-test-68ee2.firebasestorage.app",
  messagingSenderId: "615321729912",
  appId: "1:615321729912:web:725acd118500d77e33a68d"
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

const alertToggle = document.querySelector("#alert-toggle");
const autoToggle = document.querySelector("#auto-toggle");
const alertRingToggle = document.querySelector("#alert-ring-toggle");

onValue(ref(database, "alert"), (snapshot) => {
  alertToggle.checked = snapshot.val();
  localStorage.setItem("alert_toggle", alertToggle.checked ? "on" : "off");
});

onValue(ref(database, "auto"), (snapshot) => {
  autoToggle.checked = snapshot.val();
  localStorage.setItem("auto_toggle", autoToggle.checked ? "on" : "off");
});

onValue(ref(database, "alertRing"), (snapshot) => {
  alertRingToggle.checked = snapshot.val();
  localStorage.setItem("alert_ring_toggle", alertRingToggle.checked ? "on" : "off");
});

alertToggle.onclick = function(event) {
    set(ref(database, "alert"), event.target.checked)
        .catch((err) => {
            console.log(err);
        })
};

autoToggle.onclick = function(event) {
    set(ref(database, "auto"), event.target.checked)
        .catch((err) => {
            console.log(err);
        })
};

alertRingToggle.onclick = function(event) {
    set(ref(database, "alertRing"), event.target.checked)
        .catch((err) => {
            console.log(err);
        })
}

// database: {
//     posBike: {lat, lon},
//     vibrationData: [
//         {
//             hour,
//             numberOfVibration
//         }
//     ],
//     alert,
//     auto,
//     alertRing
// }

// set(ref(database, "name"), "Truong hoang lam");
// onValue(ref(database, "name"), (snapshot) => {
//   const data = snapshot.val();
//   console.log("Data:", data);
// });

