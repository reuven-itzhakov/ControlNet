const express = require('express');
const router = express.Router();
const { sequenceService, screenshotService } = require('../services/firestore');

// GET /api/sequences/:deviceId - Get keystroke sequences for a device
router.get('/:deviceId', async (req, res) => {
  const { deviceId } = req.params;
  const { date } = req.query;
  
  if (!date) {
    return res.status(400).json({
      success: false,
      error: 'Date parameter is required (format: YYYY-MM-DD)'
    });
  }
  
  try {
    const result = await sequenceService.getDeviceKeystrokes(deviceId, date);
    
    if (result.success) {
      res.json({
        success: true,
        sequences: result.keystrokes,
        total: result.keystrokes.length
      });
    } else {
      res.status(400).json(result);
    }
  } catch (error) {
    console.error('Error in get sequences route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to fetch sequences'
    });
  }
});

// GET /api/sequences/:deviceId/dates - Get all dates with keystroke data
router.get('/:deviceId/dates', async (req, res) => {
  const { deviceId } = req.params;
  
  try {
    const result = await sequenceService.getDeviceDates(deviceId);
    res.json(result);
  } catch (error) {
    console.error('Error in get dates route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to fetch dates'
    });
  }
});

// GET /api/sequences/:deviceId/stats - Get keystroke statistics
router.get('/:deviceId/stats', async (req, res) => {
  const { deviceId } = req.params;
  const { date } = req.query;
  
  if (!date) {
    return res.status(400).json({
      success: false,
      error: 'Date parameter is required (format: YYYY-MM-DD)'
    });
  }
  
  try {
    const result = await sequenceService.getDeviceStats(deviceId, date);
    res.json(result);
  } catch (error) {
    console.error('Error in get stats route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to fetch stats'
    });
  }
});

// POST /api/sequences - Log new keystroke sequence
router.post('/', async (req, res) => {
  const { deviceId, date, appName, timestamp, text } = req.body;
  
  if (!deviceId || !date || !appName || !timestamp || !text) {
    return res.status(400).json({
      success: false,
      error: 'deviceId, date, appName, timestamp, and text are required'
    });
  }
  
  try {
    const result = await sequenceService.addKeystroke(deviceId, date, appName, timestamp, text);
    
    if (result.success) {
      res.json(result);
    } else {
      res.status(400).json(result);
    }
  } catch (error) {
    console.error('Error in add keystroke route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to add keystroke'
    });
  }
});

// GET /api/sequences/:deviceId/screenshots - Get screenshots for a device
router.get('/:deviceId/screenshots', async (req, res) => {
  const { deviceId } = req.params;
  const { date } = req.query;
  
  if (!date) {
    return res.status(400).json({
      success: false,
      error: 'Date parameter is required (format: YYYY-MM-DD)'
    });
  }
  
  try {
    const result = await screenshotService.getDeviceScreenshots(deviceId, date);
    res.json(result);
  } catch (error) {
    console.error('Error in get screenshots route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to fetch screenshots'
    });
  }
});

// GET /api/sequences/:deviceId/screenshots/dates - Get all dates with screenshot data
router.get('/:deviceId/screenshots/dates', async (req, res) => {
  const { deviceId } = req.params;
  
  try {
    const result = await screenshotService.getScreenshotDates(deviceId);
    res.json(result);
  } catch (error) {
    console.error('Error in get screenshot dates route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to fetch screenshot dates'
    });
  }
});

module.exports = router;