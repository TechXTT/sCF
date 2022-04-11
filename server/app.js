// index.js
const express = require("express");
const app = express();
var cors = require("cors");
const axios = require("axios");

app.use(cors());
app.get("/get_state", async (req, res) => {
  const url = "http://192.168.88.200:80/?command=2";
  axios.defaults.headers.get["Access-Control-Allow-Origin"] = "*";

  axios
    .get(url, {
      headers: {
        "Access-Control-Allow-Origin": "*",
        Accept: "application/json",
      },
      crossdomain: true,
    })
    .then(
      (response) => {
        res.send(response.data);
      },
      (error) => {
        console.log(error);
      }
    );
});

app.get("/press_button_1", async (req, res) => {
  const url = "http://192.168.88.200:80/?command=11";
  axios.defaults.headers.get["Access-Control-Allow-Origin"] = "*";

  axios
    .get(url, {
      headers: {
        "Access-Control-Allow-Origin": "*",
        Accept: "application/json",
      },
      crossdomain: true,
    })
    .then(
      (response) => {
        res.send(response.data);
      },
      (error) => {
        console.log(error);
      }
    );
});

app.get("/press_button_2", async (req, res) => {
  const url = "http://192.168.88.200:80/?command=12";
  axios.defaults.headers.get["Access-Control-Allow-Origin"] = "*";

  axios
    .get(url, {
      headers: {
        "Access-Control-Allow-Origin": "*",
        Accept: "application/json",
      },
      crossdomain: true,
    })
    .then(
      (response) => {
        res.send(response.data);
      },
      (error) => {
        console.log(error);
      }
    );
});

app.listen(5643, () => console.log(`Started server at http://localhost:5643!`));
