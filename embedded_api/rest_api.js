const express = require("express");
const app = express();

app.get("/", (req, res) => {
  res.end(JSON.stringify({ output: "heated" }));
});

app.listen(3000);
