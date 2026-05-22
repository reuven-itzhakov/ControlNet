// Mock data storage (in production, use a real database)
// This centralizes all data for easy access across different route modules

let users = [
  {
    id: 1,
    email: 'demo@keytracker.com',
    password: 'demo123',
    name: 'Demo User',
    devices: [
      { id: 1, name: 'Main Laptop', type: 'laptop', lastActive: new Date().toISOString(), status: 'active' },
      { id: 2, name: 'Work Computer', type: 'desktop', lastActive: new Date().toISOString(), status: 'inactive' }
    ]
  }
];

let keystrokeSequences = [
  { 
    id: 1, 
    deviceId: 1, 
    userId: 1, 
    sequence: 'hello world', 
    timestamp: new Date().toISOString(), 
    length: 11,
    type: 'text',
    application: 'VSCode'
  },
  { 
    id: 2, 
    deviceId: 1, 
    userId: 1, 
    sequence: 'npm install react', 
    timestamp: new Date().toISOString(), 
    length: 17,
    type: 'command',
    application: 'Terminal'
  },
  { 
    id: 3, 
    deviceId: 1, 
    userId: 1, 
    sequence: 'const myVariable = 123', 
    timestamp: new Date().toISOString(), 
    length: 22,
    type: 'code',
    application: 'VSCode'
  },
  { 
    id: 4, 
    deviceId: 1, 
    userId: 1, 
    sequence: 'git commit -m "fix"', 
    timestamp: new Date().toISOString(), 
    length: 19,
    type: 'command',
    application: 'Terminal'
  },
  { 
    id: 5, 
    deviceId: 1, 
    userId: 1, 
    sequence: 'password123', 
    timestamp: new Date().toISOString(), 
    length: 11,
    type: 'password',
    application: 'Browser'
  },
];

// Export data for use in routes
module.exports = {
  users,
  keystrokeSequences
};