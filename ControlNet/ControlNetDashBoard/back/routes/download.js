const express = require('express');
const router = express.Router();
const path = require('path');
const fs = require('fs');

// GET /api/download/client - Download the client application
router.get('/client', (req, res) => {
  // Path to your client application executable
  // You should place the actual .exe file in this location
  const clientPath = path.join(__dirname, '..', 'downloads', 'ControlNet.exe');
  
  // Check if file exists
  if (!fs.existsSync(clientPath)) {
    return res.status(404).json({
      success: false,
      error: 'Client application not found. Please contact support.'
    });
  }
  
  // Get file stats for logging
  const stats = fs.statSync(clientPath);
  console.log(`📥 Download request for ControlNet.exe (${(stats.size / 1024 / 1024).toFixed(2)} MB)`);
  
  // Set headers for file download
  res.setHeader('Content-Type', 'application/octet-stream');
  res.setHeader('Content-Disposition', 'attachment; filename=ControlNet.exe');
  res.setHeader('Content-Length', stats.size);
  
  // Stream the file to the client
  const fileStream = fs.createReadStream(clientPath);
  fileStream.pipe(res);
  
  fileStream.on('error', (err) => {
    console.error('Error streaming file:', err);
    if (!res.headersSent) {
      res.status(500).json({
        success: false,
        error: 'Failed to download file'
      });
    }
  });
});

// GET /api/download/info - Get download information
router.get('/info', (req, res) => {
  const clientPath = path.join(__dirname, '..', 'downloads', 'ControlNet.exe');
  
  if (!fs.existsSync(clientPath)) {
    return res.json({
      success: false,
      available: false,
      message: 'Client application not yet available'
    });
  }
  
  const stats = fs.statSync(clientPath);
  
  res.json({
    success: true,
    available: true,
    filename: 'ControlNet.exe',
    size: stats.size,
    sizeFormatted: `${(stats.size / 1024 / 1024).toFixed(2)} MB`,
    platform: 'Windows',
    version: '1.0.0',
    lastModified: stats.mtime
  });
});

module.exports = router;
