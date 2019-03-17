import React, { Component } from 'react';
import deepcopy from 'deepcopy';

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      inputs: [],
      outputs: [],
      input: null,
      output: null,
      currentBank: 'A',
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
      let options = data[13 + i * 5 + 4];
      msgs[i] = {
        channel: data[13 + i * 5] & 0x0F,
        command: (data[13 + i * 5] & 0x70) >> 4,
        data1: data[13 + i * 5 + 1],
        data2: data[13 + i * 5 + 2],
        altData2: data[13 + i * 5 + 3],
        isToggle: (options & 0b00000001) !== 0,
        sendMidi: (options & 0b00000010) !== 0,
        sendPc: (options & 0b00000100) !== 0,
        sendUsb1: (options & 0b00001000) !== 0,
        sendUsb2: (options & 0b00010000) !== 0,
      }
    }
    this.setState({
      currentBank: String.fromCharCode(65 + data[5]),
      currentPatch: {
        idx: data[6],
        msgs,
        name: (data[7] !== 0 ? String.fromCharCode(data[7]) : '') +
              (data[8] !== 0 ? String.fromCharCode(data[8]) : '') +
              (data[9] !== 0 ? String.fromCharCode(data[9]) : '') +
              (data[10] !== 0 ? String.fromCharCode(data[10]) : '') +
              (data[11] !== 0 ? String.fromCharCode(data[11]) : '') +
              (data[12] !== 0 ? String.fromCharCode(data[12]) : '')
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
    let current = this.state.input;
    current.onmidimessage = null;
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
  onchannelchanged = (value, idx) => {
    let currentPatch = deepcopy(this.state.currentPatch);

    currentPatch.channel = value - 1;
    console.log(value, currentPatch);
    this.setState({
      currentPatch
    });
  }
  onnamechanged = (name) => {
    let currentPatch = deepcopy(this.state.currentPatch);
    currentPatch.name = name;
    this.setState({
      currentPatch
    });
  }
  save = () => {
    let sw = this.state.currentPatch;
    let msg = [0xF0, 0x66, 0x00, 0x00, 0x02];
    msg.push(sw.name.charCodeAt(0), sw.name.charCodeAt(1), sw.name.charCodeAt(2), sw.name.charCodeAt(3), sw.name.charCodeAt(4), sw.name.charCodeAt(5));
    for(let m of sw.msgs) {
      msg.push((m.command << 4) | m.channel, m.data1, m.data2, m.altData2);
      let options = m.isToggle ? 0b01 : 0;
      options |= m.sendMidi ? 0b10 : 0;
      options |= m.sendPc ? 0b100 : 0;
      options |= m.sendUsb1 ? 0b1000 : 0;
      options |= m.sendUsb2 ? 0b10000 : 0;
      msg.push(options);
    }
    msg.push(0xF7);
    this.state.output.send(msg);
  }
  render() {
    return (
      <div>
        <div style={{textAlign: 'center'}}>
        Input: <select onChange={this.oninputchange}>
          {this.state.inputs.map((item,idx) => {
            return <option key={idx} value={idx}>{item.name}</option>
          })}
        </select>&nbsp;
        Output: <select onChange={this.onoutputchange}>
          {this.state.outputs.map((item,idx) => {
            return <option key={idx} value={idx}>{item.name}</option>
          })}
        </select>
        </div>
        <div style={{fontSize: '3em', fontWeight: 'bold', textAlign: 'center'}}>
          {this.state.currentBank}{this.state.currentPatch.idx + 1}
        </div>
        {this.state.currentPatch.msgs.length > 0 ?
        <div style={{marginTop: 10, textAlign: 'center'}}>
          <div style={{textAlign: 'center'}}><input maxLength={6} value={this.state.currentPatch.name} onChange={ev => this.onnamechanged(ev.target.value)}/></div>
          <table>
            <thead>
              <tr><td>Midi</td><td>PC</td><td>Usb1</td><td>Usb2</td><td>Channel</td><td>Command</td><td>Data1</td><td>Data2</td><td>Toggle</td><td>Alt data2</td></tr>
            </thead>
            <tbody>
              {this.state.currentPatch.msgs.map((item, idx) =>
                <tr>
                  <td>
                    <input type="checkbox" checked={item.sendMidi}/>
                  </td>
                  <td>
                    <input type="checkbox" checked={item.sendPc}/>
                  </td>
                  <td>
                    <input type="checkbox" checked={item.sendUsb1}/>
                  </td>
                  <td>
                    <input type="checkbox" checked={item.sendUsb2}/>
                  </td>
                  <td><input type="number" min={1} max={16} step={1} value={item.channel + 1} onChange={ev => this.onchannelchanged(ev.target.value, idx)}/></td>
                  <td>
                    <select value={item.command}>
                      <option value={0}>Empty</option>
                      <option value={1}>Program change</option>
                      <option value={2}>Control change</option>
                    </select>
                  </td>
                  <td><input type="number" min={0} max={127} value={item.data1}/></td>
                  <td><input type="number" min={0} max={127} value={item.data2}/></td>
                  <td>
                    <input type="checkbox" checked={item.isToggle}/>
                  </td>
                  <td><input type="number" min={0} max={127} value={item.altData2} disabled={!item.isToggle}/></td>
                </tr>
              )}
            </tbody>
          </table>
          <div>
            <button onClick={this.save}>Save</button>
            <button onClick={this.get}>Reload</button>
          </div>
        </div>
        : <div style={{textAlign: 'center', marginTop: 10}}>No data available. Choose proper MIDI devices and click below button<br/><button onClick={this.get}>Load from device</button></div>}
      </div>
    );
  }
}

export default App;
