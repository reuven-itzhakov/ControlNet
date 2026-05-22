import axios from 'axios';

// Create axios instance with default configuration
const api = axios.create({
  baseURL: 'http://localhost:5000/api',
  timeout: 10000,
  headers: {
    'Content-Type': 'application/json',
  }
});

// Get user email from localStorage (set by AuthContext)
const getUserEmail = () => {
  return localStorage.getItem('userEmail') || '';
};

// Request interceptor - add auth token and user email if available
api.interceptors.request.use(
  (config) => {
    // Add user email to headers
    const userEmail = getUserEmail();
    if (userEmail) {
      config.headers['useremail'] = userEmail;
    }
    
    // Add Firebase auth token here if needed in the future
    // const token = localStorage.getItem('firebaseToken');
    // if (token) {
    //   config.headers.Authorization = `Bearer ${token}`;
    // }
    return config;
  },
  (error) => {
    return Promise.reject(error);
  }
);

// Response interceptor - handle common errors
api.interceptors.response.use(
  (response) => {
    return response;
  },
  (error) => {
    // Handle common HTTP errors
    if (error.response) {
      // Server responded with error status
      console.error('API Error:', error.response.status, error.response.data);
    } else if (error.request) {
      // Request was made but no response received
      console.error('Network Error:', error.request);
    } else {
      // Something else happened
      console.error('Error:', error.message);
    }
    return Promise.reject(error);
  }
);

// Auth API calls
export const authAPI = {
  register: async (email) => {
    const response = await api.post('/auth/register', { email });
    return response.data;
  },
};

// Device API calls
export const deviceAPI = {
  // Get all devices for a user
  getDevices: async (email) => {
    const userEmail = email || getUserEmail();
    const response = await api.get('/devices', {
      params: { email: userEmail }
    });
    return response.data;
  },
  
  // Get a single device
  getDevice: async (deviceId) => {
    const response = await api.get(`/devices/${deviceId}`);
    return response.data;
  },
  
  // Add a new device
  addDevice: async (deviceId, deviceData) => {
    const response = await api.post('/devices', { deviceId, deviceData });
    return response.data;
  },
  
  // Update a device
  updateDevice: async (deviceId, updates) => {
    const response = await api.put(`/devices/${deviceId}`, updates);
    return response.data;
  },
  
  // Delete a device
  deleteDevice: async (deviceId) => {
    const response = await api.delete(`/devices/${deviceId}`);
    return response.data;
  },
  
  // Update device last active timestamp
  updateLastActive: async (deviceId) => {
    const response = await api.put(`/devices/${deviceId}/last-active`);
    return response.data;
  },
};

// Sequence API calls
export const sequenceAPI = {
  // Get keystroke sequences for a device on a specific date
  getSequences: async (deviceId, date) => {
    const response = await api.get(`/sequences/${deviceId}`, {
      params: { date }
    });
    return response.data;
  },
  
  // Get all dates with keystroke data for a device
  getDeviceDates: async (deviceId) => {
    const response = await api.get(`/sequences/${deviceId}/dates`);
    return response.data;
  },
  
  // Get keystroke statistics for a device and date
  getDeviceStats: async (deviceId, date) => {
    const response = await api.get(`/sequences/${deviceId}/stats`, {
      params: { date }
    });
    return response.data;
  },
  
  // Log new keystroke sequence
  addKeystroke: async (deviceId, date, appName, timestamp, text) => {
    const response = await api.post('/sequences', {
      deviceId,
      date,
      appName,
      timestamp,
      text
    });
    return response.data;
  },
};

// Screenshot API calls
export const screenshotAPI = {
  // Get screenshots for a device on a specific date
  getScreenshots: async (deviceId, date) => {
    const response = await api.get(`/sequences/${deviceId}/screenshots`, {
      params: { date }
    });
    return response.data;
  },
  
  // Get all dates with screenshot data for a device
  getScreenshotDates: async (deviceId) => {
    const response = await api.get(`/sequences/${deviceId}/screenshots/dates`);
    return response.data;
  },
};

// Stats API calls
export const statsAPI = {
  getStats: async (deviceId, date) => {
    const response = await api.get(`/stats/${deviceId}`, {
      params: { date }
    });
    return response.data;
  },
};

// Health check
export const healthAPI = {
  check: async () => {
    const response = await api.get('/health');
    return response.data;
  },
};

// Download API calls
export const downloadAPI = {
  // Get download information
  getInfo: async () => {
    const response = await api.get('/download/info');
    return response.data;
  },
  
  // Get download URL
  getDownloadUrl: () => {
    return `${api.defaults.baseURL}/download/client`;
  },
};

export default api;