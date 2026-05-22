const express = require('express');
const router = express.Router();
const { sequenceService } = require('../services/firestore');

// GET /api/stats/:deviceId - Get statistics for a device
router.get('/:deviceId', async (req, res) => {
  const { deviceId } = req.params;
  const { date } = req.query;
  
  if (!deviceId) {
    return res.status(400).json({
      success: false,
      error: 'Device ID is required'
    });
  }
  
  const targetDate = date || new Date().toISOString().split('T')[0];
  
  try {
    const result = await sequenceService.getDeviceStats(deviceId, targetDate);
    res.json(result);
  } catch (error) {
    console.error('Error in get stats route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to fetch statistics'
    });
  }
});

module.exports = router;