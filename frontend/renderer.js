// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const { SerialPort } = require('serialport')
const { contextBridge, ipcRenderer } = require('electron');
const canvas = document.getElementById('drawCanvas');
const ctx = canvas.getContext('2d');

const port = new SerialPort({
  path: '/dev/cu.DrawFrame',
  baudRate: 115200
})

port.on('open', () => {
  console.log('Serial Port Opened');
  port.write('-');
})

port.on('close', () => {
  console.log('Serial port closed.');
});

window.addEventListener('beforeunload', (event) => {
  if (port.isOpen) {
    port.write('p');
    port.close(() => {
      console.log('Serial port closed due to program abort');
    });
  }
});

function s1() {
  port.write('1');
}

function s0() {
  port.write('0');

}

function sn() {
  port.write('\n');
}


//Reading in canvas code:
ctx.fillStyle = 'white';
ctx.fillRect(0, 0, canvas.width, canvas.height);
ctx.strokeStyle = 'black';
ctx.lineWidth = 1;

let drawing = false;

// Start drawing
canvas.addEventListener('mousedown', (e) => {
  drawing = true;
  ctx.beginPath();
  ctx.moveTo(e.offsetX, e.offsetY);
});

// Draw lines on mouse move
canvas.addEventListener('mousemove', (e) => {
  if (drawing) {
      ctx.lineTo(e.offsetX, e.offsetY);
      ctx.stroke();
  }
});

// Set mode to draw or erase
function setMode(newMode) {
  mode = newMode;
  if (mode === 'draw') {
      ctx.strokeStyle = 'black';
      ctx.lineWidth = 3;
  } else if (mode === 'erase') {
      ctx.strokeStyle = 'white';
      ctx.lineWidth = 30;
  }
}

// Stop drawing
canvas.addEventListener('mouseup', () => drawing = false);
canvas.addEventListener('mouseleave', () => drawing = false);




async function processCanvas() {
  const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
  const binaryData = [];

  // Loop through each pixel
  for (let i = 0; i < imageData.data.length; i += 4) {
      const r = imageData.data[i];
      const g = imageData.data[i + 1];
      const b = imageData.data[i + 2];

      // Check if pixel is white (1) or black (0)
      const isWhite = r === 255 && g === 255 && b === 255;
      binaryData.push(isWhite ? '1' : '0');
  }

  // Define chunk size and initialize offset
  const chunkSize = 512; // Adjust based on optimal transmission size
  let offset = 0;

  // Start transmission by sending an initial signal if needed
  port.write('\n'); // Sending initial byte, if required

  // Function to send the next chunk
  async function sendNextChunk() {
    toggleUpload(false);
      if (offset >= binaryData.length) {
          console.log('All data sent');
          toggleUpload(true);
          return;
      }

      // Slice the data for the current chunk
      const chunk = binaryData.slice(offset, offset + chunkSize).join('');
      
      // Write the chunk to the port
      port.write(chunk, (err) => {
          if (err) {
              console.error('Error sending chunk:', err);
              return;
          }
          console.log(`Chunk sent: ${offset} to ${offset + chunk.length}`);
      });

      // Increment offset and delay before sending the next chunk
      offset += chunkSize;
      await delay(35); // Delay in ms if necessary; adjust based on performance
      sendNextChunk();
  }

  // Start sending chunks
  sendNextChunk();

}

async function delay(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}


function toggleUpload(toggle) {
  document.getElementById('submit').disabled = !toggle;
  document.getElementById('submit').innerText = toggle ? "Send to DrawFrame" : "Uploading...";

}