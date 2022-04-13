import React, { useEffect, useState } from "react";
import axios from "axios";
import "./App.css";
import "@material-tailwind/react/tailwind.css";

const App = () => {
  const [data, setData] = useState("");
  const [heating, setHeating] = useState(false);
  const [heated, setHeated] = useState(false);


  // Third Attempts
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
    getStateRequest("http://192.168.88.241:5643/press_button_1");
    if(!heated) {
      setHeating(true);
      setTimeout(() => {
        setHeating(false);
        setHeated(true);
      }, 30000);

    }
  };

  const shortCoffee = () => {
    getStateRequest("http://192.168.88.241:5643/press_button_2");
    if(!heated) {
      setData("Heating");
      setHeating(true);
      setTimeout(() => {
        setData("Hot");
        setHeating(false);
        setHeated(true);
      }, 30000);
    }
  };

  const turnOff = () => {
    if(heated){
      getStateRequest("http://192.168.88.241:5643/turn_off");
      setHeating(false);
      setHeated(false);
    }else{
      alert("The Coffee Machine isn't on");
    }
  };
  
  useEffect(() => {
    getStateRequest("http://192.168.88.241:5643/get_state");
    if(data == "Hot"){
      setHeated(true);
    }
    const interval = setInterval(() => {
      getStateRequest("http://192.168.88.241:5643/get_state");
      console.log("Get State Request");
    }, 5 * 60000);
    return () => clearInterval(interval);
  }, [data]);

  return (
    <div className="App h-full bg-neutral-100 dark:bg-slate-900 text-black dark:text-white">
        <br/><span className="align-middle font-medium leading-tight text-4xl mt-0 mb-2 text-blue-600">{heating?"Heating":data}</span><br/>
        <button disabled={heating} className="button bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white" onClick={() => shortCoffee()}>Espresso</button><br/>
        <button disabled={heating} className="button bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white" onClick={() => longCoffee()}>Long Coffee</button><br/>
        <button disabled={heating} className="button bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white" onClick={() => turnOff()}>Turn off</button><br/>
    </div>
  );
};

export default App;
