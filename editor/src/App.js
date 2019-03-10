import React, { Component } from 'react';
import './App.css';

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      inputs: [],
      outputs: [],
      input: null,
      output: null,
      currentBank: 0,
      currentPatch: {
        idx: 0,
        name: '',
        msgs: []
      }
    }
  }
  async componentDidMount() {
    if (navigator.requestMIDIAccess) {
      var midi = await navigator.requestMIDIAccess({sysex: true});
      let inputs = [];
      for(let input of midi.inputs.values()) {
        if (input instanceof MIDIInput) {
          inputs.push(input);
        }
      }
      let outputs = [];
      for(let output of midi.outputs.values()) {
        if (output instanceof MIDIOutput) {
          outputs.push(output);
        }
      }
      this.setState({
        inputs, outputs, input: inputs.length > 0 ? inputs[0] : null, output: outputs.length > 0 ? outputs[0] : null
      });
      if (inputs.length > 0) {
        inputs[0].onmidimessage = this.onmidi;
      }

    }
  }
  get = () => {
    this.state.output.send([0xF0, 0x66, 0x00, 0x00, 0x01, 0xF7]);
  }
  populateData = (data) => {
    let msgs = [];
    for(let i = 0; i < 5; i++) {
      msgs[i] = {
        channel: data[13 + i * 3] & 0x0F,
        command: (data[13 + i * 3] & 0xF0) >> 4,
        data1: data[13 + i * 3 + 1],
        data2: data[13 + i * 3 + 2]
      }
    }
    this.setState({
      currentBank: data[5],
      currentPatch: {
        idx: data[6],
        msgs,
        name: String.fromCharCode(data[7])+String.fromCharCode(data[8])+String.fromCharCode(data[9])+String.fromCharCode(data[10])+String.fromCharCode(data[11])+String.fromCharCode(data[12])
      }
    });
  }
  onmidi = (msg) => {

    if (msg.data.length < 7) return;
    if (msg.data[0] !== 0xF0 || msg.data[1] !== 0x66 || msg.data[2] !== 0x00) return;
    console.log(msg.data);
    switch(msg.data[4]) {
      case 0x01:
        this.populateData(msg.data);
        break;
      default:
        console.warn("unsupported command");
    }
  }
  oninputchange = (ev) => {
    console.log(ev.target.value);
    this.state.input.onmidimessage = null;
    let input = this.state.inputs[ev.target.value];
    input.onmidimessage = this.onmidi;
    this.setState({
      input
    });
  }
  onoutputchange = (ev) => {
    this.setState({
      output: this.state.outputs[ev.target.value]
    });
  }
  render() {
    return (
      <div>
        <div>
        Input: <select onChange={this.oninputchange}>
          {this.state.inputs.map((item,idx) => {
            return <option key={idx} value={idx}>{item.name}</option>
          })}
        </select><br/>
        Output: <select onChange={this.onoutputchange}>
          {this.state.outputs.map((item,idx) => {
            return <option key={idx} value={idx}>{item.name}</option>
          })}
        </select>
        <button onClick={this.get}>Get</button>
        </div>
        <div>
          Current bank: {this.state.currentBank}<br/>
          Current patch: {this.state.currentPatch.idx} <input maxlength={6} value={this.state.currentPatch.name}/><br/>
        </div>
        <div>
          <table>
            <thead>
              <tr><td>Channel</td><td>Command</td><td>Data1</td><td>Data2</td></tr>
            </thead>
            <tbody>
              {this.state.currentPatch.msgs.map(item =>
                <tr><td><input type="number" value={item.channel}/></td>
                  <td><select value={item.command}>
                    <option value={1}>Program change</option>
                    <option value={2}>Control change</option>
                </select></td>
                <td><input type="number" value={item.data1}/></td>
                <td><input type="number" value={item.data2}/></td></tr>
              )}
            </tbody>
          </table>
        </div>
        <div>
          <button>Save</button>
        </div>
      </div>
    );
  }
}

export default App;
