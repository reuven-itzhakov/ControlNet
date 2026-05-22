const express = require('express');
const router = express.Router();
const { deviceService } = require('../services/firestore');

// GET /api/devices - Get all devices for a user
router.get('/', async (req, res) => {
  const userEmail = req.headers.useremail || req.query.email;
  
  if (!userEmail) {
    return res.status(400).json({
      success: false,
      error: 'User email is required (provide via useremail header or email query param)'
    });
  }
  
  try {
    const result = await deviceService.getUserDevices(userEmail);
    res.json(result);
  } catch (error) {
    console.error('Error in get devices route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to fetch devices'
    });
  }
});

// GET /api/devices/:deviceId - Get a single device
router.get('/:deviceId', async (req, res) => {
  const { deviceId } = req.params;
  
  try {
    const result = await deviceService.getDevice(deviceId);
    
    if (result.success) {
      res.json(result);
    } else {
      res.status(404).json(result);
    }
  } catch (error) {
    console.error('Error in get device route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to fetch device'
    });
  }
});

// POST /api/devices - Add a new device
router.post('/', async (req, res) => {
  const { deviceId, deviceData } = req.body;
  
  if (!deviceId || !deviceData) {
    return res.status(400).json({
      success: false,
      error: 'Device ID and device data are required'
    });
  }
  
  try {
    const result = await deviceService.addDevice(deviceId, deviceData);
    
    if (result.success) {
      res.json(result);
    } else {
      res.status(400).json(result);
    }
  } catch (error) {
    console.error('Error in add device route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to add device'
    });
  }
});

// PUT /api/devices/:deviceId - Update a device
router.put('/:deviceId', async (req, res) => {
  const { deviceId } = req.params;
  const updates = req.body;
  
  try {
    const result = await deviceService.updateDevice(deviceId, updates);
    
    if (result.success) {
      res.json(result);
    } else {
      res.status(400).json(result);
    }
  } catch (error) {
    console.error('Error in update device route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to update device'
    });
  }
});

// DELETE /api/devices/:deviceId - Delete a device
router.delete('/:deviceId', async (req, res) => {
  const { deviceId } = req.params;
  
  try {
    const result = await deviceService.deleteDevice(deviceId);
    
    if (result.success) {
      res.json(result);
    } else {
      res.status(400).json(result);
    }
  } catch (error) {
    console.error('Error in delete device route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to delete device'
    });
  }
});

// PUT /api/devices/:deviceId/last-active - Update device last active timestamp
router.put('/:deviceId/last-active', async (req, res) => {
  const { deviceId } = req.params;
  
  try {
    const result = await deviceService.updateLastActive(deviceId);
    
    if (result.success) {
      res.json(result);
    } else {
      res.status(400).json(result);
    }
  } catch (error) {
    console.error('Error in update last active route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to update last active'
    });
  }
});

module.exports = router;