// Simple static file server for Toyota Tacoma Simulator
// Run: node server/server.js
// Then open: http://localhost:3000

const http = require('http');
const fs   = require('fs');
const path = require('path');

const PORT    = 3000;
const GAME_DIR = path.join(__dirname, '..', 'game');

const MIME = {
  '.html': 'text/html',
  '.js':   'application/javascript',
  '.css':  'text/css',
  '.json': 'application/json',
  '.png':  'image/png',
  '.jpg':  'image/jpeg',
  '.gif':  'image/gif',
  '.svg':  'image/svg+xml',
  '.glb':  'model/gltf-binary',
  '.gltf': 'model/gltf+json',
  '.hdr':  'application/octet-stream',
  '.mp3':  'audio/mpeg',
  '.wav':  'audio/wav',
};

http.createServer((req, res) => {
  let urlPath = req.url === '/' ? '/index.html' : req.url;
  // Strip query strings
  urlPath = urlPath.split('?')[0];

  const filePath = path.join(GAME_DIR, urlPath);

  fs.readFile(filePath, (err, data) => {
    if (err) {
      res.writeHead(404, { 'Content-Type': 'text/plain' });
      res.end('404 Not Found: ' + urlPath);
      return;
    }
    const ext  = path.extname(filePath).toLowerCase();
    const mime = MIME[ext] || 'application/octet-stream';
    res.writeHead(200, {
      'Content-Type': mime,
      'Cache-Control': 'no-cache',
    });
    res.end(data);
  });
}).listen(PORT, () => {
  console.log(`\n  Toyota Tacoma Simulator running at:`);
  console.log(`  http://localhost:${PORT}\n`);
  console.log(`  To expose to the internet, run:`);
  console.log(`  cloudflared tunnel --url http://localhost:${PORT}\n`);
});
