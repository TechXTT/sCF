import React, { useState } from "react";
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
  const [lamp, setLamp] = useState({});

  const getData = (url) => {
    console.log(url);
    fetch(url, {
      methods: "GET",
      headers: {
        "Content-Type": "application/json",
        "Access-Control-Allow-Origin": "*",
        "Access-Control-Allow-Headers": "*",
      },
    }).then(function (response) {
      console.log(response);
    });
  };

  const handleSubmit1 = (event) => {
    getData("http://192.168.88.143/?pin=1");
    setLamp("");
    event.preventDefault();
  };

  const handleSubmit2 = (event) => {
    getData("http://192.168.88.143/?pin=2");
    setLamp("");
    event.preventDefault();
  };

  return (
    <div class="divide-y-2 h-screen">
      <form onSubmit={handleSubmit1} className="flex h-1/2">
        <div
          /* style={{"backgroundImage": "url(" + day.hdurl + ")"}} */ className="shit flex justify-center items-center w-full h-full bg-slate-800"
        >
          <button
            type="submit"
            className="py-2 px-4 m-auto w-1/5 bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white transition ease-in duration-200 text-center text-base font-semibold shadow-md focus:outline-none focus:ring-2 focus:ring-offset-2  rounded-lg "
          >
            LED1
          </button>
        </div>
      </form>
      <form onSubmit={handleSubmit2} className="flex h-1/2">
        <div
          /* style={{"backgroundImage": "url(" + day.hdurl + ")"}} */ className="shit flex justify-center items-center w-full h-full bg-slate-800"
        >
          <button
            type="submit"
            className="py-2 px-4 m-auto w-1/5 bg-indigo-600 hover:bg-indigo-700 focus:ring-indigo-500 focus:ring-offset-indigo-200 text-white transition ease-in duration-200 text-center text-base font-semibold shadow-md focus:outline-none focus:ring-2 focus:ring-offset-2  rounded-lg "
          >
            LED2
          </button>
        </div>
      </form>
    </div>
  );
};

export default App;
