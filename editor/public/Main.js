const {app, BrowserWindow} = require('electron')
const path = require('path')
const isDev = require('electron-is-dev')

function createWindow () {
  // Create the browser window.
let win = new BrowserWindow({width: 800, height: 600})
if (isDev) {
  win.loadURL('http://localhost:3000/')
} else {
  win.loadURL(`file://${path.join(__dirname, '../build/index.html')}`)
}
}

app.on('ready', createWindow)

