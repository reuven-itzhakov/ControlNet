const express = require('express');
const router = express.Router();
const { userService } = require('../services/firestore');

// POST /api/auth/register
router.post('/register', async (req, res) => {
  const { email } = req.body;
  
  if (!email) {
    return res.status(400).json({
      success: false,
      error: 'Email is required'
    });
  }
  
  try {
    const result = await userService.registerUser(email);
    
    if (result.success) {
      res.json(result);
    } else {
      res.status(400).json(result);
    }
  } catch (error) {
    console.error('Error in register route:', error);
    res.status(500).json({
      success: false,
      error: 'Failed to create user'
    });
  }
});

module.exports = router;