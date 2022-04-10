import React, { useEffect, useState } from "react";
import axios from "axios";
import "./App.css";
// class App extends Component {
//   render() {
//     return (
//       <div className="App">
//         <header className="App-header">
//           <div
//             /* style={{"backgroundImage": "url(" + day.hdurl + ")"}} */ className="shit flex justify-center items-center fixed w-screen h-screen"
//           >
//             <button className="py-2 px-4 m-auto w-1/5 bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white transition ease-in duration-200 text-center text-base font-semibold shadow-md focus:outline-none focus:ring-2 focus:ring-offset-2  rounded-lg ">
//               Start Chat
//             </button>
//           </div>
//         </header>
//       </div>
//     );
//   }
// }


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

  const getDataRequest = async (url) => {
    axios.get(url, {
      headers: {
        'Access-Control-Allow-Origin': '*',
        Accept: "application/json",
      },
      crossdomain: true,
    })
    .then(response => {
      console.log(response.data);
    }, error => {
      console.log(error);
    });
  };

  const longCoffee = () => {
    getDataRequest("http://192.168.88.241:8080/press_button_1");
    fetchsCFState();
  };

  const shortCoffee = () => {
    getDataRequest("http://192.168.88.241:8080/press_button_2");
    fetchsCFState();
  };

  const fetchsCFState = () => {
    getStateRequest("http://192.168.88.241:8080/fuck_cors");
  };
  
  useEffect(() => {
    // GET request using fetch inside useEffect React hook
    axios.get("http://192.168.88.241:8080/fuck_cors", {
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
    <div className="divide-y-2 h-screen">
      <div className="flex h-1/5 justify-center items-center w-full bg-slate-800">
        <span className="align-middle font-medium leading-tight text-4xl mt-0 mb-2 text-blue-600">
        {data}
        </span>
      </div>
      <form onSubmit={shortCoffee} className="flex h-2/5">
        <div
          /* style={{"backgroundImage": "url(" + day.hdurl + ")"}} */ className="shit flex justify-center items-center w-full h-full bg-slate-800"
        >
          <button
            type="submit"
            className="py-2 px-4 m-auto w-1/5 bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white transition ease-in duration-200 text-center text-base font-semibold shadow-md focus:outline-none focus:ring-2 focus:ring-offset-2  rounded-lg "
          >
            Short Coffee
          </button>
        </div>
      </form>
      <form onSubmit={longCoffee} className="flex h-2/5">
        <div
          /* style={{"backgroundImage": "url(" + day.hdurl + ")"}} */ className="shit flex justify-center items-center w-full h-full bg-slate-800"
        >
          <button
            type="submit"
            className="py-2 px-4 m-auto w-1/5 bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white transition ease-in duration-200 text-center text-base font-semibold shadow-md focus:outline-none focus:ring-2 focus:ring-offset-2  rounded-lg "
          >
            Long Coffee
          </button>
        </div>
      </form>
    </div>
  );
};

export default App;
