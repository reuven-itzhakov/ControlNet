const express = require('express');
const cors = require('cors');
require('dotenv').config();

// Import routes
const authRoutes = require('./routes/auth');
const deviceRoutes = require('./routes/devices');
const sequenceRoutes = require('./routes/sequences');
const statsRoutes = require('./routes/stats');
const downloadRoutes = require('./routes/download');

// Import middleware
const { errorHandler, logger } = require('./middleware');

const app = express();
const PORT = process.env.PORT || 5000;

// Global middleware
app.use(cors());
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(logger); // Request logging

// Basic routes
app.get('/', (req, res) => {
  res.json({ 
    message: 'KeyTracker API is running!',
    timestamp: new Date().toISOString(),
    version: '1.0.0'
  });
});

app.get('/api/health', (req, res) => {
  res.json({ 
    status: 'OK',
    message: 'Server is healthy',
    timestamp: new Date().toISOString()
  });
});

// API routes
app.use('/api/auth', authRoutes);
app.use('/api/devices', deviceRoutes);
app.use('/api/sequences', sequenceRoutes);
app.use('/api/stats', statsRoutes);
app.use('/api/download', downloadRoutes);

// Error handling middleware (must be last)
app.use(errorHandler);

// Start server
app.listen(PORT, '0.0.0.0', () => {
  console.log(`🚀 KeyTracker API is running on http://localhost:${PORT}`);
  console.log(`🌐 Network access: http://<your-ip>:${PORT}`);
  console.log(`📊 Server started at ${new Date().toISOString()}`);
  console.log(`🔑 Available endpoints:`);
  console.log(`   GET  /api/health - Health check`);
  console.log(`   POST /api/auth/register - User registration (Firestore)`);
  console.log(`   GET  /api/devices - Get user devices`);
  console.log(`   POST /api/devices - Add new device`);
  console.log(`   GET  /api/sequences/:deviceId - Get keystroke sequences for device`);
  console.log(`   GET  /api/sequences - Get all sequences for user`);
  console.log(`   POST /api/sequences - Log new keystroke sequence`);
  console.log(`   GET  /api/stats/:deviceId - Get device statistics`);
  console.log(`   GET  /api/download/client - Download client application`);
  console.log(`   GET  /api/download/info - Get download information`);
  console.log(`📁 Server structure: Modular routes for better performance`);
});