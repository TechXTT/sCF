import React, { useEffect, useState } from "react";
import axios from "axios";
import "./App.css";
import "@material-tailwind/react/tailwind.css";


const App = () => {
  const [data, setData] = useState("");

  const getStateRequest = async (url) => {
    axios.get(url, {
      headers: {
        'Access-Control-Allow-Origin': '*',
        Accept: "application/json",
      },
      crossdomain: true,
    })
    .then(response => {
      console.log(response.data.sCFState);
      setData(response.data.sCFState);
    }, error => {
      console.log(error);
    });
  };

  const longCoffee = () => {
    getStateRequest("http://192.168.88.241:5643/press_button_1")
  };

  const shortCoffee = () => {
    getStateRequest("http://192.168.88.241:5643/press_button_2")
  };
  
  useEffect(() => {
    // GET request using fetch inside useEffect React hook
    axios.get("http://192.168.88.241:5643/get_state", {
      headers: {
        'Access-Control-Allow-Origin': '*',
        Accept: "application/json",
      },
      crossdomain: true,
    })
    .then(response => {
      console.log(response.data.sCFState);
      setData(response.data.sCFState);
    }, error => {
      console.log(error);
    });
  
  // empty dependency array means this effect will only run once (like componentDidMount in classes)
  }, [data]);

  return (
    <div className="App h-full bg-neutral-100 dark:bg-slate-900 text-black dark:text-white">
        <br/><span className="align-middle font-medium leading-tight text-4xl mt-0 mb-2 text-blue-600">{data}</span><br/>
        <button className="button bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white" onClick={() => shortCoffee()}>Espresso</button><br/>
        <button className="button bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white" onClick={() => longCoffee()}>Long Coffee</button><br/>
    </div>
  );
};

export default App;