const axios = require("axios");

axios
  .get("http://170.20.10.5/?command=11")
  .then((res) => {
    console.log(`statusCode: ${res.status}`);
    console.log(res.data);
  })
  .catch((error) => {
    console.error(error);
  });
