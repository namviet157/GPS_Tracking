import { initializeApp } from "https://www.gstatic.com/firebasejs/10.5.2/firebase-app.js";
import { getDatabase, ref, set, onValue } from "https://www.gstatic.com/firebasejs/10.5.2/firebase-database.js";

const firebaseConfig = {
  apiKey: "YOUR_FIREBASE_API_KEY",
  authDomain: "YOUR_FIREBASE_AUTH_DOMAIN",
  databaseURL: "YOUR_FIREBASE_DATABASE_URL",
  projectId: "YOUR_FIREBASE_PROJECT_ID",
  storageBucket: "YOUR_FIREBASE_STORAGE_BUCKET",
  messagingSenderId: "YOUR_FIREBASE_MESSAGING_SENDER_ID",
  appId: "YOUR_FIREBASE_APP_ID"
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

